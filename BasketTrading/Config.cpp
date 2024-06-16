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

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.hpp"

namespace config {

bool Load( Options& options ) {

  bool bOk( true );

  static const std::string sFilename( "BasketTrading.cfg" );

  try {
    static const std::string sOption_Symbol( "symbol" );
    static const std::string sOption_IbClientId( "ib_client_id" );
    static const std::string sOption_DateHistory( "date_history" );
    static const std::string sOption_DateTrading( "date_trading" );
    static const std::string sOption_DaysFront( "days_front" );
    static const std::string sOption_DaysBack( "days_back" );
    static const std::string sOption_PeriodWidth( "period_width" );
    static const std::string sOption_StochasticPeriods( "stochastic_periods" );
    static const std::string sOption_TelegramToken( "telegram_token" );
    static const std::string sOption_TelegramChatId( "telegram_chat_id" );

    std::string sDateHistory;
    std::string sDateTrading;
    unsigned int nDaysFront;
    unsigned int nDaysBack;

    po::options_description config( "BasketTrading Config" );
    config.add_options()
      ( sOption_Symbol.c_str(), po::value<vSymbol_t>(&options.vSymbol), "underlying symbol" )
      ( sOption_IbClientId.c_str(), po::value<size_t>(&options.ib_client_id), "ib client id" )
      ( sOption_DateHistory.c_str(), po::value<std::string>(&sDateHistory), "history date")
      ( sOption_DateTrading.c_str(), po::value<std::string>(&sDateTrading), "trading date")
      ( sOption_DaysFront.c_str(), po::value<unsigned int>(&nDaysFront), "minimum front month days in future")
      ( sOption_DaysBack.c_str(), po::value<unsigned int>(&nDaysBack), "minimum back month days in future")
      ( sOption_PeriodWidth.c_str(), po::value<size_t>( &options.nPeriodWidth), "period width (sec)" )
      ( sOption_StochasticPeriods.c_str(), po::value<size_t>(&options.nStochasticPeriods), "stochastic (#periods)" )
      ( sOption_TelegramToken.c_str(), po::value<std::string>(&options.sTelegramToken)->default_value( "" ), "telegram token" )
      ( sOption_TelegramChatId.c_str(), po::value<uint64_t>(&options.idTelegramChat)->default_value( 0 ), "telegram chat id" )
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

    if ( 0 < vm.count( sOption_Symbol ) ) {
      options.vSymbol = std::move( vm[sOption_Symbol].as<vSymbol_t>() );
      for ( vSymbol_t::value_type& value: options.vSymbol ) {
        std::replace_if( value.begin(), value.end(), [](char ch)->bool{return '~' == ch;}, '#' );
        BOOST_LOG_TRIVIAL(info) << "symbol " << value;
      }
    }
    else {
      BOOST_LOG_TRIVIAL(error) << sFilename << " missing '" << sOption_Symbol << "='";
      bOk = false;
    }

    if ( 0 < vm.count( sOption_IbClientId ) ) {
      options.ib_client_id = vm[sOption_IbClientId].as<size_t>();
      BOOST_LOG_TRIVIAL(info) << "ib_client_id " << options.ib_client_id;

    }
    else {
      BOOST_LOG_TRIVIAL(error) << sFilename << " missing '" << sOption_IbClientId << "='";
      bOk = false;
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

    if ( 0 < vm.count( sOption_DaysFront ) ) {
      options.nDaysFront = boost::gregorian::days( vm[sOption_DaysFront].as<unsigned int>() );
      BOOST_LOG_TRIVIAL(info) << "front month days " << options.nDaysFront;
    }
    else {
      BOOST_LOG_TRIVIAL(error) << sFilename << " missing '" << sOption_DaysFront << "='";
    }

    if ( 0 < vm.count( sOption_DaysBack ) ) {
      options.nDaysBack = boost::gregorian::days( vm[sOption_DaysBack].as<unsigned int>() );
      BOOST_LOG_TRIVIAL(info) << "back month days " << options.nDaysBack;
    }
    else {
      BOOST_LOG_TRIVIAL(error) << sFilename << " missing '" << sOption_DaysBack << "='";
    }

    if ( 0 < vm.count( sOption_PeriodWidth ) ) {
      options.nPeriodWidth = vm[sOption_PeriodWidth].as<size_t>();
      BOOST_LOG_TRIVIAL(info) << "period width (seconds) " << options.nPeriodWidth;
    }
    else {
      BOOST_LOG_TRIVIAL(error) << sFilename << " missing '" << sOption_PeriodWidth << "='";
      //bOk = false; // uses default otherwise
    }

    if ( 0 < vm.count( sOption_StochasticPeriods ) ) {
      options.nStochasticPeriods = vm[sOption_StochasticPeriods].as<size_t>();
      BOOST_LOG_TRIVIAL(info) << "stochastic (#periods) " << options.nStochasticPeriods;
    }
    else {
      BOOST_LOG_TRIVIAL(error) << sFilename << " missing '" << sOption_StochasticPeriods << "='";
      //bOk = false; // uses default otherwise
    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFilename << " parse error: " << e.what();
    bOk = false;
  }

  return bOk;

}

} // namespace config
