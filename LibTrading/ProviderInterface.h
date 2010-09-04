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

#include <boost/shared_ptr.hpp>

#include <LibCommon/Delegate.h>

#include "Symbol.h"
#include "Order.h"
#include "OrderManager.h"

// need to include a check that callbacks and virtuals are in the correct thread
// in IB, processMsg may be best place to have in cross thread management, if it isn't already

// Store CInstrument in CSymbol<S>?  <= use this with smart_ptr on CInstrument.

/*
Discussion of calling sequence for open, quote, trade, depth handlers:
* client application calls Provider to add a handler
* CProviderInterface maintains list of symbols, 
   and will use the pure virtual override to create a new one when necessary 
*/

//
// =======================
//

class CProviderInterfaceBase {
public:

  typedef boost::shared_ptr<CProviderInterfaceBase> pProvider_t;

  typedef COrder::pOrder_t pOrder_t;
  typedef unsigned short enumProviderId_t;  // used in HDF5Attribute.h

  typedef CSymbolBase::quotehandler_t quotehandler_t;
  typedef CSymbolBase::tradehandler_t tradehandler_t;
  typedef CSymbolBase::depthhandler_t depthhandler_t;
  typedef CSymbolBase::greekhandler_t greekhandler_t;

  typedef CSymbolBase::pInstrument_t pInstrument_t;
  typedef CSymbolBase::pInstrument_cref pInstrument_cref;

  enum enumProviderId: enumProviderId_t { EProviderSimulator=100, EProviderIQF, EProviderIB, EProviderGNDT/*, _EProviderCount*/ };

  const std::string& Name( void ) const { return m_sName; };
  unsigned short ID( void ) const { assert( 0 != m_nID ); return m_nID; };

  CProviderInterfaceBase( void )
    : m_nID( 0 ), m_bConnected( false ),
      m_pProvidesBrokerInterface( false ),
      m_bProvidesQuotes( false ), m_bProvidesTrades( false ), m_bProvidesGreeks( false ), m_bProvidesDepth( false )
    {};
  virtual ~CProviderInterfaceBase( void ) {};

  virtual  void Connect( void ) {};
  Delegate<int> OnConnected;

  virtual  void Disconnect( void ) {};
  Delegate<int> OnDisconnected;

  bool ProvidesBrokerInterface( void ) const { return m_pProvidesBrokerInterface; };

  bool Connected( void ) const { return m_bConnected; };

  bool ProvidesQuotes( void ) const { return m_bProvidesQuotes; };
  bool ProvidesTrades( void ) const { return m_bProvidesTrades; };
  bool ProvidesDepth( void )  const { return m_bProvidesDepth; };
  bool ProvidesGreeks( void ) const { return m_bProvidesGreeks; };

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

protected:

  std::string m_sName;  // name of provider
  unsigned short m_nID;

  bool m_bConnected;

  bool m_pProvidesBrokerInterface;

  bool m_bProvidesQuotes;
  bool m_bProvidesTrades;
  bool m_bProvidesDepth;
  bool m_bProvidesGreeks;

private:

};

//
// =======================
//

template <typename P, typename S>  // p = provider, S = symbol
class CProviderInterface: public CProviderInterfaceBase {
public:

  typedef typename CSymbolBase::symbol_id_t symbol_id_t;
  typedef typename S::pSymbol_t pSymbol_t;

  CProviderInterface(void);
  ~CProviderInterface(void);

  void     AddQuoteHandler( pInstrument_cref pInstrument, quotehandler_t handler );
  void  RemoveQuoteHandler( pInstrument_cref pInstrument, quotehandler_t handler );

  void    AddOnOpenHandler( pInstrument_cref pInstrument, tradehandler_t handler );
  void RemoveOnOpenHandler( pInstrument_cref pInstrument, tradehandler_t handler );

  void     AddTradeHandler( pInstrument_cref pInstrument, tradehandler_t handler );
  void  RemoveTradeHandler( pInstrument_cref pInstrument, tradehandler_t handler );

  void     AddDepthHandler( pInstrument_cref pInstrument, depthhandler_t handler );
  void  RemoveDepthHandler( pInstrument_cref pInstrument, depthhandler_t handler );

  void     AddGreekHandler( pInstrument_cref pInstrument, greekhandler_t handler );
  void  RemoveGreekHandler( pInstrument_cref pInstrument, greekhandler_t handler );

  bool Exists( pInstrument_cref pInstrument );
  pSymbol_t Add( pInstrument_cref pInstrument );

  pSymbol_t GetSymbol( const symbol_id_t& );

  void  PlaceOrder( COrder::pOrder_t pOrder );
  void CancelOrder( COrder::pOrder_t pOrder );

protected:

   typedef std::map<symbol_id_t, pSymbol_t> m_mapSymbols_t;
  typedef std::pair<symbol_id_t, pSymbol_t> pair_mapSymbols_t;
  m_mapSymbols_t m_mapSymbols;

  virtual void StartQuoteWatch( pSymbol_t pSymbol ) {};
  virtual void  StopQuoteWatch( pSymbol_t pSymbol ) {};

  virtual void StartTradeWatch( pSymbol_t pSymbol ) {};
  virtual void  StopTradeWatch( pSymbol_t pSymbol ) {};

  virtual void StartDepthWatch( pSymbol_t pSymbol ) {};
  virtual void  StopDepthWatch( pSymbol_t pSymbol ) {};

  virtual void StartGreekWatch( pSymbol_t pSymbol ) {};
  virtual void  StopGreekWatch( pSymbol_t pSymbol ) {};

  virtual pSymbol_t NewCSymbol( pInstrument_t pInstrument ) = 0; 
  pSymbol_t AddCSymbol( pSymbol_t pSymbol );

private:
};

template <typename P, typename S>
CProviderInterface<P,S>::CProviderInterface(void) 
{
}

template <typename P, typename S>
CProviderInterface<P,S>::~CProviderInterface(void) {
  /*
  m_mapSymbols_t::iterator iter = m_mapSymbols.begin();
  while ( m_mapSymbols.end() != iter ) {
  // tod:  need to step through and unwatch anything still watching
    PreSymbolDestroy( iter->second );
    delete iter->second;
    ++iter;
  }
  */
  m_mapSymbols.clear();
}

template <typename P, typename S>
bool CProviderInterface<P,S>::Exists( pInstrument_cref pInstrument ) {
  m_mapSymbols_t::iterator iter = m_mapSymbols.find( pInstrument->GetInstrumentName( ID() ) );
  return ( m_mapSymbols.end() != iter );
}

template <typename P, typename S>
typename CProviderInterface<P,S>::pSymbol_t CProviderInterface<P,S>::Add( pInstrument_cref pInstrument ) {
   if ( Exists( pInstrument ) ) throw std::runtime_error( "Add:: Instrument already exists" );
   return NewCSymbol( pInstrument );
}

template <typename P, typename S>
typename CProviderInterface<P,S>::pSymbol_t CProviderInterface<P,S>::AddCSymbol( pSymbol_t pSymbol) {
  // todo:  add an assert to validate acceptable CSymbol type
  m_mapSymbols_t::iterator iter = m_mapSymbols.find( pSymbol->GetId() );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( pair_mapSymbols_t( pSymbol->GetId(), pSymbol ) );
    iter = m_mapSymbols.find( pSymbol->GetId() );
    assert( m_mapSymbols.end() != iter );
  }
  else {
    throw std::runtime_error( "AddCSymbol symbol already exists in provider" );
  }
  return iter->second;
}

template <typename P, typename S>
typename CProviderInterface<P,S>::pSymbol_t CProviderInterface<P,S>::GetSymbol( const symbol_id_t& id ) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( id );
  if ( m_mapSymbols.end() == iter ) {
    throw std::runtime_error( "GetSymbol did not find symbol" );
  }
  return iter->second;
}

template <typename P, typename S>
void CProviderInterface<P,S>::AddQuoteHandler(pInstrument_cref pInstrument, quotehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( 1 == 0 );
  }
  if ( iter->second->AddQuoteHandler( handler ) ) {
    StartQuoteWatch( iter->second );
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::RemoveQuoteHandler(pInstrument_cref pInstrument, quotehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( 1 == 0 );
  }
  else {
    if ( iter->second->RemoveQuoteHandler( handler ) ) {
      StopQuoteWatch( iter->second );
    }
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::AddTradeHandler(pInstrument_cref pInstrument, tradehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( 1 == 0 );
  }
  if ( iter->second->AddTradeHandler( handler ) ) {
    StartTradeWatch( iter->second );
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::RemoveTradeHandler(pInstrument_cref pInstrument, tradehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( 1 == 0 );
  }
  else {
    if ( iter->second->RemoveTradeHandler( handler ) ) {
      StopTradeWatch( iter->second );
    }
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::AddOnOpenHandler(pInstrument_cref pInstrument, tradehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( 1 == 0 );
  }
  iter->second->AddOnOpenHandler( handler );
}

template <typename P, typename S>
void CProviderInterface<P,S>::RemoveOnOpenHandler(pInstrument_cref pInstrument, tradehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( 1 == 0 );
  }
  else {
    iter->second->RemoveOnOpenHandler( handler );
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::AddDepthHandler(pInstrument_cref pInstrument, depthhandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( 1 == 0 );
  }
  if ( iter->second->AddDepthHandler( handler ) ) {
    StartDepthWatch( iter->second );
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::RemoveDepthHandler(pInstrument_cref pInstrument, depthhandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( 1 == 0 );
  }
  else {
    if ( iter->second->RemoveDepthHandler( handler ) ) {
      StopDepthWatch( iter->second );
    }
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::AddGreekHandler(pInstrument_cref pInstrument, greekhandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( 1 == 0 );
  }
  if ( iter->second->AddGreekHandler( handler ) ) {
    StartGreekWatch( iter->second );
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::RemoveGreekHandler(pInstrument_cref pInstrument, greekhandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( 1 == 0 );
  }
  else {
    if ( iter->second->RemoveGreekHandler( handler ) ) {
      StopGreekWatch( iter->second );
    }
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::PlaceOrder( pOrder_t pOrder ) {
//  pOrder->SetProviderName( m_sName );
//  this->GetSymbol( pOrder->GetInstrument()->GetSymbolName() );  // ensure we have the symbol locally registered
//  COrderManager::Instance().PlaceOrder( this, pOrder );
//  if ( &CProviderInterface<P,S>::PlaceOrder != &P::PlaceOrder ) {
//    static_cast<P*>( this )->PlaceOrder( pOrder );
//  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::CancelOrder( pOrder_t pOrder ) { 
//  pOrder->SetProviderName( m_sName );
//  COrderManager::Instance().CancelOrder( pOrder->GetOrderId() );
//  if ( &CProviderInterface<P,S>::CancelOrder != &P::CancelOrder ) {
//    static_cast<P*>( this )->CancelOrder( pOrder );
//  }
}

