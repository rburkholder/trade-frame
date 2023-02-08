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
#include <TFTrading/OrderManager.h>

#include "Provider.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

Provider::Provider()
: ProviderInterface<Provider,IQFeedSymbol>()
, IQFeed<Provider>()
, m_bExecutionEnabled( false )
{
  m_sName = "IQFeed";
  m_nID = keytypes::EProviderIQF;
  m_bProvidesQuotes = true;
  m_bProvidesTrades = true;
  m_bProvidesDepths = true;
  m_bProvidesBrokerInterface = true; // simulated
}

Provider::~Provider() {
}

void Provider::Connect() {
  if ( !m_bConnected ) {
    ProviderInterfaceBase::OnConnecting( 0 );
    inherited_t::Connect();
    IQFeed_t::Connect();
  }
}

void Provider::OnIQFeedConnected() {
  m_bConnected = true;
  inherited_t::ConnectionComplete();
  ProviderInterfaceBase::OnConnected( 0 );
}

void Provider::Disconnect() {
  if ( m_bConnected ) {
    ProviderInterfaceBase::OnDisconnecting( 0 ); // watches are regsitered here
    inherited_t::Disconnecting();  // provider then cleans up
    IQFeed_t::Disconnect();
    inherited_t::Disconnect();
  }
}

void Provider::OnIQFeedDisConnected() {
  m_bConnected = false;
  ProviderInterfaceBase::OnDisconnected( 0 );
}

void Provider::OnIQFeedError( size_t e ) {
  OnError( e );
}

Provider::pSymbol_t Provider::NewCSymbol( pInstrument_t pInstrument ) {
  pSymbol_t pSymbol( new IQFeedSymbol( pInstrument->GetInstrumentName( ID() ), pInstrument ) );
  pSymbol->m_simExec.SetOnOrderFill( MakeDelegate( this, &Provider::HandleExecution ) );
  pSymbol->m_simExec.SetOnCommission( MakeDelegate( this, &Provider::HandleCommission ) );
  pSymbol->m_simExec.SetOnOrderCancelled( MakeDelegate( this, &Provider::HandleCancellation ) );
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

void Provider::UpdateQuoteTradeWatch( char command, IQFeedSymbol::WatchState next, IQFeedSymbol* pSymbol ) {
  if ( '-' != command ) {
    std::string s = command + pSymbol->GetId() + "\n";
    //std::cout << command + pSymbol->GetId() << std::endl;
    IQFeed<Provider>::Send( s );
  }
  pSymbol->SetWatchState( next );
}

void Provider::StartQuoteWatch( pSymbol_t pSymbol ) {
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

void Provider::StopQuoteWatch(pSymbol_t pSymbol) {
  IQFeedSymbol::WatchState current = pSymbol->GetWatchState();
  IQFeedSymbol::WatchState next = IQFeedSymbol::WatchState::None;
  switch ( current ) {
    case IQFeedSymbol::WatchState::None:
      std::cout << "iqfeed::Provider::StopQuoteWatch error with None: " << pSymbol->GetId() << std::endl;
      break;
    case IQFeedSymbol::WatchState::WSQuote:
      next = IQFeedSymbol::WatchState::None;
      UpdateQuoteTradeWatch( transition[current][next], next, dynamic_cast<IQFeedSymbol*>( pSymbol.get() ) );
      break;
    case IQFeedSymbol::WatchState::WSTrade:
      std::cout << "iqfeed::Provider::StopQuoteWatch error with Trade: " << pSymbol->GetId() << std::endl;
      break;
    case IQFeedSymbol::WatchState::Both:
      next = IQFeedSymbol::WatchState::WSTrade;
      UpdateQuoteTradeWatch( transition[current][next], next, dynamic_cast<IQFeedSymbol*>( pSymbol.get() ) );
      break;
  }
}

void Provider::StartTradeWatch(pSymbol_t pSymbol) {
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

void Provider::StopTradeWatch(pSymbol_t pSymbol) {
  IQFeedSymbol::WatchState current = pSymbol->GetWatchState();
  IQFeedSymbol::WatchState next = IQFeedSymbol::WatchState::None;
  switch ( current ) {
    case IQFeedSymbol::WatchState::None:
      std::cout << "iqfeed::Provider::StopTradeWatch error with None: " << pSymbol->GetId() << std::endl;
      break;
    case IQFeedSymbol::WatchState::WSQuote:
      std::cout << "iqfeed::Provider::StopTradeWatch error with Quote: " << pSymbol->GetId() << std::endl;
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

void Provider::OnIQFeedDynamicFeedUpdateMessage( linebuffer_t* pBuffer, IQFDynamicFeedUpdateMessage *pMsg ) {
  inherited_t::mapSymbols_t::iterator mapSymbols_iter;
  mapSymbols_iter = m_mapSymbols.find( pMsg->Field( IQFDynamicFeedUpdateMessage::DFSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != mapSymbols_iter ) {
    pSym = mapSymbols_iter -> second;
    pSym ->HandleDynamicFeedUpdateMessage( pMsg );
  }
  this->DynamicFeedUpdateDone( pBuffer, pMsg );
}

void Provider::OnIQFeedDynamicFeedSummaryMessage( linebuffer_t* pBuffer, IQFDynamicFeedSummaryMessage *pMsg ) {
  inherited_t::mapSymbols_t::iterator mapSymbols_iter;
  mapSymbols_iter = m_mapSymbols.find( pMsg->Field( IQFDynamicFeedSummaryMessage::DFSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != mapSymbols_iter ) {
    pSym = mapSymbols_iter -> second;
    pSym ->HandleDynamicFeedSummaryMessage( pMsg );
  }
  this->DynamicFeedSummaryDone( pBuffer, pMsg );
}

void Provider::OnIQFeedUpdateMessage( linebuffer_t* pBuffer, IQFUpdateMessage *pMsg ) {
  inherited_t::mapSymbols_t::iterator mapSymbols_iter;
  mapSymbols_iter = m_mapSymbols.find( pMsg->Field( IQFUpdateMessage::QPSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != mapSymbols_iter ) {
    pSym = mapSymbols_iter -> second;
    pSym ->HandleUpdateMessage( pMsg );
  }
  this->UpdateDone( pBuffer, pMsg );
}

void Provider::OnIQFeedSummaryMessage( linebuffer_t* pBuffer, IQFSummaryMessage *pMsg ) {
  inherited_t::mapSymbols_t::iterator mapSymbols_iter;
  mapSymbols_iter = m_mapSymbols.find( pMsg->Field( IQFSummaryMessage::QPSymbol ) );
  pSymbol_t pSym;
  if ( m_mapSymbols.end() != mapSymbols_iter ) {
    pSym = mapSymbols_iter -> second;
    pSym ->HandleSummaryMessage( pMsg );
  }
  this->SummaryDone( pBuffer, pMsg );
}

void Provider::OnIQFeedFundamentalMessage( linebuffer_t* pBuffer, IQFFundamentalMessage *pMsg ) {
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

void Provider::OnIQFeedNewsMessage( linebuffer_t* pBuffer, IQFNewsMessage *pMsg ) {

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

void Provider::OnIQFeedTimeMessage( linebuffer_t* pBuffer, IQFTimeMessage *pMsg ) {
  //map<string, CSymbol*>::iterator m_mapSymbols_Iter;
  this->TimeDone( pBuffer, pMsg );
}

void Provider::OnIQFeedSystemMessage( linebuffer_t* pBuffer, IQFSystemMessage *pMsg ) {
  //map<string, CSymbol*>::iterator m_mapSymbols_Iter;
  this->SystemDone( pBuffer, pMsg );
}

void Provider::HandleExecution( Order::idOrder_t orderId, const Execution &exec ) {
  OrderManager::LocalCommonInstance().ReportExecution( orderId, exec );
}

void Provider::HandleCommission( Order::idOrder_t orderId, double commission ) {
  OrderManager::LocalCommonInstance().ReportCommission( orderId, commission );
}

void Provider::HandleCancellation( Order::idOrder_t orderId ) {
  OrderManager::LocalCommonInstance().ReportCancellation( orderId );
}

void Provider::SetCommission( const std::string& sSymbol, double commission ) {
  mapSymbols_t::iterator iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    assert( false );
  }
  else {
    iter->second->m_simExec.SetCommission( commission );
  }
}

void Provider::AddQuoteHandler( pInstrument_cref pInstrument, Provider::quotehandler_t handler ) {
  inherited_t::AddQuoteHandler( pInstrument, handler );
  if ( m_bExecutionEnabled ) {
    inherited_t::mapSymbols_t::iterator iter;
    iter = m_mapSymbols.find( pInstrument->GetInstrumentName( ID() ) );
    assert( m_mapSymbols.end() != iter );
    pSymbol_t& pSymSymbol( iter->second );
    if ( 1 == pSymSymbol->GetQuoteHandlerCount() ) { // on first assignment, add our own assignment
      inherited_t::AddQuoteHandler( pInstrument, MakeDelegate( &pSymSymbol->m_simExec, &sim::OrderExecution::NewQuote ) );
    }
  }
}

void Provider::RemoveQuoteHandler( pInstrument_cref pInstrument, Provider::quotehandler_t handler ) {
  inherited_t::RemoveQuoteHandler( pInstrument, handler );
  if ( m_bExecutionEnabled ) {
    inherited_t::mapSymbols_t::iterator iter;
    iter = m_mapSymbols.find( pInstrument->GetInstrumentName( ID() ) );
    assert( m_mapSymbols.end() != iter );
    pSymbol_t& pSymSymbol( iter->second );
    if ( 1 == pSymSymbol->GetQuoteHandlerCount() ) { // on last removal, remove our own assignment
      inherited_t::RemoveQuoteHandler( pInstrument, MakeDelegate( &pSymSymbol->m_simExec, &sim::OrderExecution::NewQuote ) );
    }
  }
}

void Provider::AddTradeHandler( pInstrument_cref pInstrument, Provider::tradehandler_t handler ) {
  inherited_t::AddTradeHandler( pInstrument, handler );
  if ( m_bExecutionEnabled ) {
    inherited_t::mapSymbols_t::iterator iter;
    iter = m_mapSymbols.find( pInstrument->GetInstrumentName( ID() ) );
    assert( m_mapSymbols.end() != iter );
    pSymbol_t& pSymSymbol( iter->second );
    if ( 1 == pSymSymbol->GetTradeHandlerCount() ) { // on first assignment, add our own assignment
      inherited_t::AddTradeHandler( pInstrument, MakeDelegate( &pSymSymbol->m_simExec, &sim::OrderExecution::NewTrade ) );
    }
  }
}

void Provider::RemoveTradeHandler( pInstrument_cref pInstrument, Provider::tradehandler_t handler ) {
  inherited_t::RemoveTradeHandler( pInstrument, handler );
  if ( m_bExecutionEnabled ) {
    inherited_t::mapSymbols_t::iterator iter;
    iter = m_mapSymbols.find( pInstrument->GetInstrumentName( ID() ) );
    assert( m_mapSymbols.end() != iter );
    pSymbol_t& pSymSymbol( iter->second );
    if ( 1 == pSymSymbol->GetTradeHandlerCount() ) { // on last removal, remove our own assignment
      inherited_t::RemoveTradeHandler( pInstrument, MakeDelegate( &pSymSymbol->m_simExec, &sim::OrderExecution::NewTrade ) );
    }
  }
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
