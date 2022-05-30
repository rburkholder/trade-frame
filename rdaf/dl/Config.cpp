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
 * Project: rdaf/dl
 * Created: May 29, 2022 19:07:15
 */

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.hpp"

namespace {
  static const std::string sChoice_Exchange( "exchange" );
  static const std::string sChoice_SecurityType( "security_type" );
  static const std::string sChoice_MinimumPrice( "minimum_price" );

  template<typename T>
  bool parse( const std::string& sFileName, po::variables_map& vm, const std::string& name, T& dest ) {
    bool bOk = true;
    if ( 0 < vm.count( name ) ) {
      dest = std::move( vm[name].as<T>() );
      //BOOST_LOG_TRIVIAL(info) << name << " = " << dest; // can't log a vector?
    }
    else {
      BOOST_LOG_TRIVIAL(error) << sFileName << " missing '" << name << "='";
      bOk = false;
    }
  return bOk;
  }
}

namespace config {

bool Load( const std::string& sFileName, Choices& choices ) {

  bool bOk( true );

  try {

    po::options_description config( "rdaf/dl config" );
    config.add_options()
      ( sChoice_Exchange.c_str(), po::value<vName_t>( &choices.m_vExchange ), "exchange" )
      ( sChoice_SecurityType.c_str(), po::value<vName_t>( &choices.m_vSecurityType ), "security type" )
      ( sChoice_MinimumPrice.c_str(), po::value<double>( &choices.m_dblMinPrice ), "minimum price" )
      ;
    po::variables_map vm;

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "rdaf/dl config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk &= parse<vName_t>( sFileName, vm, sChoice_Exchange, choices.m_vExchange );
      bOk &= parse<vName_t>( sFileName, vm, sChoice_SecurityType, choices.m_vSecurityType );
      bOk &= parse<double>( sFileName, vm, sChoice_MinimumPrice, choices.m_dblMinPrice );

    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " parse error: " << e.what();
    bOk = false;
  }

  return bOk;

}

} // namespace config