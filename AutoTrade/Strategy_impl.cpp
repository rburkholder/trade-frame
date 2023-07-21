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
 * File:    Strategy_impl.cpp
 * Author:  raymond@burkholder.net
 * Project: AutoTrade
 * Created: 2023/07/05 21:18:28
 */

#include "Strategy_impl.hpp"

void Strategy_impl::Queue( const NeuralNet::Input& input ) {
  m_vInput.emplace_back( input );
}

void Strategy_impl::Submit( const NeuralNet::Output& output ) {
  for ( const vInput_t::value_type& input: m_vInput ) {
    m_net.TrainingStepPattern( input, output );
  }
  m_vInput.clear();
}