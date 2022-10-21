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
 * File:    Main.cpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: October 20, 2022 17:48:11
 */

#include <string>
#include <sstream>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <OUCommon/TimeSource.h>

#include <TFTrading/Instrument.h>

#include "Config.hpp"

int main( int argc, char* argv[] ) {

  const static std::string sConfigFileName( "collector.cfg" );

  std::string sTSDataStreamStarted;

  config::Choices choices;

  if ( Load( sConfigFileName, choices ) ) {
  }
  else {
    return EXIT_FAILURE;
  }

  {
    std::stringstream ss;
    auto dt = ou::TimeSource::GlobalInstance().External();
    ss
      << ou::tf::Instrument::BuildDate( dt.date() )
      << "-"
      << dt.time_of_day()
      ;
    sTSDataStreamStarted = ss.str();  // will need to make this generic if need some for multiple providers.
  }

  //Process process( choices, vSymbols );
  //process.Wait();

  return EXIT_SUCCESS;
}

