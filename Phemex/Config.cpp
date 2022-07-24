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
 * Project: Phemex
 * Created: june 4, 2022 05:19
 */

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.hpp"

namespace {
  static const std::string sChoice_PhemexKey(    "phemex_key" );
  static const std::string sChoice_PhemexSecret( "phemex_secret" );
  static const std::string sChoice_PhemexDomain( "phemex_domain" );

  template<typename T>
  bool parse( const std::string& sFileName, po::variables_map& vm, const std::string& name, bool bRequired, T& dest ) {
    bool bOk = true;
    if ( 0 < vm.count( name ) ) {
      dest = std::move( vm[name].as<T>() );
      BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
    }
    else {
      if ( bRequired ) {
        BOOST_LOG_TRIVIAL(error) << sFileName << " missing '" << name << "='";
        bOk = false;
      }
    }
  return bOk;
  }
}

namespace config {

bool Load( const std::string& sFileName, Choices& choices ) {

  bool bOk( true );

  try {

    po::options_description config( "phemex config" );
    config.add_options()
      ( sChoice_PhemexKey.c_str(),    po::value<std::string>( &choices.m_sPhemexKey ),    "phemex key" )
      ( sChoice_PhemexSecret.c_str(), po::value<std::string>( &choices.m_sPhemexSecret ), "phemex secret" )
      ( sChoice_PhemexDomain.c_str(), po::value<std::string>( &choices.m_sPhemexDomain ), "phemex secret" )
      ;
    po::variables_map vm;

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "phemex config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk &= parse<std::string>( sFileName, vm, sChoice_PhemexKey,    true, choices.m_sPhemexKey );
      bOk &= parse<std::string>( sFileName, vm, sChoice_PhemexSecret, true, choices.m_sPhemexSecret );
      bOk &= parse<std::string>( sFileName, vm, sChoice_PhemexDomain, true, choices.m_sPhemexDomain );

    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " parse error: " << e.what();
    bOk = false;
  }

  return bOk;

}

} // namespace config