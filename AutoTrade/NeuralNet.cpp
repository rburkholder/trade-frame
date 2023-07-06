/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    NeuralNet.cpp
 * Author:  raymond@burkholder.net
 * Project: AutoTrade
 * Created: 2023/07/03 16:49:40
 */

#include "NeuralNet.hpp"

namespace {
  double c_LearningRate( 0.1 );
}

NeuralNet::NeuralNet()
: m_nTrainingSteps {}
{
  SetInitialState();
}

NeuralNet::~NeuralNet() {}

void NeuralNet::SetInitialState() {
  // 0.5 to cut -1.0 .. +1.0 down to -0.5 .. +0.5
  m_matHiddenLayerWeights     = 0.5 * matHiddenLayerWeights_t::Random();
  m_vecHiddenLayerBiasWeights = 0.5 * vecHiddenLayer_t::Random();
  m_matOutputLayerWeights     = 0.5 * matOutputLayerWeights_t::Random();
  m_vecOutputLayerBiasWeights = 0.5 * vecOutputLayer_t::Random();
}

void NeuralNet::TrainingStepPattern( const Input& input, const Output& expected ) {

  m_outputExpected = expected;

  std::size_t nCountDown = c_nTimeSteps - 1;
  vecInputLayer_t::iterator iter;
  for ( iter = m_vecInputLayer.begin(); iter != m_vecInputLayer.end(); iter++ ) {
    for ( std::size_t nStride = c_nStrideSize; 0 != nStride; nStride-- ) {
      *iter = *(iter + c_nStrideSize );
      iter++;
    }
    nCountDown--;
    if ( 0 == nCountDown ) break;
  }

  *iter++ = input.stochastic;
  *iter++ = input.tick;

  m_nTrainingSteps++;

  if ( c_nTimeSteps <= m_nTrainingSteps ) {
    TrainingStep();
  }
}

namespace {

  // this book covered the implementation details, as well as the partial deriviatives of the sigmoid functions
  // 0133341860 1994 Fundamentals of Neural Networks: Architectures, Algorithms, and Applications.pdf
  // page 291 - 296

  inline double binary_sigmoid( double x ) { return 1.0 / ( 1.0 + std::exp( -x ) ); } // 0.0 .. 1.0
  inline double binary_sigmoid_pd1( double x ) {
    const double sig = binary_sigmoid( x );
    return sig * ( 1.0 - sig );
  }
  inline double binary_sigmoid_pd2( double x ) { // use this on node with activation output
    return x * ( 1.0 - x );
  }

  inline double bipolar_sigmoid( double x ) { // -1.0 .. +1.0
    const double ex = std::exp( -x );
    return ( 2.0 / ( 1.0 + ex ) ) - 1.0;
  }
  inline double bipolar_sigmoid_pd1( double x ) {
    const double sig = bipolar_sigmoid( x );
    return 0.5 * ( 1.0 + sig ) * ( 1.0 - sig );
  }
  inline double bipolar_sigmoid_pd2( double x ) { // use this on node with activation output
    return 0.5 * ( 1.0 + x ) * ( 1.0 - x );
  }

  inline double tanh( double x ) {
    const double ex_p = std::exp(  x );
    const double ex_m = std::exp( -x );
    return ( ex_p - ex_m ) / ( ex_p + ex_m );
  }
  inline double relu( double x ) { // rectified linear unit
    return ( 0.0 < x ) ? x : 0.0;
  }
  inline double step( double x ) {
    return ( 0.0 > x ) ? 0.0 : 1.0;
  }
}

void NeuralNet::TrainingStep() {

  // Feed Forward

  vecHiddenLayer_t vecHiddenLayerIn = m_vecHiddenLayerBiasWeights + ( m_vecInputLayer * m_matHiddenLayerWeights );
  m_vecHiddenLayer = vecHiddenLayerIn;
  for ( vecHiddenLayer_t::value_type& hidden: m_vecHiddenLayer ) {
    hidden = bipolar_sigmoid( hidden );
  }

  m_vecOutputLayer = m_vecOutputLayerBiasWeights + ( m_vecHiddenLayer * m_matOutputLayerWeights );
  for ( vecOutputLayer_t::value_type& output: m_vecOutputLayer ) {
    output = binary_sigmoid( output );
  }

  // Expectation

  const double t1 = m_outputExpected.buy;      const double z1 = m_vecOutputLayer[ 0 ];
  const double t2 = m_outputExpected.neutral;  const double z2 = m_vecOutputLayer[ 1 ];
  const double t3 = m_outputExpected.sell;     const double z3 = m_vecOutputLayer[ 2 ];

  // TODO: to improve the cost function, replace mse with actual trading p/l mse
  // TODO: after each epoch, output error results

  double mse {}; // mean squared error
  double diff = t1 - z1;
  mse += diff * diff;
         diff = t2 - z2;
  mse += diff * diff;
         diff = t3 - z3;
  mse += diff * diff;

  mse = mse / c_nOutputLayerNodes;

  // Backward Propogation

  const vecOutputLayer_t vecOutputLayerDelta = {
    ( t1 - z1 ) * binary_sigmoid_pd2( z1 ),
    ( t2 - z2 ) * binary_sigmoid_pd2( z2 ),
    ( t3 - z3 ) * binary_sigmoid_pd2( z3 )
  };

  const Eigen::Matrix<double, c_nOutputLayerNodes, c_nHiddenLayerNodes> matOutputWeightCorrection
    = c_LearningRate * vecOutputLayerDelta.transpose() * m_vecHiddenLayer;

  const vecOutputLayer_t vecOutputBiasCorrection = c_LearningRate * vecOutputLayerDelta;

  vecHiddenLayer_t vecHiddenLayerDelta = vecOutputLayerDelta * matOutputWeightCorrection;

  // https://iamfaisalkhan.com/matrix-manipulations-using-eigen-cplusplus/
  // Array class may help with this
  vecHiddenLayer_t::iterator iterDelta = vecHiddenLayerDelta.begin();
  vecHiddenLayer_t::const_iterator iterHiddenIn = vecHiddenLayerIn.begin();
  while ( vecHiddenLayerDelta.end() != iterDelta ) {
    *iterDelta *= bipolar_sigmoid_pd2( *iterHiddenIn );
    iterDelta++;
    iterHiddenIn++;
  }

  matHiddenLayerWeights_t matHiddenLayerCorrection
    = c_LearningRate * m_vecInputLayer.transpose() * vecHiddenLayerDelta;

  const vecHiddenLayer_t vecHiddenLayerBiasWeightsCorrection = c_LearningRate * vecHiddenLayerDelta;

  // Update weights and biases

  m_matHiddenLayerWeights += matHiddenLayerCorrection;
  m_vecHiddenLayerBiasWeights += vecHiddenLayerBiasWeightsCorrection;

  m_matOutputLayerWeights += matOutputWeightCorrection.transpose();
  m_vecOutputLayerBiasWeights += vecOutputBiasCorrection;

}

// Neural Networks Math by Michael Taylor: useful for understanding the chained partial derivative implications

// timeseries LSTM with pytorch:
// 9781800561618 2022 Kunal Sawarkar, Dheeraj Arremsetty - Deep Learning with PyTorch Lightning_ Build and train high-performance artificial intelligence and self-supervised models using Python-Packt.pdf