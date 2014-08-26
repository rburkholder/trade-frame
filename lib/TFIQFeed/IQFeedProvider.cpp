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

#include "StdAfx.h"

#include <boost/lexical_cast.hpp>

#include <TFTrading/KeyTypes.h>

#include "IQFeedProvider.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

IQFeedProvider::IQFeedProvider( void ) 
: ProviderInterface<IQFeedProvider,IQFeedSymbol>(), 
  IQFeed<IQFeedProvider>()
{
  m_sName = "IQF";
  m_nID = keytypes::EProviderIQF;
  m_bProvidesQuotes = true;
  m_bProvidesTrades = true;
}

IQFeedProvider::~IQFeedProvider(void) {
}

void IQFeedProvider::Connect() {
  if ( !m_bConnected ) {
    OnConnecting( 0 );
    inherited_t::Connect();
    IQFeed_t::Connect();
  }
}

void IQFeedProvider::OnIQFeedConnected( void ) {
  m_bConnected = true;
  OnConnected( 0 );
}

void IQFeedProvider::Disconnect() {
  if ( m_bConnected ) {
    ProviderInterfaceBase::OnDisconnecting( 0 );
    IQFeed_t::Disconnect();
    inherited_t::Disconnect();
  }
}

void IQFeedProvider::OnIQFeedDisConnected( void ) {
  m_bConnected = false;
  OnDisconnected( 0 );
}

void IQFeedProvider::OnIQFeedError( size_t e ) {
  OnError( e );
}

IQFeedProvider::pSymbol_t IQFeedProvider::NewCSymbol( pInstrument_t pInstrument ) {
  pSymbol_t pSymbol( new IQFeedSymbol( pInstrument->GetInstrumentName( ID() ), pInstrument ) );
  inherited_t::AddCSymbol( pSymbol );
  return pSymbol;
}

void IQFeedProvider::StartQuoteTradeWatch( IQFeedSymbol* pSymbol ) {
  if ( !pSymbol->GetQuoteTradeWatchInProgress() ) {
    std::string s = "w" + pSymbol->GetId() + "\n";
    IQFeed<IQFeedProvider>::Send( s );
    pSymbol->SetQuoteTradeWatchInProgress();
  }
}

void IQFeedProvider::StopQuoteTradeWatch( IQFeedSymbol* pSymbol ) {
  if ( pSymbol->QuoteWatchNeeded() || pSymbol->TradeWatchNeeded() ) {
    // don't do anything, as stuff still active
  }
  else {
    std::string s = "r" + pSymbol->GetId() + "\n";
    IQFeed<IQFeedProvider>::Send( s );
  }
}

void IQFeedProvider::StartQuoteWatch(pSymbol_t pSymbol) {
  StartQuoteTradeWatch( dynamic_cast<IQFeedSymbol*>( pSymbol.get() ) );
}

void IQFeedProvider::StopQuoteWatch(pSymbol_t pSymbol) {
  StopQuoteTradeWatch( dynamic_cast<IQFeedSymbol*>( pSymbol.get() ) );
}

void IQFeedProvider::StartTradeWatch(pSymbol_t pSymbol) {
  StartQuoteTradeWatch( dynamic_cast<IQFeedSymbol*>( pSymbol.get() ) );
}

void IQFeedProvider::StopTradeWatch(pSymbol_t pSymbol) {
  StopQuoteTradeWatch( dynamic_cast<IQFeedSymbol*>( pSymbol.get() ) );
}

void IQFeedProvider::OnIQFeedUpdateMessage( linebuffer_t* pBuffer, IQFUpdateMessage *pMsg ) {
  inherited_t::m_mapSymbols_t::iterator m_mapSymbols_Iter;
  m_mapSymbols_Iter = m_mapSymbols.find( pMsg->Field( IQFUpdateMessage::QPSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
    pSym = m_mapSymbols_Iter -> second;
    pSym ->HandleUpdateMessage( pMsg );
  }
  UpdateDone( pBuffer, pMsg );
}

void IQFeedProvider::OnIQFeedSummaryMessage( linebuffer_t* pBuffer, IQFSummaryMessage *pMsg ) {
  inherited_t::m_mapSymbols_t::iterator m_mapSymbols_Iter;
  m_mapSymbols_Iter = m_mapSymbols.find( pMsg->Field( IQFSummaryMessage::QPSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
    pSym = m_mapSymbols_Iter -> second;
    pSym ->HandleSummaryMessage( pMsg );
  }
  SummaryDone( pBuffer, pMsg );
}

void IQFeedProvider::OnIQFeedFundamentalMessage( linebuffer_t* pBuffer, IQFFundamentalMessage *pMsg ) {
  inherited_t::m_mapSymbols_t::iterator m_mapSymbols_Iter;
  m_mapSymbols_Iter = m_mapSymbols.find( pMsg->Field( IQFFundamentalMessage::FSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
    pSym = m_mapSymbols_Iter -> second;
    pSym ->HandleFundamentalMessage( pMsg );
  }
  FundamentalDone( pBuffer, pMsg );
}

void IQFeedProvider::OnIQFeedNewsMessage( linebuffer_t* pBuffer, IQFNewsMessage *pMsg ) {

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
          IQFeedSymbol *pSym;
          if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
            pSym = (IQFeedSymbol *) m_mapSymbols_Iter -> second;
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

void IQFeedProvider::OnIQFeedTimeMessage( linebuffer_t* pBuffer, IQFTimeMessage *pMsg ) {
  //map<string, CSymbol*>::iterator m_mapSymbols_Iter;
  TimeDone( pBuffer, pMsg );
}

void IQFeedProvider::OnIQFeedSystemMessage( linebuffer_t* pBuffer, IQFSystemMessage *pMsg ) {
  //map<string, CSymbol*>::iterator m_mapSymbols_Iter;
  SystemDone( pBuffer, pMsg );
}

//http://www.iqfeed.net/symbolguide/index.cfm?symbolguide=guide&displayaction=support&section=guide&web=iqfeed&guide=options&web=IQFeed&type=stock
void IQFeedProvider::SetAlternateInstrumentName( pInstrument_t pInstrument ) {
  // need to check if it already set or not
  std::string sName;
  switch ( pInstrument->GetInstrumentType() ) {
  case ou::tf::InstrumentType::Option:
    sName += pInstrument->GetUnderlyingName();
    std::string d = pInstrument->GetCommonCalcExpiryAsIsoString();
    sName += d.substr( 2, 2 );
    int month = boost::lexical_cast<int>( d.substr( 4, 2 ) );
    sName += d.substr( 6, 2 );
    switch ( pInstrument->GetOptionSide() ) {
    case ou::tf::OptionSide::Call:
      sName += 'A' + month - 1;
      break;
    case ou::tf::OptionSide::Put:
      sName += 'M' + month - 1;
      break;
    }
    sName += boost::lexical_cast<std::string>( pInstrument->GetStrike() );
    break;
  }
  pInstrument->SetAlternateName( ID(), sName );
}

} // namespace tf
} // namespace ou
