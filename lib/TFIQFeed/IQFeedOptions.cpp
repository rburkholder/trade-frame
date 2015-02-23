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

//#include "StdAfx.h"

#include "IQFeedOptions.h"

// TODO:  convert OnNewResponse over to IQFeedRetrieval

namespace ou { // One Unified
namespace tf { // TradeFrame

CIQFeedOptions::CIQFeedOptions(CIQFeedProvider *pProvider, const char *szSymbol) 
: CIQFeedRetrieval( pProvider ) 
{
  OpenPort();
  std::string s;
  s = "OEA,";
  s += szSymbol;
  s += ";";
  m_bLookingForDetail = true;
  m_pPort->SendToSocket( s.c_str() );
}

CIQFeedOptions::~CIQFeedOptions(void) {

  typedef std::string* pString;

  while ( !m_vOptionSymbols.empty() ) {
    pString &s = m_vOptionSymbols.back();
    delete s;
    m_vOptionSymbols.pop_back();
  }
}

void CIQFeedOptions::AddOptionSymbol( const char *s, unsigned short cnt ) {
  std::string *_s = new std::string( s, cnt );
  m_vOptionSymbols.push_back( _s );
}

void CIQFeedOptions::OnNewResponse( const char *szLine ) {
  if ( !m_bLookingForDetail ) {
    if ( 0 == strcmp( szLine, "!ENDMSG!" ) ) {
      ClosePort();
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
    m_bLookingForDetail = false;
  }
}

} // namespace tf
} // namespace ou
