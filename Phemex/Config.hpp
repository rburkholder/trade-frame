/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    Config.hpp
 * Author:  raymond@burkholder.net
 * Project: Phemex
 * Created: june 4, 2022 05:19
  */

#pragma once

#include <vector>
#include <string>

namespace config {

struct Choices {

  std::string m_sPhemexKey;
  std::string m_sPhemexSecret;
  std::string m_sPhemexDomain;
};

bool Load( const std::string& sFileName, Choices& );

} // namespace config
