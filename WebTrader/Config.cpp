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
 * Project: WebTrader
 * Created: 2025/04/05 21:25:03
 */

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.hpp"

namespace {
  static const std::string sChoice_IbClientId( "ib_client_id" );
  static const std::string sChoice_IbClientPort( "ib_client_port" );
  static const std::string sChoice_UIUserName( "ui_username" );
  static const std::string sChoice_UIPassWord( "ui_password" );
  static const std::string sChoice_CandidateFuture( "candidate_future" );

  template<typename T>
  bool parse( const std::string& sFileName, po::variables_map& vm, const std::string& name, bool bRequired, T& dest ) {
    bool bOk = true;
    if ( 0 < vm.count( name ) ) {
      dest = std::move( vm[name].as<T>() );
      if constexpr( std::is_same<T, config::Choices::vCandidateFutures_t>::value ) {
        for ( const auto& item: dest ) {
          BOOST_LOG_TRIVIAL(info) << name << " = " << item;
        }
      }
      else {
        BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
      }

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

    po::options_description config( "config" );
    config.add_options()
      ( sChoice_IbClientId.c_str(), po::value<int>( &choices.ib_client_id )->default_value( 1 ), "ib client id (1)" )
      ( sChoice_IbClientPort.c_str(), po::value<unsigned int>( &choices.ib_client_port )->default_value( 7496 ), "ib client port (7496)" )
      ( sChoice_UIUserName.c_str(), po::value<std::string>( &choices.m_sUIUserName ), "ui username" )
      ( sChoice_UIPassWord.c_str(), po::value<std::string>( &choices.m_sUIPassWord ), "ui password" )
      ( sChoice_CandidateFuture.c_str(), po::value<Choices::vCandidateFutures_t>( &choices.m_vCandidateFutures ), "candidate future" )
      ;
    po::variables_map vm;

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk &= parse<int>( sFileName, vm, sChoice_IbClientId, false, choices.ib_client_id );
      bOk &= parse<unsigned int>( sFileName, vm, sChoice_IbClientPort, false, choices.ib_client_port );

      bOk &= parse<std::string>( sFileName, vm, sChoice_UIUserName, true, choices.m_sUIUserName );
      bOk &= ( 0 < choices.m_sUIUserName.size() );

      bOk &= parse<std::string>( sFileName, vm, sChoice_UIPassWord, true, choices.m_sUIPassWord );
      bOk &= ( 0 < choices.m_sUIPassWord.size() );

      bOk &= parse<Choices::vCandidateFutures_t>( sFileName, vm, sChoice_CandidateFuture, true, choices.m_vCandidateFutures );
      bOk &= ( 0 < choices.m_vCandidateFutures.size() );
    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " parse error: " << e.what();
    bOk = false;
  }

  return bOk;

}

} // namespace config

