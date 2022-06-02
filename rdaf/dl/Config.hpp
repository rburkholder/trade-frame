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
 * Project: rdaf/dl
 * Created: May 29, 2022 19:07:15
 */

#pragma once

#include <vector>
#include <string>

namespace config {

using vName_t = std::vector<std::string>; // program options won't work with std::set

struct Choices {

  vName_t m_vExchange;
  vName_t m_vSecurityType;

  double m_dblMinPrice; // based upon 52 wk low
  unsigned int m_nDays;
  unsigned int m_nSimultaneousRetrievals;

};

bool Load( const std::string& sFileName, Choices& );

} // namespace config
