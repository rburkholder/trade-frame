/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * Project: MarketTrader
 * Created: 2024/12/26 22:02:51
 */

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.hpp"

namespace {
  static const std::string c_sDescription( "market_trader config file" );

  static const std::string sValue_Var_Directory_Config( "var_directory_config" );
  static const std::string sValue_Var_Directory_Lib(    "var_directory_lib" );
  static const std::string sValue_Var_Directory_Log(    "var_directory_log" );
  static const std::string sValue_Var_Directory_Script( "var_directory_script" );

  static const std::string sValue_Telegram_Token(  "telegram_token" );
  static const std::string sValue_Telegram_ChatId( "telegram_chat_id" );

  template<typename T>
  bool parse( const std::string& sFileName, po::variables_map& vm, const std::string& name, T& dest ) {
    bool bOk = true;
    if ( 0 < vm.count( name ) ) {
      dest = vm[name].as<T>();
      BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
    }
    else {
      BOOST_LOG_TRIVIAL(error) << sFileName << " missing '" << name << "='";
      bOk = false;
    }
  return bOk;
  }
}

namespace config {

bool Load( const std::string& sFileName, Values& values ) {

  bool bOk( true );

  try {

    po::options_description config( c_sDescription );
    config.add_options()

      ( sValue_Var_Directory_Config.c_str(), po::value<std::string>( &values.sDirConfig )->default_value( "./config" ), "var directory config" )
      ( sValue_Var_Directory_Lib.c_str(),    po::value<std::string>( &values.sDirLib    )->default_value( "./lib" ),    "var directory lib" )
      ( sValue_Var_Directory_Log.c_str(),    po::value<std::string>( &values.sDirLog    )->default_value( "./log" ),    "var directory log" )
      ( sValue_Var_Directory_Script.c_str(), po::value<std::string>( &values.sDirScript )->default_value( "./script" ), "var directory script" )

      ( sValue_Telegram_Token.c_str(),  po::value<std::string>( &values.telegram.sToken )->default_value( "" ), "telegram token" )
      ( sValue_Telegram_ChatId.c_str(), po::value<uint64_t>(    &values.telegram.idChat )->default_value( 0 ),  "telegram chat id" )
      ;
    po::variables_map vm;
    //po::store( po::parse_command_line( argc, argv, config ), vm );

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << c_sDescription << " '" << sFileName << "' does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk &= parse<std::string>( sFileName, vm, sValue_Var_Directory_Config, values.sDirConfig );
      bOk &= parse<std::string>( sFileName, vm, sValue_Var_Directory_Lib,    values.sDirLib );
      bOk &= parse<std::string>( sFileName, vm, sValue_Var_Directory_Log,    values.sDirLog );
      bOk &= parse<std::string>( sFileName, vm, sValue_Var_Directory_Script, values.sDirScript );

      bOk &= parse<std::string>( sFileName, vm, sValue_Telegram_Token, values.telegram.sToken );
      bOk &= parse<uint64_t>( sFileName, vm, sValue_Telegram_ChatId,   values.telegram.idChat );
    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " parse error: " << e.what();
    bOk = false;
  }

  return bOk;
}

} // namespace config
