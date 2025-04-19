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
 * File:    Strategy_impl.hpp
 * Author:  raymond@burkholder.net
 * Project: AutoTrade
 * Created: 2023/07/05 21:18:28
 */

#pragma once

#include <vector>

#include "NeuralNet.hpp"

class Strategy_impl {
public:
  void Queue( const NeuralNet::Input& );
  void Submit( const NeuralNet::Output& );
protected:
private:

  using vInput_t = std::vector<NeuralNet::Input>;
  vInput_t m_vInput; // queue Input until direction established

  NeuralNet m_net;

};