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

#include "BuildSymbolName.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

// something similar in Option.cpp  
// need to deal with x10 type options
// http://www.iqfeed.net/symbolguide/index.cfm?symbolguide=guide&displayaction=support&section=guide&web=iqfeed&guide=options&web=IQFeed&type=stock
const std::string BuildOptionName( const std::string& sUnderlying, boost::uint16_t year, boost::uint16_t month, boost::uint16_t day, double strike, ou::tf::OptionSide::enumOptionSide side ) {
  std::string sName = sUnderlying;
  if ( 0 != year ) {
    sName += boost::lexical_cast<std::string>( year ).substr( 2, 2 );  // last two digits only
    std::string sDay( '0' + boost::lexical_cast<std::string>( day ) );
    sName += sDay.substr( sDay.length() - 2 , 2 ); // two digits
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

const std::string BuildFuturesName( const std::string& sUnderlying, boost::uint16_t year, boost::uint16_t month ) {
  static const char* code = { "FGHJKMNQUVXZ" };
  std::string sName = sUnderlying;
  if ( 0 != year ) {
    sName += code[ month - 1 ];
    sName += boost::lexical_cast<std::string>( year ).substr( 2, 2 );  // last two digits only
  }
  return sName;
}

const std::string BuildFuturesOptionName( const std::string& sUnderlying, boost::uint16_t year, boost::uint16_t month, double strike, ou::tf::OptionSide::enumOptionSide side ) {
  static const char* code = { "FGHJKMNQUVXZ" };
  std::string sName = sUnderlying;
  if ( 0 != year ) {
    sName += code[ month - 1 ];
    sName += boost::lexical_cast<std::string>( year ).substr( 2, 2 );  // last two digits only
    sName += (char) side;
    sName += boost::lexical_cast<std::string>( strike );
  }
  return sName;
}

const std::string BuildFuturesOptionName( const std::string& sUnderlying, boost::uint16_t year, boost::uint16_t month, boost::uint16_t day, double strike, ou::tf::OptionSide::enumOptionSide side ) {
  assert(0);  // day isn't used yet, need to fix
  static const char* code = { "FGHJKMNQUVXZ" };
  std::string sName = sUnderlying;
  if ( 0 != year ) {
    sName += code[ month - 1 ];
    sName += boost::lexical_cast<std::string>( year ).substr( 2, 2 );  // last two digits only
    sName += (char) side;
  }
  sName += boost::lexical_cast<std::string>( strike );
  return sName;
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
