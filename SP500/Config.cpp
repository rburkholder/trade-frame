/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * Project: SP500
 * Created: March 30, 2025 17:59:23
 */

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.hpp"

namespace {
  static const std::string sChoice_SimStart(  "run_sim" );
  static const std::string sChoice_sHdf5File( "hdf5_file" );

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

    po::options_description config( "SP500 config" );
    config.add_options()

    ( sChoice_SimStart.c_str(), po::value<bool>( &choices.m_bRunSim )->default_value( true ), "run simulation" )
    ( sChoice_sHdf5File.c_str(), po::value<std::string>( &choices.m_sHdf5File )->default_value( "TradeFrame.hdf5" ), "hdf5 file" )
    ;
    po::variables_map vm;

    std::ifstream ifs( sFileName.c_str() );
    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "SP500 config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk &= parse<bool>( sFileName, vm, sChoice_SimStart, false, choices.m_bRunSim );
      bOk &= parse<std::string>( sFileName, vm, sChoice_sHdf5File, false, choices.m_sHdf5File );
    }

    if ( choices.m_bRunSim ) {
      if ( 0 < choices.m_sHdf5File.size() ) {}
      else {
        bOk = false;
        BOOST_LOG_TRIVIAL(error) << sFileName << ' ' << sChoice_sHdf5File << " required";
      }
    }

  }
  catch( const std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " config parse error: " << e.what();
    bOk = false;
  }
  catch(...) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " config unknown error";
    bOk = false;
  }



  return bOk;

}

} // namespace config