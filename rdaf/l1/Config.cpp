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
 * File:    Config.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l1
 * Created: February 6, 2022 14:22
 */

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.h"

namespace config {

bool Load( const std::string& sFileName, Options& options ) {

  bool bOk( true );

  try {

    static const std::string sOption_Symbol( "symbol" );

    std::string sSymbol;

    po::options_description config( "rdaf_l1 Config" );
    config.add_options()
      ( sOption_Symbol.c_str(), po::value<std::string>(&sSymbol), "symbol")
      ;
    po::variables_map vm;
    //po::store( po::parse_command_line( argc, argv, config ), vm );

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "rdaf_l1 config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );
    }

    if ( 0 < vm.count( sOption_Symbol ) ) {
      options.sSymbol = vm[sOption_Symbol].as<std::string>();
      BOOST_LOG_TRIVIAL(info) << "symbol " << options.sSymbol;
    }
    else {
      BOOST_LOG_TRIVIAL(error) << sFileName << " missing '" << sOption_Symbol << "='";
      bOk = false;
    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " parse error: " << e.what();
    bOk = false;
  }

  return bOk;

}

} // namespace config
