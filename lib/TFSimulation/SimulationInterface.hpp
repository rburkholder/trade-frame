/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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

/*
 * File:    SimulationInterface.hpp
 * Author:  raymond@burkholder.net
 * Project: TFSimulation
 * Created: February 8, 2023 15:42:12
 */

#pragma once

#include <mutex>
#include <unordered_map>

#include <TFTrading/Order.h>
#include <TFTrading/ProviderInterface.h>

#include "SimulateOrderExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace sim { // simulation

template <typename P, typename S>  // p = provider, S = symbol
class SimulationInterface
: public ou::tf::ProviderInterface<P,S>
{
  friend ou::tf::ProviderInterface<P,S>;
public:

  using inherited_t = ou::tf::ProviderInterface<P,S>;

  using pOrder_t = ou::tf::Order::pOrder_t;
  using pSymbol_t = typename inherited_t::pSymbol_t;

  using pInstrument_t    = ou::tf::Instrument::pInstrument_t;
  using pInstrument_cref = ou::tf::Instrument::pInstrument_cref;

  SimulationInterface()
  : m_bExecutionEnabled( true )
  {}

  void SetCommission( const std::string& sSymbol, double commission );

  void PlaceOrder( pOrder_t pOrder );
  void CancelOrder( pOrder_t pOrder );

protected:

  bool m_bExecutionEnabled;

  pSymbol_t AddCSymbol( pSymbol_t );

  virtual void AddQuoteHandler( pInstrument_cref pInstrument, typename S::quotehandler_t );
  virtual void RemoveQuoteHandler( pInstrument_cref pInstrument, typename S::quotehandler_t );
  virtual void AddTradeHandler( pInstrument_cref pInstrument, typename S::tradehandler_t );
  virtual void RemoveTradeHandler( pInstrument_cref pInstrument, typename S::tradehandler_t );

  virtual void AddDepthByMMHandler( pInstrument_cref pInstrument, typename S::depthbymmhandler_t );
  virtual void RemoveDepthByMMHandler( pInstrument_cref pInstrument, typename S::depthbymmhandler_t );
  virtual void AddDepthByOrderHandler( pInstrument_cref pInstrument, typename S::depthbyorderhandler_t );
  virtual void RemoveDepthByOrderHandler( pInstrument_cref pInstrument, typename S::depthbyorderhandler_t );

  size_t MonitoredSymbolsCount() const { return m_mapOrderExecution.size(); }

private:

  struct EventHolders {
    OrderExecution oe;
    pSymbol_t pSymbol;
    EventHolders( pSymbol_t pSymbol_ )
    : pSymbol( std::move( pSymbol_ ) ) {}
    EventHolders( EventHolders&& rhs )
    : pSymbol( std::move( rhs.pSymbol ) ) {}
  };

  using mapOrderExecution_t = std::unordered_map<std::string,EventHolders>;
  mapOrderExecution_t m_mapOrderExecution;

  using fOrderExecution_t = std::function<void(EventHolders&)>;

  std::mutex m_mutex;

  void Update( const std::string& sName, fOrderExecution_t&& f ) {
    if ( m_bExecutionEnabled ) {
      std::scoped_lock<std::mutex> lock( m_mutex );
      typename mapOrderExecution_t::iterator iter = m_mapOrderExecution.find( sName );
      assert( m_mapOrderExecution.end() != iter );
      EventHolders& eh( iter->second );
      f( eh );
    }
  }
};

template <typename P, typename S>
typename SimulationInterface<P,S>::pSymbol_t SimulationInterface<P,S>::AddCSymbol( pSymbol_t pSymbol ) {

  if ( m_bExecutionEnabled ) {
    const std::string& sName( pSymbol->GetInstrument()->GetInstrumentName( inherited_t::ID() ) );

    std::scoped_lock<std::mutex> lock( m_mutex );
    typename mapOrderExecution_t::iterator iter = m_mapOrderExecution.find( sName );
    if ( m_mapOrderExecution.end() == iter ) {
      auto pair = m_mapOrderExecution.emplace( sName, EventHolders( pSymbol ) );
      assert( pair.second );
      iter = pair.first;
      EventHolders& eh( iter->second );
      OrderExecution& oe( eh.oe );
      oe.SetOnOrderFill( MakeDelegate( dynamic_cast<P*>( this ), &P::HandleExecution ) );
      oe.SetOnCommission( MakeDelegate( dynamic_cast<P*>( this ), &P::HandleCommission ) );
      oe.SetOnOrderCancelled( MakeDelegate( dynamic_cast<P*>( this ), &P::HandleCancellation ) );
    }
    else {
      assert( false );  // need better handling, this will be a duplicate
    }
  }

  return inherited_t::AddCSymbol( pSymbol );
}

template <typename P, typename S>
void SimulationInterface<P,S>::AddQuoteHandler( pInstrument_cref pInstrument, typename S::quotehandler_t handler ) {

  inherited_t::AddQuoteHandler( pInstrument, handler ); // needs to come first

  const std::string& sName( pInstrument->GetInstrumentName( inherited_t::ID() ) );
  Update(
    sName,
    [this,&pInstrument]( EventHolders& eh ){
      if ( 1 == eh.pSymbol->GetQuoteHandlerCount() ) {  // on first insertion add our own
        inherited_t::AddQuoteHandler( pInstrument, MakeDelegate( &eh.oe, &OrderExecution::NewQuote ) );
      }
    } );

}

template <typename P, typename S>
void SimulationInterface<P,S>::RemoveQuoteHandler( pInstrument_cref pInstrument, typename S::quotehandler_t handler ) {

  inherited_t::RemoveQuoteHandler( pInstrument, handler );

  const std::string& sName( pInstrument->GetInstrumentName( inherited_t::ID() ) );
  Update(
    sName,
    [this,&pInstrument]( EventHolders& eh ){
      if ( 1 == eh.pSymbol->GetQuoteHandlerCount() ) {  // on first insertion add our own
        inherited_t::RemoveQuoteHandler( pInstrument, MakeDelegate( &eh.oe, &OrderExecution::NewQuote ) );
      }
    } );

}

template <typename P, typename S>
void SimulationInterface<P,S>::AddTradeHandler( pInstrument_cref pInstrument, typename S::tradehandler_t handler ) {

  inherited_t::AddTradeHandler( pInstrument, handler ); // needs to come first

  const std::string& sName( pInstrument->GetInstrumentName( inherited_t::ID() ) );
  Update(
    sName,
    [this,&pInstrument]( EventHolders& eh ) {
      if ( 1 == eh.pSymbol->GetTradeHandlerCount() ) {  // on first insertion add our own
        inherited_t::AddTradeHandler( pInstrument, MakeDelegate( &eh.oe, &OrderExecution::NewTrade ) );
      }
    } );

}

template <typename P, typename S>
void SimulationInterface<P,S>::RemoveTradeHandler( pInstrument_cref pInstrument, typename S::tradehandler_t handler ) {

  inherited_t::RemoveTradeHandler( pInstrument, handler );

  const std::string& sName( pInstrument->GetInstrumentName( inherited_t::ID() ) );
  Update(
    sName,
    [this,&pInstrument]( EventHolders& eh ) {
      if ( 1 == eh.pSymbol->GetTradeHandlerCount() ) {
        inherited_t::RemoveTradeHandler( pInstrument, MakeDelegate( &eh.oe, &OrderExecution::NewTrade ) );
      }
    } );

}

template <typename P, typename S>
void SimulationInterface<P,S>::AddDepthByMMHandler( pInstrument_cref pInstrument, typename S::depthbymmhandler_t handler ) {

  inherited_t::AddDepthByMMHandler( pInstrument, handler ); // needs to come first

  const std::string& sName( pInstrument->GetInstrumentName( inherited_t::ID() ) );
  Update(
    sName,
    [this,&pInstrument]( EventHolders& eh ){
      if ( 1 == eh.pSymbol->GetDepthByMMHandlerCount() ) {  // on first insertion add our own
        inherited_t::AddDepthByMMHandler( pInstrument, MakeDelegate( &eh.oe, &OrderExecution::NewDepthByMM ) );
      }
    } );

}

template <typename P, typename S>
void SimulationInterface<P,S>::RemoveDepthByMMHandler( pInstrument_cref pInstrument, typename S::depthbymmhandler_t handler ) {

  inherited_t::RemoveDepthByMMHandler( pInstrument, handler );

  const std::string& sName( pInstrument->GetInstrumentName( inherited_t::ID() ) );
  Update(
    sName,
    [this,&pInstrument]( EventHolders& eh ){
      if ( 1 == eh.pSymbol->GetDepthByMMHandlerCount() ) {
        inherited_t::RemoveDepthByMMHandler( pInstrument, MakeDelegate( &eh.oe, &OrderExecution::NewDepthByMM ) );
      }
    } );

}

template <typename P, typename S>
void SimulationInterface<P,S>::AddDepthByOrderHandler( pInstrument_cref pInstrument, typename S::depthbyorderhandler_t handler ) {

  inherited_t::AddDepthByOrderHandler( pInstrument, handler );  // needs to come first

  const std::string& sName( pInstrument->GetInstrumentName( inherited_t::ID() ) );
  Update(
    sName,
    [this,&pInstrument]( EventHolders& eh ){
      if ( 1 == eh.pSymbol->GetDepthByOrderHandlerCount() ) {  // on first insertion add our own
        inherited_t::AddDepthByOrderHandler( pInstrument, MakeDelegate( &eh.oe, &OrderExecution::NewDepthByOrder ) );
      }
    } );

}

template <typename P, typename S>
void SimulationInterface<P,S>::RemoveDepthByOrderHandler( pInstrument_cref pInstrument, typename S::depthbyorderhandler_t handler ) {

  inherited_t::RemoveDepthByOrderHandler( pInstrument, handler );

  const std::string& sName( pInstrument->GetInstrumentName( inherited_t::ID() ) );
  Update(
    sName,
    [this,&pInstrument]( EventHolders& eh ){
      if ( 1 == eh.pSymbol->GetDepthByOrderHandlerCount() ) {
        inherited_t::RemoveDepthByOrderHandler( pInstrument, MakeDelegate( &eh.oe, &OrderExecution::NewDepthByOrder ) );
      }
    } );

}

template <typename P, typename S>
void SimulationInterface<P,S>::SetCommission( const std::string& sSymbol, double commission ) {

  Update(
    sSymbol,
    [commission]( EventHolders& eh ){
      eh.oe.SetCommission( commission );
    } );

}

template <typename P, typename S>
void SimulationInterface<P,S>::PlaceOrder( pOrder_t pOrder ) {

  inherited_t::PlaceOrder( pOrder );
  const std::string& sName( pOrder->GetInstrument()->GetInstrumentName( inherited_t::ID() ) );

  Update(
    sName,
    [pOrder]( EventHolders& eh ){
      eh.oe.SubmitOrder( pOrder );
    } );

}

template <typename P, typename S>
void SimulationInterface<P,S>::CancelOrder( pOrder_t pOrder ) {

  inherited_t::CancelOrder( pOrder );

  const std::string& sName( pOrder->GetInstrument()->GetInstrumentName( inherited_t::ID() ) );

  Update(
    sName,
    [pOrder]( EventHolders& eh ){
      eh.oe.CancelOrder( pOrder->GetOrderId() );
    } );

}

} // namespace sim
} // namespace tf
} // namespace ou
