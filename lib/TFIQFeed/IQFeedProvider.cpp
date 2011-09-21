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

#include <TFTrading/KeyTypes.h>

#include "IQFeedProvider.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

CIQFeedProvider::CIQFeedProvider( void ) 
: CProviderInterface<CIQFeedProvider,CIQFeedSymbol>(), 
  CIQFeed<CIQFeedProvider>()
{
  m_sName = "IQF";
  m_nID = keytypes::EProviderIQF;
  m_bProvidesQuotes = true;
  m_bProvidesTrades = true;
}

CIQFeedProvider::~CIQFeedProvider(void) {
}

void CIQFeedProvider::Connect() {
  if ( !m_bConnected ) {
    OnConnecting( 0 );
    inherited_t::Connect();
    IQFeed_t::Connect();
  }
}

void CIQFeedProvider::OnIQFeedConnected( void ) {
  m_bConnected = true;
  OnConnected( 0 );
}

void CIQFeedProvider::Disconnect() {
  if ( m_bConnected ) {
    CProviderInterfaceBase::OnDisconnecting( 0 );
    IQFeed_t::Disconnect();
    inherited_t::Disconnect();
  }
}

void CIQFeedProvider::OnIQFeedDisConnected( void ) {
  m_bConnected = false;
  OnDisconnected( 0 );
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

void CIQFeedProvider::StartQuoteWatch(pSymbol_t pSymbol) {
  StartQuoteTradeWatch( dynamic_cast<CIQFeedSymbol*>( pSymbol.get() ) );
}

void CIQFeedProvider::StopQuoteWatch(pSymbol_t pSymbol) {
  StopQuoteTradeWatch( dynamic_cast<CIQFeedSymbol*>( pSymbol.get() ) );
}

void CIQFeedProvider::StartTradeWatch(pSymbol_t pSymbol) {
  StartQuoteTradeWatch( dynamic_cast<CIQFeedSymbol*>( pSymbol.get() ) );
}

void CIQFeedProvider::StopTradeWatch(pSymbol_t pSymbol) {
  StopQuoteTradeWatch( dynamic_cast<CIQFeedSymbol*>( pSymbol.get() ) );
}

void CIQFeedProvider::OnIQFeedUpdateMessage( linebuffer_t* pBuffer, CIQFUpdateMessage *pMsg ) {
  inherited_t::m_mapSymbols_t::iterator m_mapSymbols_Iter;
  m_mapSymbols_Iter = m_mapSymbols.find( pMsg->Field( CIQFUpdateMessage::QPSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
    pSym = m_mapSymbols_Iter -> second;
    pSym ->HandleUpdateMessage( pMsg );
  }
  UpdateDone( pBuffer, pMsg );
}

void CIQFeedProvider::OnIQFeedSummaryMessage( linebuffer_t* pBuffer, CIQFSummaryMessage *pMsg ) {
  inherited_t::m_mapSymbols_t::iterator m_mapSymbols_Iter;
  m_mapSymbols_Iter = m_mapSymbols.find( pMsg->Field( CIQFSummaryMessage::QPSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
    pSym = m_mapSymbols_Iter -> second;
    pSym ->HandleSummaryMessage( pMsg );
  }
  SummaryDone( pBuffer, pMsg );
}

void CIQFeedProvider::OnIQFeedFundamentalMessage( linebuffer_t* pBuffer, CIQFFundamentalMessage *pMsg ) {
  inherited_t::m_mapSymbols_t::iterator m_mapSymbols_Iter;
  m_mapSymbols_Iter = m_mapSymbols.find( pMsg->Field( CIQFFundamentalMessage::FSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
    pSym = m_mapSymbols_Iter -> second;
    pSym ->HandleFundamentalMessage( pMsg );
  }
  FundamentalDone( pBuffer, pMsg );
}

void CIQFeedProvider::OnIQFeedNewsMessage( linebuffer_t* pBuffer, CIQFNewsMessage *pMsg ) {

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
  NewsDone( pBuffer, pMsg );
}

void CIQFeedProvider::OnIQFeedTimeMessage( linebuffer_t* pBuffer, CIQFTimeMessage *pMsg ) {
  //map<string, CSymbol*>::iterator m_mapSymbols_Iter;
  TimeDone( pBuffer, pMsg );
}

void CIQFeedProvider::OnIQFeedSystemMessage( linebuffer_t* pBuffer, CIQFSystemMessage *pMsg ) {
  //map<string, CSymbol*>::iterator m_mapSymbols_Iter;
  SystemDone( pBuffer, pMsg );
}

} // namespace tf
} // namespace ou
