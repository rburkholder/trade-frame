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

#include "boost/shared_ptr.hpp"

#include "LibCommon/Delegate.h"

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
  typedef unsigned short enumProviderId_t;

  enum enumProviderId: enumProviderId_t { EProviderSimulator=100, EProviderIQF, EProviderIB, EProviderGNDT, _EProviderCount };

  const std::string& Name( void ) { return m_sName; };
  unsigned short ID( void ) { assert( 0 != m_nID ); return m_nID; };

  CProviderInterfaceBase( void )
    : m_nID( 0 ), m_bConnected( false ),
      m_bProvidesQuotes( false ), m_bProvidesTrades( false ), m_bProvidesGreeks( false )
    {};
  virtual ~CProviderInterfaceBase( void ) {};

  bool Connected( void ) { return m_bConnected; };

  bool ProvidesQuotes( void ) { return m_bProvidesQuotes; };
  bool ProvidesTrades( void ) { return m_bProvidesQuotes; };
  bool ProvidesGreeks( void ) { return m_bProvidesGreeks; };

  virtual void PlaceOrder( pOrder_t pOrder ) = 0;
  virtual void CancelOrder( pOrder_t pOrder ) = 0;

protected:

  std::string m_sName;  // name of provider
  unsigned short m_nID;

  bool m_bConnected;

  bool m_bProvidesQuotes;
  bool m_bProvidesTrades;
  bool m_bProvidesGreeks;

private:

};

//
// =======================
//

template <typename P, typename S>  // p = provider, S = symbol
class CProviderInterface: public CProviderInterfaceBase {
public:

  typedef typename S::pInstrument_t pInstrument_t;
  typedef typename S::symbol_id_t symbol_id_t;

  CProviderInterface(void);
  ~CProviderInterface(void);

  virtual  void Connect( void );
  Delegate<int> OnConnected;

  virtual  void Disconnect( void );
  Delegate<int> OnDisconnected;

  virtual void     AddQuoteHandler( const symbol_id_t& id, typename S::quotehandler_t handler );
  virtual void  RemoveQuoteHandler( const symbol_id_t& id, typename S::quotehandler_t handler );

  virtual void    AddOnOpenHandler( const symbol_id_t& id, typename S::tradehandler_t handler );
  virtual void RemoveOnOpenHandler( const symbol_id_t& id, typename S::tradehandler_t handler );

  virtual void     AddTradeHandler( const symbol_id_t& id, typename S::tradehandler_t handler );
  virtual void  RemoveTradeHandler( const symbol_id_t& id, typename S::tradehandler_t handler );

  virtual void     AddDepthHandler( const symbol_id_t& id, typename S::depthhandler_t handler );
  virtual void  RemoveDepthHandler( const symbol_id_t& id, typename S::depthhandler_t handler );

  virtual void     AddGreekHandler( const symbol_id_t& id, typename S::greekhandler_t handler );
  virtual void  RemoveGreekHandler( const symbol_id_t& id, typename S::greekhandler_t handler );

//  Delegate<CPortfolio::UpdatePortfolioRecord_t> OnUpdatePortfolioRecord;  // need to do the Add/Remove thing

  S* GetSymbol( const symbol_id_t& );

  void PlaceOrder( COrder::pOrder_t pOrder );
  void CancelOrder( COrder::pOrder_t pOrder );

protected:

   typedef std::map<symbol_id_t, S*> m_mapSymbols_t;
  typedef std::pair<symbol_id_t, S*> pair_mapSymbols_t;
  m_mapSymbols_t m_mapSymbols;

  virtual void StartQuoteWatch( S* pSymbol ) {};
  virtual void  StopQuoteWatch( S* pSymbol ) {};

  virtual void StartTradeWatch( S* pSymbol ) {};
  virtual void  StopTradeWatch( S* pSymbol ) {};

  virtual void StartDepthWatch( S* pSymbol ) {};
  virtual void  StopDepthWatch( S* pSymbol ) {};

//  virtual S *NewCSymbol( const std::string& sSymbolName ) = 0; // override needs to call AddCSymbol to add symbol to map
  virtual S *NewCSymbol( pInstrument_t pInstrument ) = 0; 
  S* AddCSymbol( S* pSymbol );
  virtual void PreSymbolDestroy( S* pSymbol );

private:
};

template <typename P, typename S>
CProviderInterface<P,S>::CProviderInterface(void) 
{
}

template <typename P, typename S>
CProviderInterface<P,S>::~CProviderInterface(void) {
  m_mapSymbols_t::iterator iter = m_mapSymbols.begin();
  while ( m_mapSymbols.end() != iter ) {
  // tod:  need to step through and unwatch anything still watching
    PreSymbolDestroy( iter->second );
    delete iter->second;
    ++iter;
  }
  m_mapSymbols.clear();
}

template <typename P, typename S>
void CProviderInterface<P,S>::Connect() {
  // override for provider specific code
}

template <typename P, typename S>
void CProviderInterface<P,S>::Disconnect() {
  // override for provider specific code
}

template <typename P, typename S>
S* CProviderInterface<P,S>::AddCSymbol( S* pSymbol) {
  // todo:  add an assert to validate acceptable CSymbol type
  m_mapSymbols_t::iterator iter = m_mapSymbols.find( pSymbol->GetId() );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( pair_mapSymbols_t( pSymbol->GetId(), pSymbol ) );
    iter = m_mapSymbols.find( pSymbol->GetId() );
    assert( m_mapSymbols.end() != iter );
  }
  return iter->second;
}

template <typename P, typename S>
void CProviderInterface<P,S>::AddQuoteHandler(const symbol_id_t& id, typename S::quotehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( id );
  if ( m_mapSymbols.end() == iter ) {
//    m_mapSymbols.insert( pair_mapSymbols_t( id, NewCSymbol( sSymbol ) ) );
//    iter = m_mapSymbols.find( sSymbol );
    assert( 1 == 0 );
  }
  if ( iter->second->AddQuoteHandler( handler ) ) {
    StartQuoteWatch( iter->second );
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::RemoveQuoteHandler(const symbol_id_t& id, typename S::quotehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( id );
  if ( m_mapSymbols.end() == iter ) {
//    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, new S( sSymbol ) ) );
    assert( 1 == 0 );
  }
  else {
    if ( iter->second->RemoveQuoteHandler( handler ) ) {
      StopQuoteWatch( iter->second );
    }
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::AddTradeHandler(const symbol_id_t& id, typename S::tradehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( id );
  if ( m_mapSymbols.end() == iter ) {
//    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, NewCSymbol( sSymbol ) ) );
//    iter = m_mapSymbols.find( sSymbol );
    assert( 1 == 0 );
  }
  if ( iter->second->AddTradeHandler( handler ) ) {
    StartTradeWatch( iter->second );
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::RemoveTradeHandler(const symbol_id_t& id, typename S::tradehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( id );
  if ( m_mapSymbols.end() == iter ) {
//    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, new S( sSymbol ) ) );
    // should probably raise exception here as trying to remove handler from non-existtant symbol
    assert( 1 == 0 );
  }
  else {
    if ( iter->second->RemoveTradeHandler( handler ) ) {
      StopTradeWatch( iter->second );
    }
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::AddOnOpenHandler(const symbol_id_t& id, typename S::tradehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( id );
  if ( m_mapSymbols.end() == iter ) {
//    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, NewCSymbol( sSymbol ) ) );
//    iter = m_mapSymbols.find( sSymbol );
    assert( 1 == 0 );
  }
  iter->second->AddOnOpenHandler( handler );
}

template <typename P, typename S>
void CProviderInterface<P,S>::RemoveOnOpenHandler(const symbol_id_t& id, typename S::tradehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( id );
  if ( m_mapSymbols.end() == iter ) {
//    m_mapSymbols.insert( std::pair<string, S*>( sSymbol, new S( sSymbol ) ) );
    assert( 1 == 0 );
  }
  else {
    iter->second->RemoveOnOpenHandler( handler );
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::AddDepthHandler(const symbol_id_t& id, typename S::depthhandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( id );
  if ( m_mapSymbols.end() == iter ) {
//    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, NewCSymbol( sSymbol ) ) );
//    iter = m_mapSymbols.find( sSymbol );
    assert( 1 == 0 );
  }
  if ( iter->second->AddDepthHandler( handler ) ) {
    StartDepthWatch( iter->second );
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::RemoveDepthHandler(const symbol_id_t& id, typename S::depthhandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( id );
  if ( m_mapSymbols.end() == iter ) {
//    m_mapSymbols.insert( std::pair<string, S*>( sSymbol, new S( sSymbol ) ) );
    assert( 1 == 0 );
  }
  else {
    if ( iter->second->RemoveDepthHandler( handler ) ) {
      StopDepthWatch( iter->second );
    }
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::AddGreekHandler(const symbol_id_t& id, typename S::greekhandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( id );
  if ( m_mapSymbols.end() == iter ) {
    assert( 1 == 0 );
  }
  iter->second->AddGreekHandler( handler );
}

template <typename P, typename S>
void CProviderInterface<P,S>::RemoveGreekHandler(const symbol_id_t& id, typename S::greekhandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( id );
  if ( m_mapSymbols.end() == iter ) {
    assert( 1 == 0 );
  }
  iter->second->RemoveGreekHandler( handler );
}

template <typename P, typename S>
S* CProviderInterface<P,S>::GetSymbol( const symbol_id_t& id ) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( id );
  if ( m_mapSymbols.end() == iter ) {
//    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, NewCSymbol( sSymbol ) ) );
//    iter = m_mapSymbols.find( sSymbol );
    assert( 1 == 0 );
  }
  return iter->second;
}

template <typename P, typename S>
void CProviderInterface<P,S>::PreSymbolDestroy( S* pSymbol ) {
}

template <typename P, typename S>
void CProviderInterface<P,S>::PlaceOrder( COrder::pOrder_t pOrder ) {
//  pOrder->SetProviderName( m_sName );
//  this->GetSymbol( pOrder->GetInstrument()->GetSymbolName() );  // ensure we have the symbol locally registered
  COrderManager::Instance().PlaceOrder( this, pOrder );
}

template <typename P, typename S>
void CProviderInterface<P,S>::CancelOrder( COrder::pOrder_t pOrder ) {
//  pOrder->SetProviderName( m_sName );
  COrderManager::Instance().CancelOrder( pOrder->GetOrderId() );
}

