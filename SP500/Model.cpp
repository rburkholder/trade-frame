/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

/*
 * File:    Model.cpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: July 4, 2025 11:32:42
 */

//#include <algorithm>

#include <boost/log/trivial.hpp>

#include "LSTM.hpp"
#include "Model.hpp"
#include "HyperParameters.hpp"

namespace {
  static const size_t c_secondsSampleOffset( 23 );  // offset for each sample
  static const size_t c_secondsSequence( 210 ); // duration of sample sequence
  static const size_t c_secondsYOffset( 30 ); // attempt prediction this far in the future
}

Model::Model()
: m_iterDataScaled( m_vDataScaled.begin() )
{
}

Model::~Model() {
}

namespace {
  // from AutoTrade/NeuralNet.cpp
  template<int coef>
  inline double binary_sigmoid( double x ) { // 0.0 .. 1.0
    constexpr double k( -coef );
    return 1.0 / ( 1.0 + std::exp( x * k ) );
  }

  // from AutoTrade/NeuralNet.cpp
  template<int coef>
  inline double bipolar_sigmoid( double x ) { // -1.0 .. +1.0 (aka tanh)
    constexpr double k( -coef );
    const double ex = std::exp( x * k );
    return ( 1.0 - ex ) / ( 1.0 + ex );
  }

  // additional activiation functions:
  // https://machinelearninggeek.com/activation-functions/
}

namespace {
  struct maxmin {
    double& max;
    double& min;

    maxmin( double& max_, double& min_, double init )
    : max( max_ ), min( min_ ) {
      max = min = init;
    }

    void test( const double value ) {
      if ( max < value ) max = value;
      else {
        if ( min > value ) min = value;
      }
    }
  };
}

void Model::Append( const Features_raw& raw, Features_scaled& scaled ) {

  double max;
  double min;
  maxmin mm( max, min, raw.dblEma200 );
  mm.test( raw.dblEma050 );
  mm.test( raw.dblEma029 );
  mm.test( raw.dblEma013 );
  // on purpose: no test on price

  if ( max > min ) {

    const double range( max - min );

    // detrend timeseries to 0.0 - 1.0
    scaled.ema200 = ( ( raw.dblEma200 - min ) / range );
    scaled.ema050 = ( ( raw.dblEma050 - min ) / range );
    scaled.ema029 = ( ( raw.dblEma029 - min ) / range );
    scaled.ema013 = ( ( raw.dblEma013 - min ) / range );

    const double ratioPrice( ( ( raw.dblPrice - min ) / range ) * 2.0 - 1.0 ); // even scaling top and bottom
    const double sigmoidPrice( bipolar_sigmoid<3>( ratioPrice ) );
    scaled.price = ( sigmoidPrice * 0.5 + 0.5 ); // translate to 0.0 - 1.0

    const double sigmoidTickJ( bipolar_sigmoid<2>( raw.dblTickJ ) ); // even scaling top and bottom
    scaled.tickJ = ( sigmoidTickJ * 0.5 + 0.5 ); // translate to 0.0 - 1.0

    const double sigmoidTickL( bipolar_sigmoid<2>( raw.dblTickL ) ); // even scaling top and bottom
    scaled.tickL = ( sigmoidTickL * 0.5 + 0.5 ); // translate to 0.0 - 1.0

    scaled.AdvDec = ( raw.dblAdvDecRatio * 0.5 + 0.5 ); // translate to 0.0 - 1.0

    const fields_t<float> features_scaled(
      scaled.ema200.flt, scaled.ema050.flt, scaled.ema029.flt, scaled.ema013.flt
    , scaled.price.flt
    , scaled.tickJ.flt, scaled.tickL.flt
    //, 0.5 // dblAdvDec use neutral mid until multi-day series tackled
    );
    m_vDataScaled.push_back( features_scaled ); // will need to timestamp each entry

    if ( c_secondsSequence <= m_vDataScaled.size() ) {

      ++m_iterDataScaled;
    }

  }
  else {
    scaled.Zero();
  }

}

Model::pLSTM_t Model::Build( torch::DeviceType device, const HyperParameters& hp ) {

  BOOST_LOG_TRIVIAL(info)
    << "scaled vector size: "
           << m_vDataScaled.size() << "sec"
    << ',' << m_vDataScaled.size() / 60.0 << "min"
    << ',' << m_vDataScaled.size() / 3600.0 << "hr"
    ;

  BOOST_LOG_TRIVIAL(info)
           << "size of fields_t<float>: " << sizeof( fields_t<float> )
    << ',' << "size of fields_t<double>: " << sizeof( fields_t<double> )
    ;

  // references:
  //   https://github.com/pytorch/pytorch/issues/14000
  //   https://github.com/pytorch/pytorch/blob/main/tools/autograd/templates/variable_factories.h
  //   https://docs.pytorch.org/cppdocs/notes/tensor_creation.html
  //   https://docs.alcf.anl.gov/polaris/data-science/frameworks/libtorch/#linking-the-torch-libraries
  //   https://www.geeksforgeeks.org/deep-learning/long-short-term-memory-networks-using-pytorch/

  // using as a guide:
  //  https://machinelearningmastery.com/how-to-develop-lstm-models-for-time-series-forecasting/

  // Note: Y.length must be same as X.length, but Y can have different feature count

  static const size_t secondsTotal( c_secondsSequence + c_secondsYOffset ); // training + prediction

  static const int nOutputFeature( 1 );

  static const int nInputFeature( nInputFeature_ );
  static const size_t sizeFloat( sizeof( float ) );

  static const size_t nFieldBytes( nInputFeature * sizeFloat );
  assert( nFieldBytes == sizeof( fields_t<float> ) );

  const size_t nSamples_theory( m_vDataScaled.size() / c_secondsSampleOffset ); // assumes integer math with truncation

  long nSamples_actual {};
  vValuesFlt_t::size_type ixDataScaled {};

  vValuesFlt_t vSourceForTensorX; // implicit 3 dimensions:  [sample index][sample size in seconds][feature list]
  std::vector<float> vSourceForTensorY; // [samples match X][1 second for prediction][last index implies 1 feature]

  {
    vValuesFlt_t::const_iterator bgnX = m_vDataScaled.begin(); // begin of input
    vValuesFlt_t::const_iterator endX( bgnX + c_secondsSequence ); // end of inpu

    vValuesFlt_t::const_iterator bgnY( bgnX + c_secondsYOffset ); // start of prediction time frame
    vValuesFlt_t::const_iterator endY( bgnY + c_secondsSequence ); // length same as X sequence length

    while ( m_vDataScaled.size() > ( ixDataScaled + secondsTotal ) ) {

      // append X sequence
      std::copy( bgnX, endX, std::back_inserter( vSourceForTensorX ) );

      // append Y sequence
      std::for_each(
         bgnY, endY,
         [&vSourceForTensorY]( auto& entry ){
          vSourceForTensorY.push_back( entry.fields[ ixTrade ] );
        } );

      bgnX += c_secondsSampleOffset;
      endX += c_secondsSampleOffset;
      bgnY += c_secondsSampleOffset;
      endY += c_secondsSampleOffset;
      ixDataScaled += c_secondsSampleOffset;
      ++nSamples_actual;
    }
    // won't match for now:
    BOOST_LOG_TRIVIAL(info) << "nSamples: " << nSamples_actual << ',' << nSamples_theory;
  }

  BOOST_LOG_TRIVIAL(info)
    << "data usage: " << ixDataScaled << ',' << m_vDataScaled.size() << ',' << ixDataScaled + secondsTotal;
  BOOST_LOG_TRIVIAL(info)
    << "input samples * (time steps in each sample): "
    << nSamples_actual
    << ',' << c_secondsSequence
    << '=' << '(' << vSourceForTensorX.size()
    << ','        << vSourceForTensorY.size()
           << ')'
    ;

  // note: from_blob does not manage memory, so underlying needs to be valid during lifetime of tensor

  torch::Tensor tensorX = // input
    torch::from_blob( vSourceForTensorX.data(), { nSamples_actual, c_secondsSequence, nInputFeature },
    torch::TensorOptions().dtype( torch::kFloat32 )
  ).to( device ); // without .clone(), data source remains in the vector
  BOOST_LOG_TRIVIAL(info) << "batched tensorX sizes: " << tensorX.sizes();

  torch::Tensor tensorY = // output
    torch::from_blob( vSourceForTensorY.data(), { nSamples_actual, c_secondsSequence, nOutputFeature },
    torch::TensorOptions().dtype( torch::kFloat32 )
  ).to( device ); // without .clone(), data source remains in the vector
  BOOST_LOG_TRIVIAL(info) << "batched tensorY sizes: " << tensorY.sizes();


  // https://github.com/pytorch/pytorch
  // https://docs.pytorch.org/docs/stable/torch.html
  // https://docs.pytorch.org/cppdocs/api/classat_1_1_tensor.html

  //BOOST_LOG_TRIVIAL(info) << "tensor: " << tensor;

  // building the time series, need to fix the tensor based upon:
  // https://machinelearningmastery.com/how-to-develop-lstm-models-for-time-series-forecasting/
  // https://machinelearningmastery.com/lstm-for-time-series-prediction-in-pytorch/
  // * The power of an LSTM cell depends on the size of the hidden state or cell memory,
  //   which usually has a larger dimension than the number of features in the input.

  // Hyperparameters
  const int batch_size( nSamples_actual );
  const int input_size( nInputFeature );
  const int hidden_size( nInputFeature * 9 );
  const int output_size( nOutputFeature );
  const int sequence_length( c_secondsSequence );
  const int num_layers( 1 );

  assert( 100 <= hp.m_nEpochs );

  const double learning_rate( hp.m_dblLearningRate ); // 0.001 is good
  const int num_epochs( hp.m_nEpochs ); // 10000 is good
  const int epoch_skip( num_epochs / 100 );

    // Instantiate the model
  pLSTM_t pModel = std::make_shared<LSTM>( input_size, hidden_size, num_layers, output_size );
  pModel->train();
  pModel->to( device );

  // Loss, optimizer, state
  torch::nn::MSELoss criterion( torch::nn::MSELossOptions().reduction( torch::kMean ) ); // loss function
  criterion->to( device );

  torch::optim::Adam optimizer( pModel->parameters(), learning_rate );

  for ( size_t epoch = 0; epoch < num_epochs; ++epoch ) {

    torch::Tensor loss;

    LSTM::lstm_state_t state( pModel->init_states( device, batch_size ) );

    torch::Tensor predictions = pModel->forward( tensorX, state );
    loss = criterion( predictions, tensorY );

    optimizer.zero_grad();
    loss.backward();
    optimizer.step();

    //std::get<0>( state ).detach();
    //std::get<1>( state ).detach();

    if ( 0 == ( ( 1 + epoch ) % epoch_skip ) ) {
      BOOST_LOG_TRIVIAL(info) << "epoch " << ( 1 + epoch ) << '/' << num_epochs << " loss: " << loss.item<float>();
    }

  }

  BOOST_LOG_TRIVIAL(info) << "training done";

  return pModel;

}

