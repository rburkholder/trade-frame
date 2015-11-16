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

// need to deal with x10 type options
// http://www.iqfeed.net/symbolguide/index.cfm?symbolguide=guide&displayaction=support&section=guide&web=iqfeed&guide=options&web=IQFeed&type=stock
const std::string BuildOptionName( const std::string& sUnderlying, boost::uint16_t year, boost::uint8_t month, boost::uint8_t day, ou::tf::OptionSide::enumOptionSide side, double strike ) {
  std::string sName = sUnderlying;
  sName += boost::lexical_cast<std::string>( year ).substr( 2, 2 );  // last two digits only
  sName += ( '0' + boost::lexical_cast<std::string>( day ) ).substr( 2, 2 ); // two digits
  switch ( side ) {
    case 'C': 
      sName += 'A' - 1 + month;
      break;
    case 'P':
      sName =+ 'M' - 1 + month;
      break;
    default:
      assert( 0 );
      break;
  }
  sName +=boost::lexical_cast<std::string>( strike );
  return sName;
}

const std::string BuildFuturesName( const std::string& sUnderlying, boost::uint16_t year, boost::uint8_t month ) {
  static const char* code = { "FGHJKMNQUVXZ" };
  std::string sName = sUnderlying;
  sName += code[ month - 1 ];
  sName += boost::lexical_cast<std::string>( year ).substr( 2, 2 );  // last two digits only
  return sName;
}

const std::string BuildFuturesOptionName( const std::string& sUnderlying, boost::uint16_t year, boost::uint8_t month, ou::tf::OptionSide::enumOptionSide side, double strike ) {
  static const char* code = { "FGHJKMNQUVXZ" };
  std::string sName = sUnderlying;
  sName += code[ month - 1 ];
  sName += boost::lexical_cast<std::string>( year ).substr( 2, 2 );  // last two digits only
  sName += (char) side;
  sName += boost::lexical_cast<std::string>( strike );
  return sName;
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
