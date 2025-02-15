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
 * Project: Collector
 * Created: October 20, 2022 20:37:22
  */

#pragma once

#include <set>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace config {

struct Choices {

  using setName_t = std::set<std::string>;
  setName_t m_setSymbolName_L1;     // symbols with level 1 data
  setName_t m_setSymbolName_L2;     // symbols with level 2 data
  setName_t m_setSymbolName_Atm;    // at the money greeks P+C - TODO need days to expiry (default to 2)
  setName_t m_setSymbolName_Greeks; // greeks for options - add symbol to L1 for L1 data

  std::string m_sStopTime;
  boost::posix_time::time_duration m_tdStopTime;

  //std::string m_sHDF5FileName; // to be implemented
};

bool Load( const std::string& sFileName, Choices& );

} // namespace config
