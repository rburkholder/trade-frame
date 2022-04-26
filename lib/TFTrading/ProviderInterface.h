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

#pragma once

#include <map>
#include <string>
#include <stdexcept>
#include <algorithm>

#include <boost/shared_ptr.hpp>

#include <boost/thread.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/executor_work_guard.hpp>

#include <OUCommon/Delegate.h>

#include "KeyTypes.h"
#include "Symbol.h"
#include "Order.h"

// need to include a check that callbacks and virtuals are in the correct thread
// in IB, processMsg may be best place to have in cross thread management, if it isn't already

// Store CInstrument in CSymbol<S>?  <= use this with smart_ptr on CInstrument.

/*
Discussion of calling sequence for open, quote, trade, depth handlers:
* client application calls Provider to add a handler
* ProviderInterface maintains list of symbols,
   and will use the pure virtual override to create a new one when necessary
*/

//
// =======================
//

namespace ou { // One Unified
namespace tf { // TradeFrame

class ProviderInterfaceBase {
public:

  using pProvider_t = boost::shared_ptr<ProviderInterfaceBase>;

  using pOrder_t = Order::pOrder_t;
//  typedef keytypes::idProvider_t idProvider_t;

  using quotehandler_t = SymbolBase::quotehandler_t;
  using tradehandler_t = SymbolBase::tradehandler_t ;
  using depthhandler_t = SymbolBase::depthhandler_t;
  using greekhandler_t = SymbolBase::greekhandler_t;

  using pInstrument_t = SymbolBase::pInstrument_t;
  using pInstrument_cref =  SymbolBase::pInstrument_cref;

  using eidProvider_t = keytypes::eidProvider_t;

  const std::string& GetName() const { return m_sName; };
  void SetName( const std::string& sName ) { m_sName = sName; };
  eidProvider_t ID() const { assert( keytypes::EProviderUnknown != m_nID ); return m_nID; };

  ProviderInterfaceBase()
  : m_nID( keytypes::EProviderUnknown ), m_bConnected( false )
  , m_pProvidesBrokerInterface( false )
  , m_bProvidesQuotes( false ), m_bProvidesTrades( false ), m_bProvidesGreeks( false ), m_bProvidesDepths( false )
  , m_nThreads( 0 )
  {
    m_srvcWork = boost::asio::require(
      m_srvc.get_executor(),
      boost::asio::execution::outstanding_work.tracked );
  };

  virtual ~ProviderInterfaceBase() {
    m_srvcWork = boost::asio::any_io_executor();
    m_threads.join_all();
  };

  virtual void Connect() { // provides a worker thread for each provider
    if ( 0 < m_nThreads ) {
      if ( 0 == m_threads.size() ) { // one time initialization
        std::cout << "ProviderInterfaceBase::Connect using " << m_nThreads << " threads" << std::endl;
        for ( std::size_t ix = 0; ix < m_nThreads; ix++ ) {
          m_threads.create_thread( boost::bind( &boost::asio::io_context::run, &m_srvc ) ); // add handlers
        }
      }
    }
  }; // called by inheriting provider

  //virtual void Connecting() {}; // called by inheriting provider
  ou::Delegate<int> OnConnecting;
  ou::Delegate<int> OnConnected;  // could be in another thread
  //virtual void Connected() {}; // called by inheriting provider

  //virtual void Disconnecting( {}; // called by inheriting provider
  ou::Delegate<int> OnDisconnecting;
  ou::Delegate<int> OnDisconnected;  // could be in another thread
  //virtual void Disconnected( {}; // called by inheriting provider
  virtual void Disconnect() {}; // called by inheriting provider

  ou::Delegate<size_t> OnError;

  bool Connected() const { return m_bConnected; };

  bool ProvidesBrokerInterface() const { return m_pProvidesBrokerInterface; };

  bool ProvidesQuotes() const { return m_bProvidesQuotes; };
  bool ProvidesTrades() const { return m_bProvidesTrades; };
  bool ProvidesDepth()  const { return m_bProvidesDepths; };
  bool ProvidesGreeks() const { return m_bProvidesGreeks; };

  virtual void     AddQuoteHandler( pInstrument_cref pInstrument, quotehandler_t handler ) = 0;
  virtual void  RemoveQuoteHandler( pInstrument_cref pInstrument, quotehandler_t handler ) = 0;

  virtual void    AddOnOpenHandler( pInstrument_cref pInstrument, tradehandler_t handler ) = 0;
  virtual void RemoveOnOpenHandler( pInstrument_cref pInstrument, tradehandler_t handler ) = 0;

  virtual void     AddTradeHandler( pInstrument_cref pInstrument, tradehandler_t handler ) = 0;
  virtual void  RemoveTradeHandler( pInstrument_cref pInstrument, tradehandler_t handler ) = 0;

  virtual void     AddDepthHandler( pInstrument_cref pInstrument, depthhandler_t handler ) = 0;
  virtual void  RemoveDepthHandler( pInstrument_cref pInstrument, depthhandler_t handler ) = 0;

  virtual void     AddGreekHandler( pInstrument_cref pInstrument, greekhandler_t handler ) = 0;
  virtual void  RemoveGreekHandler( pInstrument_cref pInstrument, greekhandler_t handler ) = 0;

  virtual void PlaceOrder( pOrder_t pOrder ) = 0;
  virtual void CancelOrder( pOrder_t pOrder ) = 0;

  typedef FastDelegate0<void> OnSecurityDefinitionNotFoundHandler_t;
  void SetOnSecurityDefinitionNotFoundHandler( OnSecurityDefinitionNotFoundHandler_t function ) {
    OnSecurityDefinitionNotFound = function;
  }

  // strong suggestion: set prior to connect
  //   affects context and optional thread usage (to be implemented
  void SetThreadCount( size_t nThreads ) {
    assert( 0 < nThreads );
    m_nThreads = nThreads;
    }

protected:

  std::string m_sName;  // name of provider
  eidProvider_t m_nID;

  bool m_bConnected;

  bool m_pProvidesBrokerInterface;

  bool m_bProvidesQuotes;
  bool m_bProvidesTrades;
  bool m_bProvidesDepths;
  bool m_bProvidesGreeks;

  size_t m_nThreads;

  boost::asio::io_context m_srvc; // threads for use in symbols
  boost::asio::any_io_executor m_srvcWork;
  boost::thread_group m_threads;

  OnSecurityDefinitionNotFoundHandler_t OnSecurityDefinitionNotFound;

private:

};

//
// =======================
//

template <typename P, typename S>  // p = provider, S = symbol
class ProviderInterface: public ProviderInterfaceBase {
public:

  using idSymbol_t = typename SymbolBase::idSymbol_t;
  using pSymbol_t = typename S::pSymbol_t;

  ProviderInterface();
  virtual ~ProviderInterface();

  void    AddOnOpenHandler( pInstrument_cref pInstrument, tradehandler_t handler );
  void RemoveOnOpenHandler( pInstrument_cref pInstrument, tradehandler_t handler );

  void     AddQuoteHandler( pInstrument_cref pInstrument, quotehandler_t handler );
  void  RemoveQuoteHandler( pInstrument_cref pInstrument, quotehandler_t handler );

  void     AddTradeHandler( pInstrument_cref pInstrument, tradehandler_t handler );
  void  RemoveTradeHandler( pInstrument_cref pInstrument, tradehandler_t handler );

  void     AddDepthHandler( pInstrument_cref pInstrument, depthhandler_t handler );
  void  RemoveDepthHandler( pInstrument_cref pInstrument, depthhandler_t handler );

  void     AddGreekHandler( pInstrument_cref pInstrument, greekhandler_t handler );
  void  RemoveGreekHandler( pInstrument_cref pInstrument, greekhandler_t handler );

  bool Exists( pInstrument_cref pInstrument );
  pSymbol_t Add( pInstrument_cref pInstrument );

  pSymbol_t GetSymbol( const idSymbol_t& );
  pSymbol_t GetSymbol( const pInstrument_t );

  void  PlaceOrder( Order::pOrder_t pOrder );
  void CancelOrder( Order::pOrder_t pOrder );

protected:

  using mapSymbols_t = std::map<idSymbol_t, pSymbol_t>;
  mapSymbols_t m_mapSymbols;

  //void Connecting( void );
  void ConnectionComplete();
  void Disconnecting();
  //void Disconnected();

  virtual void StartQuoteWatch( pSymbol_t pSymbol ) {};
  virtual void  StopQuoteWatch( pSymbol_t pSymbol ) {};

  virtual void StartTradeWatch( pSymbol_t pSymbol ) {};
  virtual void  StopTradeWatch( pSymbol_t pSymbol ) {};

  virtual void StartDepthWatch( pSymbol_t pSymbol ) {};
  virtual void  StopDepthWatch( pSymbol_t pSymbol ) {};

  virtual void StartGreekWatch( pSymbol_t pSymbol ) {};
  virtual void  StopGreekWatch( pSymbol_t pSymbol ) {};

  bool Exists( pInstrument_cref pInstrument, typename mapSymbols_t::iterator& iter );

  virtual pSymbol_t NewCSymbol( pInstrument_t pInstrument ) = 0;
  pSymbol_t AddCSymbol( pSymbol_t pSymbol );

private:

  typename mapSymbols_t::iterator Find( const pInstrument_t );

};

template <typename P, typename S>
ProviderInterface<P,S>::ProviderInterface(void)
{
}

template <typename P, typename S>
ProviderInterface<P,S>::~ProviderInterface(void) {
  m_mapSymbols.clear();
}

template <typename P, typename S>
void ProviderInterface<P,S>::ConnectionComplete() {
  std::for_each( m_mapSymbols.begin(), m_mapSymbols.end(),
    [this](typename mapSymbols_t::value_type& vt){
      if ( vt.second->GetQuoteHandlerCount() ) StartQuoteWatch( vt.second );
      if ( vt.second->GetTradeHandlerCount() ) StartTradeWatch( vt.second );
      if ( vt.second->GetDepthHandlerCount() ) StartDepthWatch( vt.second );
      if ( vt.second->GetGreekHandlerCount() ) StartGreekWatch( vt.second );
    }
    );
}

template <typename P, typename S>
void ProviderInterface<P,S>::Disconnecting() {
  std::for_each( m_mapSymbols.begin(), m_mapSymbols.end(),
    [this](typename mapSymbols_t::value_type& vt){
      if ( vt.second->GetTradeHandlerCount() ) StopTradeWatch( vt.second );
      if ( vt.second->GetQuoteHandlerCount() ) StopQuoteWatch( vt.second );
      if ( vt.second->GetDepthHandlerCount() ) StopDepthWatch( vt.second );
      if ( vt.second->GetGreekHandlerCount() ) StopGreekWatch( vt.second );
    }
  );
}

template <typename P, typename S>
bool ProviderInterface<P,S>::Exists( pInstrument_cref pInstrument ) {
  typename mapSymbols_t::iterator iter = m_mapSymbols.find( pInstrument->GetInstrumentName( ID() ) );
  bool b( m_mapSymbols.end() != iter );
  return b;
}

template <typename P, typename S>
bool ProviderInterface<P,S>::Exists( pInstrument_cref pInstrument, typename mapSymbols_t::iterator& iter ) {
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( ID() ) );
  bool b( m_mapSymbols.end() != iter );
  return b;
}

template <typename P, typename S>
typename ProviderInterface<P,S>::pSymbol_t ProviderInterface<P,S>::Add( pInstrument_cref pInstrument ) {
  if ( Exists( pInstrument ) ) throw std::runtime_error( "Add:: Instrument already exists" );
  return NewCSymbol( pInstrument );
}

template <typename P, typename S>
typename ProviderInterface<P,S>::pSymbol_t ProviderInterface<P,S>::AddCSymbol( pSymbol_t pSymbol) {
  // todo:  add an assert to validate acceptable CSymbol type

  if ( 0 < m_nThreads ) {
    pSymbol->SetContext( m_srvc );
  }

  typename mapSymbols_t::iterator iter = m_mapSymbols.find( pSymbol->GetId() );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( typename mapSymbols_t::value_type( pSymbol->GetId(), pSymbol ) );
    iter = m_mapSymbols.find( pSymbol->GetId() );
    assert( m_mapSymbols.end() != iter );
  }
  else {
    throw std::runtime_error( "AddCSymbol " + pSymbol->GetId() + " symbol already exists in provider" );
  }

  return iter->second;
}

template <typename P, typename S>
typename ProviderInterface<P,S>::mapSymbols_t::iterator ProviderInterface<P,S>:: Find( const pInstrument_t pInstrument ) {
  typename mapSymbols_t::iterator iter;
  if ( !Exists( pInstrument, iter ) ) {
    Add( pInstrument );
    iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
    assert( m_mapSymbols.end() != iter );
  }
  return iter;
}


template <typename P, typename S>
typename ProviderInterface<P,S>::pSymbol_t ProviderInterface<P,S>::GetSymbol( const idSymbol_t& id ) {
  typename mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( id );
  if ( m_mapSymbols.end() == iter ) {
    throw std::runtime_error( "GetSymbol did not find symbol " + id );
  }
  return iter->second;
}

template <typename P, typename S>
typename ProviderInterface<P,S>::pSymbol_t ProviderInterface<P,S>:: GetSymbol( const pInstrument_t pInstrument ) {
  return Find( pInstrument )->second;
}

template <typename P, typename S>
void ProviderInterface<P,S>::AddQuoteHandler(pInstrument_cref pInstrument, quotehandler_t handler) {
  typename mapSymbols_t::iterator iter = Find( pInstrument );
  if ( iter->second->AddQuoteHandler( handler ) ) {
    if ( m_bConnected ) StartQuoteWatch( iter->second );
  }
}

template <typename P, typename S>
void ProviderInterface<P,S>::RemoveQuoteHandler(pInstrument_cref pInstrument, quotehandler_t handler) {
  typename mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( 1 == 0 );
  }
  else {
    if ( iter->second->RemoveQuoteHandler( handler ) ) {
      if ( m_bConnected ) StopQuoteWatch( iter->second );
    }
  }
}

template <typename P, typename S>
void ProviderInterface<P,S>::AddTradeHandler(pInstrument_cref pInstrument, tradehandler_t handler) {
  typename mapSymbols_t::iterator iter = Find( pInstrument );
  if ( iter->second->AddTradeHandler( handler ) ) {
    if ( m_bConnected ) StartTradeWatch( iter->second );
  }
}

template <typename P, typename S>
void ProviderInterface<P,S>::RemoveTradeHandler(pInstrument_cref pInstrument, tradehandler_t handler) {
  typename mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( 1 == 0 );
  }
  else {
    if ( iter->second->RemoveTradeHandler( handler ) ) {
      if ( m_bConnected ) StopTradeWatch( iter->second );
    }
  }
}

template <typename P, typename S>
void ProviderInterface<P,S>::AddOnOpenHandler(pInstrument_cref pInstrument, tradehandler_t handler) {
  typename mapSymbols_t::iterator iter = Find( pInstrument );
  iter->second->AddOnOpenHandler( handler );
}

template <typename P, typename S>
void ProviderInterface<P,S>::RemoveOnOpenHandler(pInstrument_cref pInstrument, tradehandler_t handler) {
  typename mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( 0 );
  }
  else {
    iter->second->RemoveOnOpenHandler( handler );
  }
}

template <typename P, typename S>
void ProviderInterface<P,S>::AddDepthHandler(pInstrument_cref pInstrument, depthhandler_t handler) {
  typename mapSymbols_t::iterator iter = Find( pInstrument );
  if ( iter->second->AddDepthHandler( handler ) ) {
    if ( m_bConnected ) StartDepthWatch( iter->second );
  }
}

template <typename P, typename S>
void ProviderInterface<P,S>::RemoveDepthHandler(pInstrument_cref pInstrument, depthhandler_t handler) {
  typename mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( 1 == 0 );
  }
  else {
    if ( iter->second->RemoveDepthHandler( handler ) ) {
      if ( m_bConnected ) StopDepthWatch( iter->second );
    }
  }
}

template <typename P, typename S>
void ProviderInterface<P,S>::AddGreekHandler(pInstrument_cref pInstrument, greekhandler_t handler) {
  typename mapSymbols_t::iterator iter = Find( pInstrument );
  if ( iter->second->AddGreekHandler( handler ) ) {
    if ( m_bConnected ) StartGreekWatch( iter->second );
  }
}

template <typename P, typename S>
void ProviderInterface<P,S>::RemoveGreekHandler(pInstrument_cref pInstrument, greekhandler_t handler) {
  typename mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( 1 == 0 );
  }
  else {
    if ( iter->second->RemoveGreekHandler( handler ) ) {
      if ( m_bConnected ) StopGreekWatch( iter->second );
    }
  }
}

template <typename P, typename S>
void ProviderInterface<P,S>::PlaceOrder( pOrder_t pOrder ) {
//  pOrder->SetProviderName( m_sName );
//  this->GetSymbol( pOrder->GetInstrument()->GetSymbolName() );  // ensure we have the symbol locally registered
//  COrderManager::Instance().PlaceOrder( this, pOrder );
//  if ( &ProviderInterface<P,S>::PlaceOrder != &P::PlaceOrder ) {
//    static_cast<P*>( this )->PlaceOrder( pOrder );
//  }
}

template <typename P, typename S>
void ProviderInterface<P,S>::CancelOrder( pOrder_t pOrder ) {
//  pOrder->SetProviderName( m_sName );
//  COrderManager::Instance().CancelOrder( pOrder->GetOrderId() );
//  if ( &ProviderInterface<P,S>::CancelOrder != &P::CancelOrder ) {
//    static_cast<P*>( this )->CancelOrder( pOrder );
//  }
}

} // namespace tf
} // namespace ou
