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
 * Project: DepthOfMarket
 * Created: October 24, 2021 14:46
 */

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.h"

namespace {
  static const std::string sFileName( "dom.cfg" );

  static const std::string sOption_SymbolName( "symbol" );
  static const std::string sOption_IbClientId( "ib_client_id" );
  static const std::string sOption_DepthType( "depth" );
  static const std::string sOption_BlockSize( "block_size" );
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
} // namespace anonymous

namespace config {

bool Load( Options& options ) {

  bool bOk( true );

  try {

    po::options_description config( "Application Depth of Market Config" );

    config.add_options()
      ( sOption_SymbolName.c_str(), po::value<std::string>(&options.sSymbolName ), "symbol name")
      ( sOption_IbClientId.c_str(), po::value<int>( &options.ib_client_id )->default_value( 1 ), "IB Client ID" )
      ( sOption_DepthType.c_str(),  po::value<std::string>(&options.sDepthType ), "depth type" )
      ( sOption_BlockSize.c_str(),  po::value<unsigned int>( &options.nBlockSize ), "block size" )

      ( sOption_PeriodWidth.c_str(), po::value<int>( &options.nPeriodWidth ), "period width (sec)" )

      ( sOption_MA1Periods.c_str(),  po::value<int>( &options.nMA1Periods ), "ma1 (#periods)" )
      ( sOption_MA2Periods.c_str(),  po::value<int>( &options.nMA2Periods ), "ma2 (#periods)" )
      ( sOption_MA3Periods.c_str(),  po::value<int>( &options.nMA3Periods ), "ma3 (#periods)" )

      ( sOption_Stochastic1Periods.c_str(), po::value<int>( &options.nStochastic1Periods ), "stochastic1 (#periods)" )
      ( sOption_Stochastic2Periods.c_str(), po::value<int>( &options.nStochastic2Periods ), "stochastic2 (#periods)" )
      ( sOption_Stochastic3Periods.c_str(), po::value<int>( &options.nStochastic3Periods ), "stochastic3 (#periods)" )
      ;

    po::variables_map vm;
    //po::store( po::parse_command_line( argc, argv, config ), vm );

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "depth of market config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk &= parse<std::string>( sFileName, vm, sOption_SymbolName, options.sSymbolName );
      std::replace_if( options.sSymbolName.begin(), options.sSymbolName.end(), [](char ch)->bool{return '~' == ch;}, '#' );

      bOk &= parse<int>( sFileName, vm, sOption_IbClientId, options.ib_client_id );
      bOk &= parse<std::string>( sFileName, vm, sOption_DepthType, options.sDepthType );
      bOk &= parse<unsigned int>( sFileName, vm, sOption_BlockSize, options.nBlockSize );

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
    BOOST_LOG_TRIVIAL(error) << "options parse error: " << e.what();
    bOk = false;
  }

  return bOk;

}

} // namespace config