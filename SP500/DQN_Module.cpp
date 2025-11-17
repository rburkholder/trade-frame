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
 * File:    DQN_Module.cpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: November 16, 2025 17:51:41
 */

#include "DQN_Module.hpp"

namespace DQN {

Module::Module( int input_size, int hidden_size, int output_size )
: relu()
, conv1d1( input_size, hidden_size, 3 )
, conv1d2( hidden_size, hidden_size, 3 )
, flatten1()
, fc1( hidden_size, hidden_size )
, fc2( hidden_size, output_size )
{
  register_module( "relu", relu );
  register_module( "conv1d", conv1d1 );
  register_module( "conv1d", conv1d2 );
  register_module( "flatten", flatten1 ); // may not need this
  register_module( "linear", fc1 );
  register_module( "linear", fc2 );
}

Module::~Module() {}

} // namespace DQN
