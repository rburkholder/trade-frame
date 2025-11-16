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
 * File:    LSTM.cpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: June 28, 2025 17:38:07
 */

 #include "LSTM.hpp"


namespace {
  // note dropout has no affect on 1==num_layers LSTM, if more than 1, common values are 0.3 to 0.5
  //static const double c_dropout( 0.3 );
  static const double c_dropout( 0.0 );
}

LSTM::LSTM( int input_size, int hidden_size, int num_layers, int output_size )
: lstm( torch::nn::LSTMOptions( input_size, hidden_size ).num_layers( num_layers ).batch_first( true ).dropout( c_dropout ) )
, linear( hidden_size, output_size )
{
  register_module( "lstm", lstm );
  register_module( "linear", linear );
  train( true );  // training is true by default, set to false by call to eval()
}

LSTM::lstm_state_t LSTM::init_states( c10::Device device, int batch_size  ) {
  //BOOST_LOG_TRIVIAL(debug) << "LSTM::init_state batch_size: " << batch_size;
  torch::Tensor hidden_state = torch::zeros( { lstm->options.num_layers(), batch_size, lstm->options.hidden_size() } ).to( device );
  torch::Tensor   cell_state = torch::zeros( { lstm->options.num_layers(), batch_size, lstm->options.hidden_size() } ).to( device );
  return std::make_tuple( hidden_state, cell_state );
}

torch::Tensor LSTM::forward( torch::Tensor x, lstm_state_t& state ) {

  torch::Tensor out;

  // Pass the input through the LSTM layer
  std::tie( out, state ) = lstm->forward( x, state );
  //out.to( x.device() );

  // Pass the output of the LSTM layer through the linear layer
  //torch::Tensor prediction = linear->forward( out );
  torch::Tensor prediction = linear->forward( out ).to( x.device() );
  //torch::Tensor prediction = linear->forward( out.reshape( { -1, lstm->options.hidden_size() } ) ).to( x.device() );
  return prediction;
}

// expanded model definition to investigate:
// https://github.com/jinglescode/time-series-forecasting-pytorch/blob/main/step_by_step_code_blocks/define_lstm_model.py
