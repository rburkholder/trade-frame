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
 * Project: Dividend, sourced from rdaf/dl
 * Created: May 29, 2022 19:07:15
 * Updated: 2022/08/21 11:47:30
*/

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.hpp"

namespace {
  static const std::string sChoice_ListedMarket( "listed_market" );
  static const std::string sChoice_SecurityType( "security_type" );
  static const std::string sChoice_IgnoreName( "ignore_name" );
  static const std::string sChoice_SecurityState( "security_state" );
  static const std::string sChoice_MinimumYield( "minimum_yield" );
  static const std::string sChoice_MinimumVolume( "minimum_volume" );
  static const std::string sChoice_MaxInTransit( "max_in_transit" );

  template<typename T>
  bool parse( const std::string& sFileName, po::variables_map& vm, const std::string& name, bool bOptional, T& dest ) {
    bool bOk = true;
    if ( 0 < vm.count( name ) ) {
      dest = std::move( vm[name].as<T>() );
      if constexpr( std::is_same<T, config::vName_t>::value ) {
        for ( const auto& item: dest ) {
          BOOST_LOG_TRIVIAL(info) << name << " = " << item;
        }
      }
      else {
        BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
      }    }
    else {
      if ( !bOptional ) {
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

    po::options_description config( "dividend config" );
    config.add_options()
      ( sChoice_ListedMarket.c_str(), po::value<vName_t>( &choices.m_vListedMarket ), "listed market" )
      ( sChoice_SecurityType.c_str(), po::value<vName_t>( &choices.m_vSecurityType ), "security type" )
      ( sChoice_IgnoreName.c_str(), po::value<vName_t>( &choices.m_vIgnoreNames ) , "ignore names" )
      ( sChoice_SecurityState.c_str(), po::value<vName_t>( &choices.m_vSecurityState ) , "state - symbol,{decline,good,ignore,portfolio}" )
      ( sChoice_MinimumYield.c_str(), po::value<double>( &choices.m_dblMinimumYield ), "minimum yield" )
      ( sChoice_MinimumVolume.c_str(), po::value<uint32_t>( &choices.m_nMinimumVolume ), "minimum volume" )
      ( sChoice_MaxInTransit.c_str(), po::value<uint32_t>( &choices.m_nMaxInTransit ), "maximum in transit" )
      ;
    po::variables_map vm;

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "dividend config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk &= parse<vName_t>( sFileName, vm, sChoice_ListedMarket, false, choices.m_vListedMarket );
      bOk &= parse<vName_t>( sFileName, vm, sChoice_SecurityType, false, choices.m_vSecurityType );
      bOk &= parse<vName_t>( sFileName, vm, sChoice_IgnoreName, true, choices.m_vIgnoreNames );
      bOk &= parse<vName_t>( sFileName, vm, sChoice_SecurityState, true, choices.m_vSecurityState );
      bOk &= parse<double>( sFileName, vm, sChoice_MinimumYield, false, choices.m_dblMinimumYield );
      bOk &= parse<uint32_t>( sFileName, vm, sChoice_MinimumVolume, false, choices.m_nMinimumVolume );
      bOk &= parse<uint32_t>( sFileName, vm, sChoice_MaxInTransit, false, choices.m_nMaxInTransit );

    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " parse error: " << e.what();
    bOk = false;
  }

  return bOk;

}

} // namespace config