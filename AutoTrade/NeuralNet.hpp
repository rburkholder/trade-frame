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

#pragma once

#include <eigen3/Eigen/Eigen>

class NeuralNet {
public:

  NeuralNet();
  ~NeuralNet();

  struct Input {
    double stochastic;  // range -1.0 .. +1.0 equiv to 0 .. 100
    double tick;  // range -1.0 .. +1.0
    Input(): stochastic {}, tick {} {}
    Input( double stochastic_, double tick_ ): stochastic( stochastic_ ), tick( tick_ ) {}
  };

  struct Output {
    double buy;
    double neutral;
    double sell;
    Output(): buy {}, neutral {}, sell {} {}
    Output( double buy_, double neutral_, double sell_ )
    : buy( buy_ ), neutral( neutral_ ), sell( sell_ ) {}
  };

  void SetInitialState();
  void TrainingStepPattern( const Input&, const Output& );

protected:
private:

  static const std::size_t c_nTimeSteps = 4;
  static const std::size_t c_nStrideSize = 2; // based upon #elements in struct Input
  static const std::size_t c_nInputLayerNodes = c_nTimeSteps * c_nStrideSize;
  static const std::size_t c_nHiddenLayerNodes = c_nInputLayerNodes * 2;
  static const std::size_t c_nOutputLayerNodes = 3;

  using vecInputLayer_t =         Eigen::Matrix<double, 1, c_nInputLayerNodes >;
  // Rows, Columns
  using matHiddenLayerWeights_t = Eigen::Matrix<double, c_nInputLayerNodes, c_nHiddenLayerNodes>;
  using vecHiddenLayer_t =        Eigen::Matrix<double, 1, c_nHiddenLayerNodes>;
  using matOutputLayerWeights_t = Eigen::Matrix<double, c_nHiddenLayerNodes, c_nOutputLayerNodes>;
  using vecOutputLayer_t =        Eigen::Matrix<double, 1, c_nOutputLayerNodes>;

  vecInputLayer_t m_vecInputLayer;

  matHiddenLayerWeights_t m_matHiddenLayerWeights;
  vecHiddenLayer_t m_vecHiddenLayerBiasWeights;
  vecHiddenLayer_t m_vecHiddenLayer;

  matOutputLayerWeights_t m_matOutputLayerWeights;
  vecOutputLayer_t m_vecOutputLayerBiasWeights;
  vecOutputLayer_t m_vecOutputLayer;

  Output m_outputExpected;

  std::size_t m_nTrainingSteps;

  void TrainingStep();

};
