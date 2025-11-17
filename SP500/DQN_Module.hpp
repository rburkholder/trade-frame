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
 * File:    DQN_Module.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: November 16, 2025 17:51:41
 */

#pragma once

//#include <torch/torch.h>

#include <torch/nn/modules/activation.h>
#include <torch/nn/modules/conv.h>
#include <torch/nn/modules/linear.h>
#include <torch/nn/modules/container/sequential.h>

namespace DQN {

class Module: public torch::nn::Module {
public:
  Module( int input_size, int hidden_size, int output_size );
  virtual ~Module();
protected:
private:

  torch::nn::ReLU relu;

  torch::nn::Conv1d conv1d1;
  torch::nn::Conv1d conv1d2;
  torch::nn::Flatten flatten1;
  torch::nn::Linear fc1;
  torch::nn::Linear fc2;
};

} // namespace DQN

