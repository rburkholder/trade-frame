/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    Choices.cpp
 * Author:  raymond@burkholder.net
 * Project: ImpliedVolatility
 * Created: January 30, 2023 18:50:12
 */

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Choices.hpp"

namespace {
  static const std::string sOption_Symbol( "symbol" );
  static const std::string sOption_DaysFront( "days_front" );
  static const std::string sOption_DaysBack( "days_back" );

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

bool Load( const std::string& sFileName, Choices& choices ) {

  bool bOk( true );

  try {

    po::options_description config( "AppImpliedVolatility Config" );
    config.add_options()
      ( sOption_Symbol.c_str(), po::value<std::string>( &choices.sSymbol ), "symbol" )

      ( sOption_DaysFront.c_str(), po::value<boost::gregorian::days>(&choices.nDaysFront), "minimum front month days in future")
      ( sOption_DaysBack.c_str(), po::value<boost::gregorian::days>(&choices.nDaysBack), "minimum back month days in future")

      ;
    po::variables_map vm;

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "AppImpliedVolatility config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk &= parse<std::string>( sFileName, vm, sOption_Symbol, choices.sSymbol );
      std::replace_if( choices.sSymbol.begin(), choices.sSymbol.end(), [](char ch)->bool{return '~' == ch;}, '#' );

      bOk &= parse<boost::gregorian::days>( sFileName, vm, sOption_DaysFront, choices.nDaysFront );
      bOk &= parse<boost::gregorian::days>( sFileName, vm, sOption_DaysBack, choices.nDaysBack );

    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " parse error: " << e.what();
    bOk = false;
  }

  return bOk;

}

} // namespace config
