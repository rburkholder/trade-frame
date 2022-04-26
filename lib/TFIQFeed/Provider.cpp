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

#include <boost/lexical_cast.hpp>

#include <TFTrading/KeyTypes.h>

#include "Provider.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

IQFeedProvider::IQFeedProvider()
: ProviderInterface<IQFeedProvider,IQFeedSymbol>()
, IQFeed<IQFeedProvider>()
{
  m_sName = "IQF";
  m_nID = keytypes::EProviderIQF;
  m_bProvidesQuotes = true;
  m_bProvidesTrades = true;

}

IQFeedProvider::~IQFeedProvider() {
}

void IQFeedProvider::Connect() {
  if ( !m_bConnected ) {


    ProviderInterfaceBase::OnConnecting( 0 );
    inherited_t::Connect();
    IQFeed_t::Connect();
  }
}

void IQFeedProvider::OnIQFeedConnected() {
  m_bConnected = true;
  inherited_t::ConnectionComplete();
  ProviderInterfaceBase::OnConnected( 0 );
}

void IQFeedProvider::Disconnect() {
  if ( m_bConnected ) {
    ProviderInterfaceBase::OnDisconnecting( 0 ); // watches are regsitered here
    inherited_t::Disconnecting();  // provider then cleans up
    IQFeed_t::Disconnect();
    inherited_t::Disconnect();
  }
}

void IQFeedProvider::OnIQFeedDisConnected() {
  m_bConnected = false;
  ProviderInterfaceBase::OnDisconnected( 0 );
}

void IQFeedProvider::OnIQFeedError( size_t e ) {
  OnError( e );
}

IQFeedProvider::pSymbol_t IQFeedProvider::NewCSymbol( pInstrument_t pInstrument ) {
  pSymbol_t pSymbol( new IQFeedSymbol( pInstrument->GetInstrumentName( ID() ), pInstrument ) );
  inherited_t::AddCSymbol( pSymbol );
  return pSymbol;
}

namespace {
  static const char transition[4][4] = {
    /* from             to    None Quote Trade Both*/
    /* WatchState::None  */ { '-', 'w',  't',  'w' },
    /* WatchState::Quote */ { 'r', '-',  't',  '-' },
    /* WatchState::Trade */ { 'r', 'w',  '-',  'w' },
    /* WatchState::Both  */ { 'r', '-',  't',  '-' }
  };  // - = no change, w = watch, t = trades only, r = reset
      // reverse diagonal is illegal as it includes two simultaneous watch changes
}

void IQFeedProvider::UpdateQuoteTradeWatch( char command, IQFeedSymbol::WatchState next, IQFeedSymbol* pSymbol ) {
  if ( '-' != command ) {
    std::string s = command + pSymbol->GetId() + "\n";
    //std::cout << command + pSymbol->GetId() << std::endl;
    IQFeed<IQFeedProvider>::Send( s );
  }
  pSymbol->SetWatchState( next );
}

void IQFeedProvider::StartQuoteWatch( pSymbol_t pSymbol ) {
  IQFeedSymbol::WatchState current = pSymbol->GetWatchState();
  IQFeedSymbol::WatchState next = IQFeedSymbol::WatchState::None;
  switch ( current ) {
    case IQFeedSymbol::WatchState::None:
      next = IQFeedSymbol::WatchState::WSQuote;
      UpdateQuoteTradeWatch( transition[current][next], next, dynamic_cast<IQFeedSymbol*>( pSymbol.get() ) );
      break;
    case IQFeedSymbol::WatchState::WSQuote:
      // nothing to do
      break;
    case IQFeedSymbol::WatchState::WSTrade:
      next = IQFeedSymbol::WatchState::Both;
      UpdateQuoteTradeWatch( transition[current][next], next, dynamic_cast<IQFeedSymbol*>( pSymbol.get() ) );
      break;
    case IQFeedSymbol::WatchState::Both:
      // nothing to do
      break;
  }
}

void IQFeedProvider::StopQuoteWatch(pSymbol_t pSymbol) {
  IQFeedSymbol::WatchState current = pSymbol->GetWatchState();
  IQFeedSymbol::WatchState next = IQFeedSymbol::WatchState::None;
  switch ( current ) {
    case IQFeedSymbol::WatchState::None:
      std::cout << "IQFeedProvider::StopQuoteWatch error with None: " << pSymbol->GetId() << std::endl;
      break;
    case IQFeedSymbol::WatchState::WSQuote:
      next = IQFeedSymbol::WatchState::None;
      UpdateQuoteTradeWatch( transition[current][next], next, dynamic_cast<IQFeedSymbol*>( pSymbol.get() ) );
      break;
    case IQFeedSymbol::WatchState::WSTrade:
      std::cout << "IQFeedProvider::StopQuoteWatch error with Trade: " << pSymbol->GetId() << std::endl;
      break;
    case IQFeedSymbol::WatchState::Both:
      next = IQFeedSymbol::WatchState::WSTrade;
      UpdateQuoteTradeWatch( transition[current][next], next, dynamic_cast<IQFeedSymbol*>( pSymbol.get() ) );
      break;
  }
}

void IQFeedProvider::StartTradeWatch(pSymbol_t pSymbol) {
  IQFeedSymbol::WatchState current = pSymbol->GetWatchState();
  IQFeedSymbol::WatchState next = IQFeedSymbol::WatchState::None;
  switch ( current ) {
    case IQFeedSymbol::WatchState::None:
      next = IQFeedSymbol::WatchState::WSTrade;
      UpdateQuoteTradeWatch( transition[current][next], next, dynamic_cast<IQFeedSymbol*>( pSymbol.get() ) );
      break;
    case IQFeedSymbol::WatchState::WSQuote:
      next = IQFeedSymbol::WatchState::Both;
      UpdateQuoteTradeWatch( transition[current][next], next, dynamic_cast<IQFeedSymbol*>( pSymbol.get() ) );
      break;
    case IQFeedSymbol::WatchState::WSTrade:
      // nothing to do
      break;
    case IQFeedSymbol::WatchState::Both:
      // nothing to do
      break;
  }
}

void IQFeedProvider::StopTradeWatch(pSymbol_t pSymbol) {
  IQFeedSymbol::WatchState current = pSymbol->GetWatchState();
  IQFeedSymbol::WatchState next = IQFeedSymbol::WatchState::None;
  switch ( current ) {
    case IQFeedSymbol::WatchState::None:
      std::cout << "IQFeedProvider::StopTradeWatch error with None: " << pSymbol->GetId() << std::endl;
      break;
    case IQFeedSymbol::WatchState::WSQuote:
      std::cout << "IQFeedProvider::StopTradeWatch error with Quote: " << pSymbol->GetId() << std::endl;
      break;
    case IQFeedSymbol::WatchState::WSTrade:
      next = IQFeedSymbol::WatchState::None;
      UpdateQuoteTradeWatch( transition[current][next], next, dynamic_cast<IQFeedSymbol*>( pSymbol.get() ) );
      break;
    case IQFeedSymbol::WatchState::Both:
      next = IQFeedSymbol::WatchState::WSQuote;
      UpdateQuoteTradeWatch( transition[current][next], next, dynamic_cast<IQFeedSymbol*>( pSymbol.get() ) );
      break;
  }
}

void IQFeedProvider::OnIQFeedDynamicFeedUpdateMessage( linebuffer_t* pBuffer, IQFDynamicFeedUpdateMessage *pMsg ) {
  inherited_t::mapSymbols_t::iterator mapSymbols_iter;
  mapSymbols_iter = m_mapSymbols.find( pMsg->Field( IQFDynamicFeedUpdateMessage::DFSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != mapSymbols_iter ) {
    pSym = mapSymbols_iter -> second;
    pSym ->HandleDynamicFeedUpdateMessage( pMsg );
  }
  this->DynamicFeedUpdateDone( pBuffer, pMsg );
}

void IQFeedProvider::OnIQFeedDynamicFeedSummaryMessage( linebuffer_t* pBuffer, IQFDynamicFeedSummaryMessage *pMsg ) {
  inherited_t::mapSymbols_t::iterator mapSymbols_iter;
  mapSymbols_iter = m_mapSymbols.find( pMsg->Field( IQFDynamicFeedSummaryMessage::DFSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != mapSymbols_iter ) {
    pSym = mapSymbols_iter -> second;
    pSym ->HandleDynamicFeedSummaryMessage( pMsg );
  }
  this->DynamicFeedSummaryDone( pBuffer, pMsg );
}

void IQFeedProvider::OnIQFeedUpdateMessage( linebuffer_t* pBuffer, IQFUpdateMessage *pMsg ) {
  inherited_t::mapSymbols_t::iterator mapSymbols_iter;
  mapSymbols_iter = m_mapSymbols.find( pMsg->Field( IQFUpdateMessage::QPSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != mapSymbols_iter ) {
    pSym = mapSymbols_iter -> second;
    pSym ->HandleUpdateMessage( pMsg );
  }
  this->UpdateDone( pBuffer, pMsg );
}

void IQFeedProvider::OnIQFeedSummaryMessage( linebuffer_t* pBuffer, IQFSummaryMessage *pMsg ) {
  inherited_t::mapSymbols_t::iterator mapSymbols_iter;
  mapSymbols_iter = m_mapSymbols.find( pMsg->Field( IQFSummaryMessage::QPSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != mapSymbols_iter ) {
    pSym = mapSymbols_iter -> second;
    pSym ->HandleSummaryMessage( pMsg );
  }
  this->SummaryDone( pBuffer, pMsg );
}

void IQFeedProvider::OnIQFeedFundamentalMessage( linebuffer_t* pBuffer, IQFFundamentalMessage *pMsg ) {
  inherited_t::mapSymbols_t::iterator mapSymbols_iter;
  mapSymbols_iter = m_mapSymbols.find( pMsg->Field( IQFFundamentalMessage::FSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != mapSymbols_iter ) {
    pSym = mapSymbols_iter -> second;
    pSym ->HandleFundamentalMessage(
      pMsg,
      [this](int nSecurityType )->ESecurityType { return LookupSecurityType( nSecurityType ); },
      [this](std::string sExchangeId)->std::string{ // supplied string is in hex
        int n {};
        int t {};
        for ( std::string::const_reverse_iterator iter = sExchangeId.rbegin(); iter != sExchangeId.rend(); iter++ ) {
          n = n << 4;
          char cur = *iter;
          if ( ( 'A' <= cur ) && ( 'F' >= cur ) ) {
            t = cur - 'A' + 10;
          }
          else {
            if ( ( 'a' <= cur ) && ( 'f' >= cur ) ) {
              t = cur - 'a' + 10;
            }
            else {
              if ( ( '0' <= cur ) && ( '9' >= cur ) ) {
                t = cur - '0';
              }
            }
          }
          n += t;
        }
        return LookupListedMarket( n );
      }
      );
  }
  this->FundamentalDone( pBuffer, pMsg );
}

void IQFeedProvider::OnIQFeedNewsMessage( linebuffer_t* pBuffer, IQFNewsMessage *pMsg ) {

  inherited_t::mapSymbols_t::iterator mapSymbols_iter;
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
  this->NewsDone( pBuffer, pMsg );
}

void IQFeedProvider::OnIQFeedTimeMessage( linebuffer_t* pBuffer, IQFTimeMessage *pMsg ) {
  //map<string, CSymbol*>::iterator m_mapSymbols_Iter;
  this->TimeDone( pBuffer, pMsg );
}

void IQFeedProvider::OnIQFeedSystemMessage( linebuffer_t* pBuffer, IQFSystemMessage *pMsg ) {
  //map<string, CSymbol*>::iterator m_mapSymbols_Iter;
  this->SystemDone( pBuffer, pMsg );
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
