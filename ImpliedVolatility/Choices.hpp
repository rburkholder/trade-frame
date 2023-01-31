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
 * File:    Choices.hpp
 * Author:  raymond@burkholder.net
 * Project: ImpliedVolatility
 * Created: January 30, 2023 18:50:12
 */

#include <string>

#include <boost/date_time/gregorian/gregorian.hpp>

namespace config {

struct Choices {

  std::string sSymbol; // this probably will be the general symbol, not a specific instrument

  boost::gregorian::days nDaysFront;
  boost::gregorian::days nDaysBack;

};

bool Load( const std::string& sFileName, Choices& );

} // namespace config