/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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

// started 2015/11/15

#include <boost/lexical_cast.hpp>

#include <TFTrading/TradingEnumerations.h>

#include "BuildSymbolName.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

// month is 1 - 12
// day is 1 - 31
const std::string BuildName( const NameParts& parts ) {
  std::string sBuiltName;
  switch ( parts.it ) {
    case ou::tf::InstrumentType::Stock:
      sBuiltName = parts.sRootName;
      break;
    case ou::tf::InstrumentType::Option:
      if ( 0 != parts.day ) std::runtime_error( "ou::tf::iqfeed::BuildName: 0 == parts.day" );
      sBuiltName
        = ou::tf::iqfeed::BuildOptionName( parts.sRootName, parts.year, parts.month, parts.day, parts.strike, parts.side );
      break;
    case ou::tf::InstrumentType::Future:
      sBuiltName
        = ou::tf::iqfeed::BuildFuturesName( parts.sRootName, parts.year, parts.month );
      break;
    case ou::tf::InstrumentType::FuturesOption:
      sBuiltName
        = ou::tf::iqfeed::BuildFuturesOptionName( parts.sRootName, parts.year, parts.month, parts.strike, parts.side );
      break;
    default:
      throw std::runtime_error( "ou::tf::iqfeed::BuildName: unknown instrument type" );
      break;
  }
  return sBuiltName;
}

// something similar in Option.cpp
// need to deal with x10 type options
// http://www.iqfeed.net/symbolguide/index.cfm?symbolguide=guide&displayaction=support&section=guide&web=iqfeed&guide=options&web=IQFeed&type=stock
const std::string BuildOptionName( const std::string& sUnderlying, uint16_t year, uint16_t month, uint16_t day, double strike, ou::tf::OptionSide::EOptionSide side ) {
  std::string sName = sUnderlying;
  if ( 0 != year ) {
    sName += boost::lexical_cast<std::string>( year ).substr( 2, 2 );  // last two digits only
    sName += ( ( 9 < day ) ? "" : "0" ) + boost::lexical_cast<std::string>( day );
    //std::string sDay( '0' + boost::lexical_cast<std::string>( day ) );
    //sName += sDay.substr( sDay.length() - 2 , 2 ); // two digits
    switch ( side ) {
      case 'C':
        sName += 'A' - 1 + month;
        break;
      case 'P':
        sName += 'M' - 1 + month;
        break;
      default:
        assert( 0 );
        break;
    }
    sName += boost::lexical_cast<std::string>( strike );
  }
  return sName;
}

const std::string BuildFuturesName( const std::string& sUnderlying, uint16_t year, uint16_t month ) {
  static const char* code = { "FGHJKMNQUVXZ" };
  std::string sName = sUnderlying;
  if ( 0 != year ) {
    sName += code[ month - 1 ];
    sName += boost::lexical_cast<std::string>( year ).substr( 2, 2 );  // last two digits only
  }
  return sName;
}

const std::string BuildFuturesOptionName( const std::string& sUnderlying, uint16_t year, uint16_t month, double strike, ou::tf::OptionSide::EOptionSide side ) {
  assert( 1 <= month );
  assert( 12 >= month );
  static const char* code = { "-FGHJKMNQUVXZ" }; // hyphen to make as 1 index
  std::string sName = sUnderlying;
  if ( 0 != year ) {
    sName += ( 12 == month ? code[ 1 ] : code[ month + 1 ] ); // need code of next month
    sName += boost::lexical_cast<std::string>( year ).substr( 2, 2 );  // last two digits only
    sName += (char) side;
    sName += boost::lexical_cast<std::string>( strike );
  }
  return sName;
}

const std::string BuildFuturesOptionName( const std::string& sUnderlying, uint16_t year, uint16_t month, uint16_t day, double strike, ou::tf::OptionSide::EOptionSide side ) {
  std::runtime_error( "ou::tf::iqfeed::BuildFuturesOptionName: day not used yet" ); // day isn't used yet, need to fix
  assert( 1 <= month );
  assert( 12 >= month );
  static const char* code = { "-FGHJKMNQUVXZ" }; // hyphen to make as 1 index
  std::string sName = sUnderlying;
  if ( 0 != year ) {
    sName += ( 12 == month ? code[ 1 ] : code[ month + 1 ] ); // need code of next month
    sName += boost::lexical_cast<std::string>( year ).substr( 2, 2 );  // last two digits only
    sName += (char) side;
  }
  sName += boost::lexical_cast<std::string>( strike );
  return sName;
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
