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
 * File:    LSTM.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: June 28, 2025 17:38:07
 */

#pragma once

#include <torch/torch.h>

class LSTM : public torch::nn::Module {
public:

  using lstm_state_t = std::tuple<torch::Tensor, torch::Tensor>; // hidden state, cell state

  LSTM( int input_size, int hidden_size, int num_layers, int output_size );

  lstm_state_t init_states( torch::DeviceType device, int batch_size  );

  torch::Tensor forward( torch::Tensor x, lstm_state_t& state );

private:
  torch::nn::LSTM lstm;
  torch::nn::Linear linear;
};
