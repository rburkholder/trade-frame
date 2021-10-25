/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    Config.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created: October 24, 2021 21:25
 */

#include <boost-1_69/boost/date_time/gregorian/greg_date.hpp>
#include <string>
#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.h"

namespace config {

bool Load( Options& options ) {

  bool bOk( true );

  static const std::string sFilename( "BasketTrading.cfg" );

  try {

    static const std::string sOption_DateHistory( "date_history" );
    static const std::string sOption_DateTrading( "date_trading" );

    std::string sDateHistory;
    std::string sDateTrading;

    po::options_description config( "BasketTrading Config" );
    config.add_options()
      ( sOption_DateHistory.c_str(), po::value<std::string>(&sDateHistory), "history date")
      ( sOption_DateTrading.c_str(), po::value<std::string>(&sDateTrading), "trading date")
      ;
    po::variables_map vm;
    //po::store( po::parse_command_line( argc, argv, config ), vm );

    std::ifstream ifs( sFilename.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "basket trading config file " << sFilename << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );
    }

    if ( 0 < vm.count( sOption_DateHistory ) ) {
      options.dateHistory = boost::gregorian::from_string( vm[sOption_DateHistory].as<std::string>() );
      BOOST_LOG_TRIVIAL(info) << "date history " << options.dateHistory;
    }
    else {
      BOOST_LOG_TRIVIAL(error) << sFilename << " missing '" << sOption_DateHistory << "='";
      bOk = false;
    }

    if ( 0 < vm.count( sOption_DateTrading ) ) {
      options.dateTrading = boost::gregorian::from_string( vm[sOption_DateTrading].as<std::string>() );
      BOOST_LOG_TRIVIAL(info) << "date trading " << options.dateTrading;
    }
    else {
      BOOST_LOG_TRIVIAL(error) << sFilename << " missing '" << sOption_DateTrading << "='";
      bOk = false;
    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFilename << " parse error: " << e.what();
    bOk = false;
  }

  return bOk;

}

} // namespace config