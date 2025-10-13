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

#include <boost/log/trivial.hpp>

#include "LSTM.hpp"
#include "Model.hpp"
#include "Features.hpp"
#include "HyperParameters.hpp"

namespace {
  static const size_t c_secondsSampleOffset( 23 );  // offset for each sample
  static const size_t c_secondsSequence( 224 ); // duration of sample sequence
  static const size_t c_secondsYOffset( 32 ); // attempt prediction this far in the future

  static const int c_nOutputFeature( 1 );
}

size_t Model::PredictionDistance() const { return c_secondsYOffset; }

Model::Model( const std::string& sDevice, std::int8_t ixDevice )
: m_ixDataScaled {}
, m_fPredictionResult( nullptr )
, m_torchDevice( torch::kCPU )
{

  torch::DeviceType torchDeviceType;

  int num_devices = 0;
  if ( torch::cuda::is_available() ) {
    torchDeviceType = torch::kCUDA;
    num_devices = torch::cuda::device_count();
    BOOST_LOG_TRIVIAL(info) << "number of CUDA devices detected: " << num_devices;
    // when > 1, then can use, as example ' .device( torch::kCUDA, 1 )'
  }
  else {
    torchDeviceType = torch::kCPU;
    BOOST_LOG_TRIVIAL(info) << "no CUDA devices detected, set device to CPU";
  }

  m_torchDevice = torch::Device( sDevice );
  m_torchDevice.set_index( ixDevice );

  torch::manual_seed( 1 );
  torch::cuda::manual_seed_all( 1 );

  static const int input_size( nInputFeature_ );
  static const int hidden_size( nInputFeature_ * 8 );
  static const int num_layers( 1 );
  static const int output_size( c_nOutputFeature );

  m_pLSTM = std::make_unique<LSTM>( input_size, hidden_size, num_layers, output_size );
  m_pLSTM->to( m_torchDevice );
}

Model::~Model() {
  m_pLSTM.reset();
}

namespace {

  // activation functions inspired by AutoTrade/NeuralNet.cpp
  // additional activiation functions:
  // https://machinelearninggeek.com/activation-functions/

  template<int coef>
  inline double binary_sigmoid( double x ) { // 0.0 .. 1.0
    constexpr double k( -coef );
    return 1.0 / ( 1.0 + std::exp( x * k ) );
  }

  template<int coef>
  inline double bipolar_sigmoid( double x ) { // -1.0 .. +1.0 (aka tanh)
    constexpr double k( -coef );
    const double ex = std::exp( x * k );
    return ( 1.0 - ex ) / ( 1.0 + ex );
  }

}

namespace {
  struct maxmin { // not used, kept for reference
    double& max;
    double& min;

    maxmin( double& max_, double& min_, double init )
    : max( max_ ), min( min_ ) {
      max = min = init;
    }

    void add( const double value ) {
      if ( max < value ) max = value;
      else {
        if ( min > value ) min = value;
      }
    }
  };

  inline double ScaleTranslate( const double value ) {
    return ( value * 0.5 ) + 0.5;
  }
}

bool Model::Scale( const Features_raw& raw, Features_scaled& scaled ) {

  bool bScaled( true );

  scaled.price = ScaleTranslate( raw.dblPrice );

  scaled.SDDirection = ScaleTranslate( raw.dblSDDirection );

  scaled.ema029 = ScaleTranslate( raw.dblEma029 );
  scaled.ema013 = ScaleTranslate( raw.dblEma013 );

  const double sigmoidTickJ( bipolar_sigmoid<1>( raw.dblTickJ ) ); // even scaling top and bottom
  scaled.tickJ = ScaleTranslate( sigmoidTickJ );

  const double sigmoidTickL( bipolar_sigmoid<1>( raw.dblTickL ) ); // even scaling top and bottom
  scaled.tickL = ScaleTranslate( sigmoidTickL );

  const double sigmoidReturnsMean( bipolar_sigmoid<1>( raw.dblReturnsMean ) );
  scaled.returns_mean = ScaleTranslate( sigmoidReturnsMean );

  const double signmoidReturnsSlope( bipolar_sigmoid<1>( raw.dblReturnsSlope ) );
  scaled.returns_slope = ScaleTranslate( signmoidReturnsSlope );

  return bScaled;

}

void Model::Append( const Features_raw& raw, Features_scaled& scaled ) {

  if ( Scale( raw, scaled ) ) {

    const fields_t<float> scaled_flt(
      scaled.price.flt
    , scaled.SDDirection.flt
    , scaled.ema029.flt, scaled.ema013.flt
    , scaled.tickJ.flt, scaled.tickL.flt
    , scaled.returns_mean.flt, scaled.returns_slope.flt
    );

    m_vDataScaled.push_back( scaled_flt ); // will need to timestamp each entry
    if ( 1 == m_vDataScaled.size() ) {
      m_ixDataScaled = 0;
    }
  }
}

void Model::EnablePredictionMode() {
  // with torch.no_grad()  ?
  m_vDataScaled.clear();
  m_ixDataScaled = 0;
  m_pLSTM->eval();
}

void Model::SetPredictionResult( fPredictionResult_t&& f ) {
  m_fPredictionResult = std::move( f );
}

float Model::Predict() {

  float price {};

  // TODO: need to test with and without
  torch::NoGradGuard no_grad;

  if ( c_secondsSequence <= m_vDataScaled.size() ) {
    const auto diff( m_vDataScaled.size() - m_ixDataScaled );
    assert( c_secondsSequence == diff );

    torch::Tensor tensorX = // input
      torch::from_blob( (void*)(&m_vDataScaled[ m_ixDataScaled ]), { 1, c_secondsSequence, nInputFeature_ },
      torch::TensorOptions().dtype( torch::kFloat32 )
    ).to( m_torchDevice );

    LSTM::lstm_state_t state( m_pLSTM->init_states( m_torchDevice, 1 ) );
    torch::Tensor prediction = m_pLSTM->forward( tensorX, state );
    //BOOST_LOG_TRIVIAL(info) << "prediction sizes: " << prediction.sizes();
    //assert( prediction.is_contiguous() );
    if ( m_fPredictionResult ) {
      assert( prediction.is_cuda() );

      torch::Tensor cpu_tensor = prediction.to( torch::kCPU );
      assert( prediction.is_cuda() );
      assert( cpu_tensor.is_contiguous() );

      const auto nElements( cpu_tensor.numel() );
      assert( c_secondsSequence == nElements );

      float* pData = cpu_tensor.data_ptr<float>();
      c10::ArrayRef<float> rView( pData, nElements );

      m_fPredictionResult( rView );

      price = rView.back();
    }
    else {
      price = prediction[ 0 ][ c_secondsSequence - 1 ][ 0 ].item<float>();
    }

    ++m_ixDataScaled;
  }

  return price;
}

// reference:
// https://labs.quansight.org/blog/2020/04/pytorch-tensoriterator-internals
// https://labs.quansight.org/blog/2021/04/pytorch-tensoriterator-internals-update

void Model::Train_Init() {

  BOOST_LOG_TRIVIAL(info)
           << "size of fields_t<float>: " << sizeof( fields_t<float> )
    << ',' << "size of fields_t<double>: " << sizeof( fields_t<double> )
    ;

  m_vDataScaled.clear();

  m_vSourceForTensorX.clear();
  m_vSourceForTensorY.clear();

  m_nSamples_actual = 0;
}

void Model::Train_BuildSamples() {

  BOOST_LOG_TRIVIAL(info)
    << "scaled vector size: "
           << m_vDataScaled.size() << "sec"
    << ',' << m_vDataScaled.size() / 60.0 << "min"
    << ',' << m_vDataScaled.size() / 3600.0 << "hr"
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

  static const int nInputFeature( nInputFeature_ );
  static const size_t sizeFloat( sizeof( float ) );

  static const size_t nFieldBytes( nInputFeature * sizeFloat );
  assert( nFieldBytes == sizeof( fields_t<float> ) );

  const size_t nSamples_theory( m_vDataScaled.size() / c_secondsSampleOffset ); // assumes integer math with truncation

  vValuesFlt_t::size_type ixDataScaled {};

  {
    vValuesFlt_t::const_iterator bgnX = m_vDataScaled.begin(); // begin of input
    vValuesFlt_t::const_iterator endX( bgnX + c_secondsSequence ); // end of inpu

    vValuesFlt_t::const_iterator bgnY( bgnX + c_secondsYOffset ); // start of prediction time frame
    vValuesFlt_t::const_iterator endY( bgnY + c_secondsSequence ); // length same as X sequence length

    while ( m_vDataScaled.size() > ( ixDataScaled + secondsTotal ) ) {

      // append X sequence
      std::copy( bgnX, endX, std::back_inserter( m_vSourceForTensorX ) );

      // append Y sequence
      std::for_each(
         bgnY, endY,
         [this]( auto& entry ){
          m_vSourceForTensorY.push_back( entry.fields[ ixTrade ] );
        } );

      bgnX += c_secondsSampleOffset;
      endX += c_secondsSampleOffset;
      bgnY += c_secondsSampleOffset;
      endY += c_secondsSampleOffset;
      ixDataScaled += c_secondsSampleOffset;
      ++m_nSamples_actual;
    }
    // won't match for now:
    BOOST_LOG_TRIVIAL(info) << "nSamples: " << m_nSamples_actual << ',' << nSamples_theory;
  }

  BOOST_LOG_TRIVIAL(info)
    << "data usage: " << ixDataScaled << ',' << m_vDataScaled.size() << ',' << ixDataScaled + secondsTotal;
  BOOST_LOG_TRIVIAL(info)
    << "input samples * (time steps in each sample): "
    << m_nSamples_actual
    << ',' << c_secondsSequence
    << '=' << '(' << m_vSourceForTensorX.size()
    << ','        << m_vSourceForTensorY.size()
           << ')'
    ;

  m_vDataScaled.clear(); // prepare for another set of features

}

void Model::Train_Perform( const HyperParameters& hp ) {

  // use a second layer to reduce the output size?

  // note: from_blob does not manage memory, so underlying needs to be valid during lifetime of tensor

  static const int nInputFeature( nInputFeature_ );

  torch::Tensor tensorX = // input
    torch::from_blob( m_vSourceForTensorX.data(), { m_nSamples_actual, c_secondsSequence, nInputFeature },
    torch::TensorOptions().dtype( torch::kFloat32 )
  ).to( m_torchDevice ); // without .clone(), data source remains in the vector
  BOOST_LOG_TRIVIAL(info) << "batched tensorX sizes: " << tensorX.sizes();

  torch::Tensor tensorY = // output
    torch::from_blob( m_vSourceForTensorY.data(), { m_nSamples_actual, c_secondsSequence, c_nOutputFeature },
    torch::TensorOptions().dtype( torch::kFloat32 )
  ).to( m_torchDevice ); // without .clone(), data source remains in the vector
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
  const int batch_size( m_nSamples_actual );

  assert( 100 <= hp.m_nEpochs );

  const double learning_rate( hp.m_dblLearningRate ); // 0.001 is good
  const int num_epochs( hp.m_nEpochs ); // 10000 is good
  const int epoch_skip( num_epochs / 100 );

  m_pLSTM->train();

  // Loss, optimizer, state
  torch::nn::MSELoss criterion( torch::nn::MSELossOptions().reduction( torch::kMean ) ); // loss function
  criterion->to( m_torchDevice );

  torch::optim::Adam optimizer( m_pLSTM->parameters(), learning_rate );

  for ( size_t epoch = 0; epoch < num_epochs; ++epoch ) {

    torch::Tensor loss;

    LSTM::lstm_state_t state( m_pLSTM->init_states( m_torchDevice, batch_size ) );

    torch::Tensor predictions = m_pLSTM->forward( tensorX, state );
    loss = criterion( predictions, tensorY );

    optimizer.zero_grad();
    loss.backward();
    optimizer.step();

    //std::get<0>( state ).detach();
    //std::get<1>( state ).detach();

    if ( 0 == ( ( 1 + epoch ) % epoch_skip ) ) {
      BOOST_LOG_TRIVIAL(info) << "epoch " << ( 1 + epoch ) << '/' << num_epochs << " loss: " << loss.item<float>();
    }

    static const double target( 0.046 );
    if ( target > loss.item<float>() ) {
      BOOST_LOG_TRIVIAL(info) << "epoch " << ( 1 + epoch ) << '/' << num_epochs << " loss < " << target << ": " << loss.item<float>();
      break;
    }

  }

  BOOST_LOG_TRIVIAL(info) << "training done";

}

void Model::Save( const std::string& sFileName ) {
  BOOST_LOG_TRIVIAL(info) << "save " << c10::str( *m_pLSTM );
  torch::serialize::OutputArchive output_archive;
  m_pLSTM->save( output_archive );
  output_archive.save_to( sFileName );
}

void Model::Load( const std::string& sFileName ) {
  torch::serialize::InputArchive archive;
  archive.load_from( sFileName );
  m_pLSTM->load( archive );
  BOOST_LOG_TRIVIAL(info) << "load " << c10::str( *m_pLSTM );
}
