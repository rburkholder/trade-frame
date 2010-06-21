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
#include "AlternateInstrumentNames.h"
#include "OrderManager.h"
#include "Portfolio.h"

// need to include a check that callbacks and virtuals are in the correct thread
// in IB, processMsg may be best place to have in cross thread management, if it isn't already

// Store CInstrument in CSymbol<S>?  <= use this with smart_ptr on CInstrument.

/*
Discussion of calling sequence for open, quote, trade, depth handlers:
* client application calls Provider to add a handler
* CProviderInterface maintains list of symbols, 
   and will use the pure virtual override to create a new one when necessary 
*/

class CProviderInterfaceBase {
public:

  CProviderInterfaceBase( void ) {};
  virtual ~CProviderInterfaceBase( void ) {};

  virtual void PlaceOrder( COrder *pOrder ) = 0;
  virtual void CancelOrder( COrder *pOrder ) = 0;
protected:
private:
};

template <typename P, typename S>  // p = provider, S = symbol
class CProviderInterface: public CProviderInterfaceBase {
public:

  typedef boost::shared_ptr<CProviderInterface<P,S> > pProvider_t;
  typedef typename S::pInstrument_t pInstrument_t;
  typedef typename S::symbol_id_t symbol_id_t;

  enum enumProviderId { EProviderSimulator=100, EProviderIQF, EProviderIB, EProviderGNDT, _EProviderCount };

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

  Delegate<CPortfolio::UpdatePortfolioRecord_t> OnUpdatePortfolioRecord;  // need to do the Add/Remove thing

  S* GetSymbol( const symbol_id_t& );

  const std::string& Name( void ) { return m_sName; };
  unsigned short ID( void ) { assert( 0 != m_nID ); return m_nID; };
  bool Connected( void ) { return m_bConnected; };

  void PlaceOrder( COrder *pOrder );
  void CancelOrder( COrder *pOrder );

  void SetAlternateInstrumentName( const std::string& OriginalInstrumentName, const std::string& AlternateIntrumentName );
  void GetAlternateInstrumentName( const std::string& OriginalInstrumentName, std::string* pAlternateInstrumentName );

protected:

  std::string m_sName;  // name of provider
  unsigned short m_nID;
  bool m_bConnected;

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

  // need to redo this and place into CInstrument?  or CSymbol?
  std::map<std::string, std::string> m_mapAlternateNames;  // caching map to save database lookups
  CAlternateInstrumentNames m_lutAlternateInstrumentNames;

  COrderManager m_OrderManager;

private:
};

template <typename P, typename S>
CProviderInterface<P,S>::CProviderInterface(void) 
: m_bConnected( false ), m_nID( 0 )
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
void CProviderInterface<P,S>::PlaceOrder( COrder *pOrder ) {
  pOrder->SetProviderName( m_sName );
//  this->GetSymbol( pOrder->GetInstrument()->GetSymbolName() );  // ensure we have the symbol locally registered
  COrderManager::Instance().PlaceOrder( this, pOrder );
}

template <typename P, typename S>
void CProviderInterface<P,S>::CancelOrder( COrder *pOrder ) {
  pOrder->SetProviderName( m_sName );
  COrderManager::Instance().CancelOrder( pOrder->GetOrderId() );
}

template <typename P, typename S>
void CProviderInterface<P,S>::SetAlternateInstrumentName(const std::string &OriginalInstrumentName, const std::string &AlternateIntrumentName) {
  m_lutAlternateInstrumentNames.Save( m_sName, OriginalInstrumentName, AlternateIntrumentName );
  std::map<std::string, std::string>::iterator iter 
    = m_mapAlternateNames.find( OriginalInstrumentName );
  if ( m_mapAlternateNames.end() == iter ) {
    m_mapAlternateNames.insert( std::pair<std::string, std::string>( OriginalInstrumentName, AlternateIntrumentName ) );
  }
  else m_mapAlternateNames[ OriginalInstrumentName ] = AlternateIntrumentName;
}

template <typename P, typename S>
void CProviderInterface<P,S>::GetAlternateInstrumentName(const std::string &OriginalInstrumentName, std::string *pAlternateInstrumentName) {
  std::map<std::string, std::string>::iterator iter 
    = m_mapAlternateNames.find( OriginalInstrumentName );
  if ( m_mapAlternateNames.end() != iter ) {
    pAlternateInstrumentName->assign( iter->second );
  }
  else {
    try {
      m_lutAlternateInstrumentNames.Get( m_sName, OriginalInstrumentName, pAlternateInstrumentName );
      m_mapAlternateNames.insert( std::pair<std::string, std::string>( OriginalInstrumentName, *pAlternateInstrumentName ) );
    }
    catch ( std::out_of_range e ) {
      m_mapAlternateNames.insert( std::pair<std::string, std::string>( OriginalInstrumentName, OriginalInstrumentName ) );
      pAlternateInstrumentName->assign( OriginalInstrumentName );
    }
    catch ( std::exception e ) {
      std::cout << "CProviderInterface::GetAlternateInstrumentName has error: " << e.what() << std::endl;
    }
  }
}