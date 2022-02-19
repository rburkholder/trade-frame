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
 * Project: rdaf/l1
 * Created: February 6, 2022 14:22
 */


#include <fstream>
#include <exception>

#include <boost/date_time/posix_time/time_parsers.hpp>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.h"

namespace {
  static const std::string sOption_Symbol( "symbol" );
  static const std::string sOption_TimeBinsCount( "time_bins" );
  static const std::string sOption_TimeBinsUpper( "time_upper" );
  static const std::string sOption_TimeBinsLower( "time_lower" );
  static const std::string sOption_PriceBinsCount( "price_bins" );
  static const std::string sOption_PriceBinsUpper( "price_upper" );
  static const std::string sOption_PriceBinsLower( "price_lower" );
  static const std::string sOption_VolDltaBinsCount( "vol_side_bins" );
  static const std::string sOption_VolDltaBinsUpper( "vol_side_upper" );
  static const std::string sOption_VolDltaBinsLower( "vol_side_lower" );
  static const std::string sOption_VolTtlBinsCount( "vol_ttl_bins" );
  static const std::string sOption_VolTtlBinsUpper( "vol_ttl_upper" );
  static const std::string sOption_VolTtlBinsLower( "vol_ttl_lower" );

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

  std::string sDateTimeUpper;
  std::string sDateTimeLower;

  try {

    po::options_description config( "rdaf_l1 Config" );
    config.add_options()
      ( sOption_Symbol.c_str(), po::value<std::string>( &options.sSymbol), "symbol" )

      ( sOption_TimeBinsCount.c_str(), po::value<int>( &options.nTimeBins), "#time bins" )
      ( sOption_TimeBinsUpper.c_str(), po::value<std::string>( &sDateTimeUpper), "time upper yyyymmddThhmmss" )
      ( sOption_TimeBinsLower.c_str(), po::value<std::string>( &sDateTimeLower), "time lower yyyymmddThhmmss" )

      ( sOption_PriceBinsCount.c_str(), po::value<int>( &options.nPriceBins), "#price bins" )
      ( sOption_PriceBinsUpper.c_str(), po::value<double>( &options.dblPriceUpper), "price upper" )
      ( sOption_PriceBinsLower.c_str(), po::value<double>( &options.dblPriceLower), "price lower" )

      ( sOption_VolDltaBinsCount.c_str(), po::value<int>( &options.nVolumeSideBins), "#volume side bins" )
      ( sOption_VolDltaBinsUpper.c_str(), po::value<double>( &options.dblVolumeSideUpper), "volume side upper" )
      ( sOption_VolDltaBinsLower.c_str(), po::value<double>( &options.dblVolumeSideLower), "volume side lower" )

      ( sOption_VolTtlBinsCount.c_str(), po::value<int>( &options.nVolumeTotalBins), "#volume total bins" )
      ( sOption_VolTtlBinsUpper.c_str(), po::value<double>( &options.dblVolumeTotalUpper), "volume total upper" )
      ( sOption_VolTtlBinsLower.c_str(), po::value<double>( &options.dblVolumeTotalLower), "volume total lower" )
      ;
    po::variables_map vm;

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "rdaf_l1 config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk |= parse<std::string>( sFileName, vm, sOption_Symbol, options.sSymbol );
      std::replace_if( options.sSymbol.begin(), options.sSymbol.end(), [](char ch)->bool{return '~' == ch;}, '#' );

      bOk |= parse<int>(    sFileName, vm, sOption_TimeBinsCount, options.nTimeBins );
      bOk |= parse<std::string>( sFileName, vm, sOption_TimeBinsUpper, sDateTimeUpper );
      options.dtTimeUpper = boost::posix_time::from_iso_string( sDateTimeUpper );
      bOk |= parse<std::string>( sFileName, vm, sOption_TimeBinsLower, sDateTimeLower );
      options.dtTimeLower = boost::posix_time::from_iso_string( sDateTimeLower );

      bOk |= parse<int>(    sFileName, vm, sOption_PriceBinsCount, options.nPriceBins );
      bOk |= parse<double>( sFileName, vm, sOption_PriceBinsUpper, options.dblPriceUpper );
      bOk |= parse<double>( sFileName, vm, sOption_PriceBinsLower, options.dblPriceLower );

      bOk |= parse<int>(    sFileName, vm, sOption_VolDltaBinsCount, options.nVolumeSideBins );
      bOk |= parse<double>( sFileName, vm, sOption_VolDltaBinsUpper, options.dblVolumeSideUpper );
      bOk |= parse<double>( sFileName, vm, sOption_VolDltaBinsLower, options.dblVolumeSideLower );

      bOk |= parse<int>(    sFileName, vm, sOption_VolTtlBinsCount, options.nVolumeTotalBins );
      bOk |= parse<double>( sFileName, vm, sOption_VolTtlBinsUpper, options.dblVolumeTotalUpper );
      bOk |= parse<double>( sFileName, vm, sOption_VolTtlBinsLower, options.dblVolumeTotalLower );
    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " parse error: " << e.what();
    bOk = false;
  }

  return bOk;

}

} // namespace config
