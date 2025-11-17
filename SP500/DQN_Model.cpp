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
 * File:    DQN_Moodel.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: November 16, 2025 18:48:18
 */

#include "DQN_Model.hpp"
#include "DQN_Module.hpp"

namespace DQN {

Model::Model() {
  Module module( 9 * 10, 128, 4 );
}

Model::~Model() {
}

} // namespace DQN
