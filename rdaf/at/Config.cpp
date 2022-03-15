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
 * Project: rdaf/at
 * Created: March 7, 2022 14:35
 */

#include <fstream>
#include <exception>

#include <boost/date_time/posix_time/time_parsers.hpp>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.h"

namespace {
  // program option key names
  static const std::string sOption_Symbol( "symbol" );
  static const std::string sOption_TimeBinsCount( "time_bins" );
  static const std::string sOption_TimeBinsUpper( "time_upper" );
  static const std::string sOption_TimeBinsLower( "time_lower" );
  static const std::string sOption_PriceBinsCount( "price_bins" );
  static const std::string sOption_PriceBinsUpper( "price_upper" );
  static const std::string sOption_PriceBinsLower( "price_lower" );
  static const std::string sOption_VolumeBinsCount( "volume_bins" );
  static const std::string sOption_VolumeBinsUpper( "volume_upper" );
  static const std::string sOption_VolumeBinsLower( "volume_lower" );
  static const std::string sOption_GroupDirectory( "group_directory" );
  static const std::string sOption_SimStart( "sim_start" );
  static const std::string sOption_IbInstance( "ib_instance" );

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

    po::options_description config( "AutoTrade Config" );
    config.add_options()
      ( sOption_Symbol.c_str(), po::value<std::string>( &options.sSymbol), "symbol" )

      ( sOption_TimeBinsCount.c_str(), po::value<int>( &options.nTimeBins), "#time bins" )
      ( sOption_TimeBinsUpper.c_str(), po::value<std::string>( &sDateTimeUpper), "time upper yyyymmddThhmmss" )
      ( sOption_TimeBinsLower.c_str(), po::value<std::string>( &sDateTimeLower), "time lower yyyymmddThhmmss" )

      ( sOption_PriceBinsCount.c_str(), po::value<int>( &options.nPriceBins), "#price bins" )
      ( sOption_PriceBinsUpper.c_str(), po::value<double>( &options.dblPriceUpper), "price upper" )
      ( sOption_PriceBinsLower.c_str(), po::value<double>( &options.dblPriceLower), "price lower" )

      ( sOption_VolumeBinsCount.c_str(), po::value<int>( &options.nVolumeBins), "#volume bins" )
      ( sOption_VolumeBinsUpper.c_str(), po::value<double>( &options.dblVolumeUpper), "volume upper" )
      ( sOption_VolumeBinsLower.c_str(), po::value<double>( &options.dblVolumeLower), "volume lower" )

      ( sOption_GroupDirectory.c_str(), po::value<std::string>( &options.sGroupDirectory)->default_value( "" ), "hdf5 group directory" )

      ( sOption_SimStart.c_str(), po::value<bool>( &options.bSimStart)->default_value( false ), "auto start simulation" )
      ( sOption_IbInstance.c_str(), po::value<int>( &options.nIbInstance)->default_value( 1 ), "IB instance" )
      ;

    po::variables_map vm;

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "AutoTrade config file " << sFileName << " does not exist";
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

      bOk |= parse<int>(    sFileName, vm, sOption_VolumeBinsCount, options.nVolumeBins );
      bOk |= parse<double>( sFileName, vm, sOption_VolumeBinsUpper, options.dblVolumeUpper );
      bOk |= parse<double>( sFileName, vm, sOption_VolumeBinsLower, options.dblVolumeLower );

      // TODO: need to make this optional?
      bOk |= parse<std::string>( sFileName, vm, sOption_GroupDirectory, options.sGroupDirectory );

      bOk |= parse<bool>( sFileName, vm, sOption_SimStart, options.bSimStart );
      bOk |= parse<int>( sFileName, vm, sOption_IbInstance, options.nIbInstance );
    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " parse error: " << e.what();
    bOk = false;
  }

  return bOk;

}

} // namespace config
