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
 * Project: IndicatorTrading
 * Created: February 8, 2022 00:16
 */

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.h"

namespace {
  static const std::string sOption_Symbol( "symbol" );
  static const std::string sOption_IbClientId( "ib_client_id" );
  static const std::string sOption_BlockSize( "order block size" );
  static const std::string sOption_Threads( "threads" );
  static const std::string sOption_DaysFront( "days_front" );
  static const std::string sOption_DaysBack( "days_back" );
  static const std::string sOption_L2Levels( "l2_levels" );
  static const std::string sOption_PeriodWidth( "period_width" );
  static const std::string sOption_MA1Periods( "ma1_periods" );
  static const std::string sOption_MA2Periods( "ma2_periods" );
  static const std::string sOption_MA3Periods( "ma3_periods" );
  static const std::string sOption_Stochastic1Periods( "stochastic1_periods" );
  static const std::string sOption_Stochastic2Periods( "stochastic2_periods" );
  static const std::string sOption_Stochastic3Periods( "stochastic3_periods" );

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

bool Load( const std::string& sFileName, Options& options ) {

  bool bOk( true );

  try {

    po::options_description config( "AppIndicatorTrading Config" );
    config.add_options()
      ( sOption_Symbol.c_str(), po::value<std::string>( &options.sSymbol ), "symbol" )
      ( sOption_IbClientId.c_str(), po::value<int>( &options.ib_client_id )->default_value( 1 ), "IB Client ID" )
      ( sOption_BlockSize.c_str(), po::value<unsigned int>( &options.nBlockSize )->default_value( 1 ), "Order Block Size" )
      ( sOption_Threads.c_str(), po::value<size_t>( &options.nThreads )->default_value( 1 ), "threads" )

      ( sOption_DaysFront.c_str(), po::value<boost::gregorian::days>(&options.nDaysFront), "minimum front month days in future")
      ( sOption_DaysBack.c_str(), po::value<boost::gregorian::days>(&options.nDaysBack), "minimum back month days in future")

      ( sOption_L2Levels.c_str(), po::value<size_t>( &options.nL2Levels )->default_value( 10 ), "# L2 Levels" )

      ( sOption_PeriodWidth.c_str(), po::value<int>( &options.nPeriodWidth ), "period width (sec)" )

      ( sOption_MA1Periods.c_str(),  po::value<int>( &options.nMA1Periods ), "ma1 (#periods)" )
      ( sOption_MA2Periods.c_str(),  po::value<int>( &options.nMA2Periods ), "ma2 (#periods)" )
      ( sOption_MA3Periods.c_str(),  po::value<int>( &options.nMA3Periods ), "ma3 (#periods)" )

      ( sOption_Stochastic1Periods.c_str(), po::value<int>( &options.nStochastic1Periods ), "stochastic1 (#periods)" )
      ( sOption_Stochastic2Periods.c_str(), po::value<int>( &options.nStochastic2Periods ), "stochastic2 (#periods)" )
      ( sOption_Stochastic3Periods.c_str(), po::value<int>( &options.nStochastic3Periods ), "stochastic3 (#periods)" )
      ;
    po::variables_map vm;

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "AppIndicatorTrading config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk &= parse<std::string>( sFileName, vm, sOption_Symbol, options.sSymbol );
      std::replace_if( options.sSymbol.begin(), options.sSymbol.end(), [](char ch)->bool{return '~' == ch;}, '#' );

      bOk &= parse<int>( sFileName, vm, sOption_IbClientId, options.ib_client_id );
      bOk &= parse<unsigned int>( sFileName, vm, sOption_BlockSize, options.nBlockSize );
      bOk &= parse<size_t>( sFileName, vm, sOption_Threads, options.nThreads );

      bOk &= parse<boost::gregorian::days>( sFileName, vm, sOption_DaysFront, options.nDaysFront );
      bOk &= parse<boost::gregorian::days>( sFileName, vm, sOption_DaysBack, options.nDaysBack );

      bOk &= parse<size_t>( sFileName, vm, sOption_L2Levels, options.nL2Levels );

      bOk &= parse<int>( sFileName, vm, sOption_PeriodWidth, options.nPeriodWidth );

      bOk &= parse<int>( sFileName, vm, sOption_MA1Periods,  options.nMA1Periods );
      bOk &= parse<int>( sFileName, vm, sOption_MA2Periods,  options.nMA2Periods );
      bOk &= parse<int>( sFileName, vm, sOption_MA3Periods,  options.nMA3Periods );

      bOk &= parse<int>( sFileName, vm, sOption_Stochastic1Periods, options.nStochastic1Periods );
      bOk &= parse<int>( sFileName, vm, sOption_Stochastic2Periods, options.nStochastic2Periods );
      bOk &= parse<int>( sFileName, vm, sOption_Stochastic3Periods, options.nStochastic3Periods );

    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " parse error: " << e.what();
    bOk = false;
  }

  return bOk;

}

} // namespace config
