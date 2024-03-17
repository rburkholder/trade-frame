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
 * Project: CurrencyTrader
 * Created: March 09, 2024 19:58:27
 */

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.hpp"

namespace {
  static const std::string sChoice_SymbolName(      "symbol_name" );
  static const std::string sChoice_sExchange(       "exchange" );
  static const std::string sOption_IbInstance(      "ib_instance" );
  static const std::string sChoice_StartTime(       "start_time" );
  static const std::string sChoice_StopTime(        "stop_time" );
  static const std::string sChoice_MaxLifeTime(     "max_life_time" );
  static const std::string sChoice_PipProfit(       "pip_profit" );
  static const std::string sChoice_PipStopLoss(     "pip_stop_loss" );
  static const std::string sChoice_PipTrailingStop( "pip_trailing_stop" );
  static const std::string sChoice_LotSize(         "lot_size" );

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

    po::options_description config( "currency trader config" );
    config.add_options()
      ( sChoice_SymbolName.c_str(), po::value<std::string>( &choices.m_sSymbolName ), "symbol name" )
      ( sChoice_sExchange.c_str(), po::value<std::string>( &choices.m_sExchange ), "exchange name" )

      ( sOption_IbInstance.c_str(), po::value<int>( &choices.m_nIbInstance)->default_value( 1 ), "IB instance" )

      ( sChoice_StopTime.c_str(),   po::value<std::string>( &choices.m_sStopTime ), "stop time HH:mm:ss UTC" )
      ( sChoice_StartTime.c_str(),   po::value<std::string>( &choices.m_sStartTime ), "start time HH:mm:ss UTC" )
      ( sChoice_MaxLifeTime.c_str(),   po::value<std::string>( &choices.m_sMaxTradeLifeTime ), "max life time HH:mm:ss" )

      ( sChoice_PipProfit.c_str(), po::value<unsigned int>( &choices.m_nPipProfit ), "pip profit taking" )
      ( sChoice_PipStopLoss.c_str(), po::value<unsigned int>( &choices.m_nPipStopLoss ), "pip stop loss" )
      ( sChoice_PipTrailingStop.c_str(), po::value<unsigned int>( &choices.m_nPipTrailingStop )->default_value( 0 ), "pip trailing stop" )

      ( sChoice_LotSize.c_str(), po::value<unsigned int>( &choices.m_nLotSize )->default_value( 1 ), "lot size" )
      ;
    po::variables_map vm;

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "currency trader config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk &= parse<std::string>( sFileName, vm, sChoice_SymbolName, true, choices.m_sSymbolName );
      std::replace_if( choices.m_sSymbolName.begin(), choices.m_sSymbolName.end(), [](char ch)->bool{return '~' == ch;}, '#' );

      bOk &= parse<std::string>( sFileName, vm, sChoice_sExchange, true, choices.m_sExchange );

      bOk &= parse<std::string>( sFileName, vm, sChoice_StopTime, true, choices.m_sStopTime );
      choices.m_tdStopTime = boost::posix_time::duration_from_string( choices.m_sStopTime );

      bOk &= parse<std::string>( sFileName, vm, sChoice_StartTime, true, choices.m_sStartTime );
      choices.m_tdStartTime = boost::posix_time::duration_from_string( choices.m_sStartTime );

      bOk &= parse<std::string>( sFileName, vm, sChoice_MaxLifeTime, true, choices.m_sMaxTradeLifeTime );
      choices.m_tdMaxTradeLifeTime = boost::posix_time::duration_from_string( choices.m_sMaxTradeLifeTime );

      bOk &= parse<unsigned int>( sFileName, vm, sChoice_PipProfit, true, choices.m_nPipProfit );
      bOk &= parse<unsigned int>( sFileName, vm, sChoice_PipStopLoss, true, choices.m_nPipStopLoss );
      bOk &= parse<unsigned int>( sFileName, vm, sChoice_PipTrailingStop, true, choices.m_nPipTrailingStop );
      bOk &= parse<unsigned int>( sFileName, vm, sChoice_LotSize, true, choices.m_nLotSize );
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