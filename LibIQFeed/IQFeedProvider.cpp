/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include "IQFeedProvider.h"

CIQFeedProvider::CIQFeedProvider(void)
: CProviderInterface(), CIQFeed()
{
  m_sName = "IQF";
  m_nID = EProviderIQF;
}

CIQFeedProvider::~CIQFeedProvider(void) {
}

void CIQFeedProvider::Connect() {
  if ( !m_bConnected ) {
    CProviderInterface::Connect();
    CIQFeed::Connect();
    m_bConnected = true;
    OnConnected( 0 );
  }
}

void CIQFeedProvider::Disconnect() {
  if ( m_bConnected ) {
    CIQFeed::Disconnect();
    CProviderInterface::Disconnect();
    m_bConnected = false;
    OnDisconnected( 0 );
  }
}

CSymbol *CIQFeedProvider::NewCSymbol(const std::string &sSymbolName) {
  return new CIQFeedSymbol( sSymbolName );
}

void CIQFeedProvider::StartQuoteWatch(CSymbol *pSymbol) {
  StartQuoteTradeWatch( dynamic_cast<CIQFeedSymbol *>( pSymbol ) );
}

void CIQFeedProvider::StopQuoteWatch(CSymbol *pSymbol) {
  StopQuoteTradeWatch( dynamic_cast<CIQFeedSymbol *>( pSymbol ) );
}

void CIQFeedProvider::StartTradeWatch(CSymbol *pSymbol) {
  StartQuoteTradeWatch( dynamic_cast<CIQFeedSymbol *>( pSymbol ) );
}

void CIQFeedProvider::StopTradeWatch(CSymbol *pSymbol) {
  StopQuoteTradeWatch( dynamic_cast<CIQFeedSymbol *>( pSymbol ) );
}

void CIQFeedProvider::HandleQMessage( CIQFUpdateMessage *pMsg ) {
  CProviderInterface::m_mapSymbols_t::iterator m_mapSymbols_Iter;
  m_mapSymbols_Iter = m_mapSymbols.find( pMsg->Field( CIQFUpdateMessage::QPSymbol ) );
  CIQFeedSymbol *pSym;
  if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
    pSym = (CIQFeedSymbol *) m_mapSymbols_Iter -> second;
    pSym ->HandleUpdateMessage( pMsg );
  }
}

void CIQFeedProvider::HandlePMessage( CIQFSummaryMessage *pMsg ) {
  CProviderInterface::m_mapSymbols_t::iterator m_mapSymbols_Iter;
  m_mapSymbols_Iter = m_mapSymbols.find( pMsg->Field( CIQFSummaryMessage::QPSymbol ) );
  CIQFeedSymbol *pSym;
  if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
    pSym = (CIQFeedSymbol *) m_mapSymbols_Iter -> second;
    pSym ->HandleSummaryMessage( pMsg );
  }
}

void CIQFeedProvider::HandleFMessage( CIQFFundamentalMessage *pMsg ) {
  CProviderInterface::m_mapSymbols_t::iterator m_mapSymbols_Iter;
  m_mapSymbols_Iter = m_mapSymbols.find( pMsg->Field( CIQFFundamentalMessage::FSymbol ) );
  CIQFeedSymbol *pSym;
  if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
    pSym = (CIQFeedSymbol *) m_mapSymbols_Iter -> second;
    pSym ->HandleFundamentalMessage( pMsg );
  }
}

void CIQFeedProvider::HandleNMessage( CIQFNewsMessage *pMsg ) {
  CProviderInterface::m_mapSymbols_t::iterator m_mapSymbols_Iter;
  const char *ixFstColon = pMsg->m_sSymbolList.c_str();
  const char *ixLstColon = pMsg->m_sSymbolList.c_str();
  string s;
  __w64 int cnt;

  if ( 0 != *ixLstColon ) {
    do {
      // each symbol has a surrounding set of colons
      if ( ':' == *ixLstColon ) {
        if ( ( ixLstColon - ixFstColon ) > 1 ) {
          // extract symbol
          cnt = ixLstColon - ixFstColon - 1;
          s.assign( ++ixFstColon, cnt );

          m_mapSymbols_Iter = m_mapSymbols.find( s.c_str() );
          CIQFeedSymbol *pSym;
          if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
            pSym = (CIQFeedSymbol *) m_mapSymbols_Iter -> second;
            pSym ->HandleNewsMessage( pMsg );
          }
          ixFstColon = ixLstColon;
        }
        else {
          if ( 1 == ( ixLstColon - ixFstColon ) ) {
            // no symbol, move FstColon
            ixFstColon = ixLstColon;
          }
        }
      }
      ixLstColon++;
    } while ( 0 != *ixLstColon );
  }
}

void CIQFeedProvider::HandleTMessage( CIQFTimeMessage *pMsg ) {
  //map<string, CSymbol*>::iterator m_mapSymbols_Iter;
}

void CIQFeedProvider::HandleSMessage( CIQFSystemMessage *pMsg ) {
  //map<string, CSymbol*>::iterator m_mapSymbols_Iter;
}
