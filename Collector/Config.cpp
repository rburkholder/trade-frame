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
 * Project: Collector
 * Created: October 20, 2022 20:37:22
 */

#include <vector>
#include <fstream>
#include <exception>
#include <type_traits>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.hpp"

using vName_t = std::vector<std::string>;

namespace {
  static const std::string sChoice_SymbolName_L1( "symbol_name_l1" );
  static const std::string sChoice_SymbolName_L2( "symbol_name_l2" );
  static const std::string sChoice_SymbolName_Greeks( "symbol_name_greeks" );
  static const std::string sChoice_SymbolName_Atm( "symbol_name_atm" );
  static const std::string sChoice_StopTime(   "stop_time" );

  template<typename T>
  void log( const std::string& name, typename std::enable_if<std::is_pod<T>::value>::type& dest ) {
    BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
  }

  void log( const std::string& name, const std::string& dest ) {
    BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
  }

  void log( const std::string& name, const vName_t& dest ) {
    for ( const vName_t::value_type& value: dest ) {
      BOOST_LOG_TRIVIAL(info) << name << " = " << value;
    }
  }
  //void log( const std::string& name, const config::Choices::setName_t& dest ) {
  //  for ( const std::string& value: dest ) {
  //    BOOST_LOG_TRIVIAL(info) << name << " = " << value;
  //  }
  //}

  template<typename T>
  bool parse( const std::string& sFileName, po::variables_map& vm, const std::string& name, bool bRequired, T& dest ) {
    bool bOk = true;
    if ( 0 < vm.count( name ) ) {
      dest = std::move( vm[name].as<T>() );
      log( name, dest );
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
  vName_t vSymbolName_L1;   // symbols with level 1 data
  vName_t vSymbolName_L2;   // symbols with level 2 data
  vName_t vSymbolName_Atm;  // at the money greeks P+C - TODO need days to expiry (default to 2)
  vName_t vSymbolName_Greeks;

  try {

    po::options_description config( "collector config" );
    config.add_options()
      ( sChoice_SymbolName_L1.c_str(), po::value<vName_t>( &vSymbolName_L1 ), "symbol name for level 1 data" )
      ( sChoice_SymbolName_L2.c_str(), po::value<vName_t>( &vSymbolName_L2 ), "symbol name for level 1 data" )
      ( sChoice_SymbolName_Atm.c_str(), po::value<vName_t>( &vSymbolName_Atm ), "underlying symbol name for atm IV etc" )
      ( sChoice_SymbolName_Greeks.c_str(), po::value<vName_t>( &vSymbolName_Greeks ), "iqfeed option name from mktsymbols file" )
      ( sChoice_StopTime.c_str(),   po::value<std::string>( &choices.m_sStopTime ), "stop time HH:mm:ss UTC" )
      ;
    po::variables_map vm;

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "collector config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk &= parse<vName_t>( sFileName, vm, sChoice_SymbolName_L1, true, vSymbolName_L1 );
      if ( bOk ) {
        for ( vName_t::value_type& sn: vSymbolName_L1 ) {
          std::replace_if( sn.begin(), sn.end(), [](char ch)->bool{return '~' == ch;}, '#' );
          choices.m_setSymbolName_L1.emplace( sn );
        }
      }

      bOk &= parse<vName_t>( sFileName, vm, sChoice_SymbolName_L2, false, vSymbolName_L2 );
      if ( bOk ) {
        for ( vName_t::value_type& sn: vSymbolName_L2 ) {
          std::replace_if( sn.begin(), sn.end(), [](char ch)->bool{return '~' == ch;}, '#' );
          choices.m_setSymbolName_L2.emplace( sn );
        }
      }

      bOk &= parse<vName_t>( sFileName, vm, sChoice_SymbolName_Atm, false, vSymbolName_Atm );
      if ( bOk ) {
        for ( vName_t::value_type& sn: vSymbolName_Atm ) {
          std::replace_if( sn.begin(), sn.end(), [](char ch)->bool{return '~' == ch;}, '#' );
          choices.m_setSymbolName_Atm.emplace( sn );
        }
      }

      bOk &= parse<vName_t>( sFileName, vm, sChoice_SymbolName_Greeks, false, vSymbolName_Greeks );
      if ( bOk ) {
        for ( vName_t::value_type& sn: vSymbolName_Greeks ) {
          choices.m_setSymbolName_Greeks.emplace( sn );
        }
      }

      bOk &= parse<std::string>( sFileName, vm, sChoice_StopTime, true, choices.m_sStopTime );
      if ( bOk ) {
        choices.m_tdStopTime = boost::posix_time::duration_from_string( choices.m_sStopTime );
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