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
 * Project: Dividend, sourced from rdaf/dl
 * Created: May 29, 2022 19:07:15
 * Updated: 2022/08/21 11:47:30
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace config {

using vName_t = std::vector<std::string>; // program options won't work with std::set

struct Choices {

  vName_t m_vListedMarket;
  vName_t m_vSecurityType;
  vName_t m_vIgnoreNames;
  vName_t m_vSecurityState; // symbol,{decline,good,ignore,portfolio}

  double m_dblMinimumYield;
  uint32_t m_nMinimumVolume;

  uint32_t m_nMaxInTransit;

};

bool Load( const std::string& sFileName, Choices& );

} // namespace config
