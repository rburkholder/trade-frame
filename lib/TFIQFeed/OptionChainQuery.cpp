/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include <sstream>

#include "OptionChainQuery.h"

// TODO:  convert OnNewResponse over to IQFeedRetrieval

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

// http://www.iqfeed.net/dev/api/docs/OptionChainsviaTCPIP.cfm

OptionChainQuery::OptionChainQuery() 
: Network<OptionChainQuery>( "127.0.0.1", 9100 )
{
}

OptionChainQuery::~OptionChainQuery(void) {
}

void OptionChainQuery::QueryFutureChain(
    const std::string& sSymbol, 
    const std::string& sMonthCodes,
    const std::string& sYears,
    const std::string& sNearMonths,
    const std::string& sRequestId
    ) {
  std::stringstream ss;
  //boost::this_thread::sleep( boost::posix_time::milliseconds( m_nMillisecondsToSleep ) );
  ss 
    << "CFU," 
    << sSymbol << "," 
    << sMonthCodes << ","
    << sYears << ","
    << sNearMonths << ","
    << sRequestId
    << "\n";
  this->Send( ss.str().c_str() );
}

void OptionChainQuery::QueryFutureOptionChain(
    const std::string& sSymbol, 
    const std::string& sSide,
    const std::string& sMonthCodes,
    const std::string& sYears,
    const std::string& sNearMonths,
    const std::string& sRequestId
    ) {
  std::stringstream ss;
  //boost::this_thread::sleep( boost::posix_time::milliseconds( m_nMillisecondsToSleep ) );
  ss 
    << "CFO," 
    << sSymbol << "," 
    << sSide << ","
    << sMonthCodes << ","
    << sYears << ","
    << sNearMonths << ","
    << sRequestId
    << "\n";
  this->Send( ss.str().c_str() );
}

void OptionChainQuery::QueryEquityOptionChain(
    const std::string& sSymbol, 
    const std::string& sSide,
    const std::string& sMonthCodes,
    const std::string& sNearMonths,
    const std::string& sFilterType,
    const std::string& sFilterOne,
    const std::string& sFilterTwo,
    const std::string& sRequestId
    ) {
  std::stringstream ss;
  //boost::this_thread::sleep( boost::posix_time::milliseconds( m_nMillisecondsToSleep ) );
  ss 
    << "CEO," 
    << sSymbol << "," 
    << sSide << ","
    << sMonthCodes << ","
    << sNearMonths << ","
    << "1" << "," // exclude binary
    << sFilterType << ","
    << sFilterOne << "," 
    << sFilterTwo << "," 
    << sRequestId
    << "\n";
  this->Send( ss.str().c_str() );
}


void OptionChainQuery::AddOptionSymbol( const char *s, unsigned short cnt ) {
  std::string *_s = new std::string( s, cnt );
  m_vOptionSymbols.push_back( _s );
}

void OptionChainQuery::OnNewResponse( const char *szLine ) {
  if ( !true ) {
    if ( 0 == strcmp( szLine, "!ENDMSG!" ) ) {
      //ClosePort();
    }
  }
  else {
    char *szSubStr = (char*) szLine;
    char *ixLine = (char*) szLine;
    unsigned short cnt = 0;

    while ( 0 != *ixLine ) {
      if ( ':' == *ixLine ) {
        if ( 0 != cnt ) {
          AddOptionSymbol( szSubStr, cnt );
          cnt = 0;
        }
        // switch from calls to puts
      }
      else {
        if ( ',' == *ixLine ) {
          if ( 0 != cnt ) {
            AddOptionSymbol( szSubStr, cnt );
            cnt = 0;
          }
        }
        else {
          // add to outstanding string
          if ( 0 == cnt ) {
            szSubStr = ixLine;
          }
          cnt++;
        }
      }
      ixLine++;
    }
    if ( 0 != cnt ) {
      if ( ' ' != *szSubStr ) {
        AddOptionSymbol( szSubStr, cnt );
        cnt = 0;
      }
    }
    if ( NULL != OnSymbolListReceived ) OnSymbolListReceived();
    //m_bLookingForDetail = false;
  }
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
