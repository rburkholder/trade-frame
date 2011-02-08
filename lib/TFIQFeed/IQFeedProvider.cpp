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

CIQFeedProvider::CIQFeedProvider( void ) 
: CProviderInterface<CIQFeedProvider,CIQFeedSymbol>(), 
  CIQFeed<CIQFeedProvider>()
{
  m_sName = "IQF";
  m_nID = EProviderIQF;
  m_bProvidesQuotes = true;
  m_bProvidesTrades = true;
}

CIQFeedProvider::~CIQFeedProvider(void) {
}

void CIQFeedProvider::Connect() {
  if ( !m_bConnected ) {
    inherited_t::Connect();
    IQFeed_t::Connect();
    m_bConnected = true;
    OnConnected( 0 );
  }
}

void CIQFeedProvider::Disconnect() {
  if ( m_bConnected ) {
    IQFeed_t::Disconnect();
    inherited_t::Disconnect();
    m_bConnected = false;
    OnDisconnected( 0 );
  }
}

CIQFeedProvider::pSymbol_t CIQFeedProvider::NewCSymbol( pInstrument_t pInstrument ) {
  pSymbol_t pSymbol( new CIQFeedSymbol( pInstrument->GetInstrumentName(), pInstrument ) );
  inherited_t::AddCSymbol( pSymbol );
  return pSymbol;
}

void CIQFeedProvider::StartQuoteTradeWatch( CIQFeedSymbol *pSymbol ) {
  if ( !pSymbol->GetQuoteTradeWatchInProgress() ) {
    std::string s = "w" + pSymbol->GetId() + "\n";
    CIQFeed<CIQFeedProvider>::Send( s );
    pSymbol->SetQuoteTradeWatchInProgress();
  }
}

void CIQFeedProvider::StopQuoteTradeWatch( CIQFeedSymbol *pSymbol ) {
  if ( pSymbol->QuoteWatchNeeded() || pSymbol->TradeWatchNeeded() ) {
    // don't do anything, as stuff still active
  }
  else {
    std::string s = "r" + pSymbol->GetId() + "\n";
    CIQFeed<CIQFeedProvider>::Send( s );
  }
}

void CIQFeedProvider::StartQuoteWatch(CIQFeedSymbol* pSymbol) {
  StartQuoteTradeWatch( pSymbol );
}

void CIQFeedProvider::StopQuoteWatch(CIQFeedSymbol* pSymbol) {
  StopQuoteTradeWatch( pSymbol );
}

void CIQFeedProvider::StartTradeWatch(CIQFeedSymbol* pSymbol) {
  StartQuoteTradeWatch( pSymbol );
}

void CIQFeedProvider::StopTradeWatch(CIQFeedSymbol* pSymbol) {
  StopQuoteTradeWatch( pSymbol );
}

void CIQFeedProvider::HandleQMessage( CIQFUpdateMessage *pMsg ) {
  inherited_t::m_mapSymbols_t::iterator m_mapSymbols_Iter;
  m_mapSymbols_Iter = m_mapSymbols.find( pMsg->Field( CIQFUpdateMessage::QPSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
    pSym = m_mapSymbols_Iter -> second;
    pSym ->HandleUpdateMessage( pMsg );
  }
}

void CIQFeedProvider::HandlePMessage( CIQFSummaryMessage *pMsg ) {
  inherited_t::m_mapSymbols_t::iterator m_mapSymbols_Iter;
  m_mapSymbols_Iter = m_mapSymbols.find( pMsg->Field( CIQFSummaryMessage::QPSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
    pSym = m_mapSymbols_Iter -> second;
    pSym ->HandleSummaryMessage( pMsg );
  }
}

void CIQFeedProvider::HandleFMessage( CIQFFundamentalMessage *pMsg ) {
  inherited_t::m_mapSymbols_t::iterator m_mapSymbols_Iter;
  m_mapSymbols_Iter = m_mapSymbols.find( pMsg->Field( CIQFFundamentalMessage::FSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
    pSym = m_mapSymbols_Iter -> second;
    pSym ->HandleFundamentalMessage( pMsg );
  }
}

void CIQFeedProvider::HandleNMessage( CIQFNewsMessage *pMsg ) {

  inherited_t::m_mapSymbols_t::iterator m_mapSymbols_Iter;
/*
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
  */
}

void CIQFeedProvider::HandleTMessage( CIQFTimeMessage *pMsg ) {
  //map<string, CSymbol*>::iterator m_mapSymbols_Iter;
}

void CIQFeedProvider::HandleSMessage( CIQFSystemMessage *pMsg ) {
  //map<string, CSymbol*>::iterator m_mapSymbols_Iter;
}
