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

// Merge CInstrument and CSymbol<S>?
// Create CSymbol<S> from CInstrument?
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
  CProviderInterface(void);
  ~CProviderInterface(void);

  typedef boost::shared_ptr<CProviderInterface<P,S> > pProvider_t;

  enum enumProviderId { EProviderSimulator=100, EProviderIQF, EProviderIB, EProviderGNDT, _EProviderCount };

  virtual  void Connect( void );
  Delegate<int> OnConnected;

  virtual  void Disconnect( void );
  Delegate<int> OnDisconnected;

  virtual void     AddQuoteHandler( const std::string& sSymbol, typename S::quotehandler_t handler );
  virtual void  RemoveQuoteHandler( const std::string& sSymbol, typename S::quotehandler_t handler );

  virtual void    AddOnOpenHandler( const std::string& sSymbol, typename S::tradehandler_t handler );
  virtual void RemoveOnOpenHandler( const std::string& sSymbol, typename S::tradehandler_t handler );

  virtual void     AddTradeHandler( const std::string& sSymbol, typename S::tradehandler_t handler );
  virtual void  RemoveTradeHandler( const std::string& sSymbol, typename S::tradehandler_t handler );

  virtual void     AddDepthHandler( const std::string& sSymbol, typename S::depthhandler_t handler );
  virtual void  RemoveDepthHandler( const std::string& sSymbol, typename S::depthhandler_t handler );

  Delegate<CPortfolio::UpdatePortfolioRecord_t> OnUpdatePortfolioRecord;  // need to do the Add/Remove thing

  S* GetSymbol( const string &sSymbol );

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

  typedef std::map<const std::string, S*> m_mapSymbols_t;
  typedef std::pair<const std::string, S*> pair_mapSymbols_t;
  m_mapSymbols_t m_mapSymbols;

  virtual void StartQuoteWatch( S* pSymbol ) {};
  virtual void  StopQuoteWatch( S* pSymbol ) {};

  virtual void StartTradeWatch( S* pSymbol ) {};
  virtual void  StopTradeWatch( S* pSymbol ) {};

  virtual void StartDepthWatch( S* pSymbol ) {};
  virtual void  StopDepthWatch( S* pSymbol ) {};

  virtual S *NewCSymbol( const std::string& sSymbolName ) = 0; // override needs to call AddCSymbol to add symbol to map
  S* AddCSymbol( S* pSymbol );
  virtual void PreSymbolDestroy( S* pSymbol );

  std::map<std::string, std::string> m_mapAlternateNames;  // caching map to save database lookups
  CAlternateInstrumentNames m_lutAlternateInstrumentNames;

  //COrderManager m_OrderManager;

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
  m_mapSymbols_t::iterator iter = m_mapSymbols.find( pSymbol->Name() );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( pair_mapSymbols_t( pSymbol->Name(), pSymbol ) );
    iter = m_mapSymbols.find( pSymbol->Name() );
    assert( m_mapSymbols.end() != iter );
  }
  return iter->second;
}

template <typename P, typename S>
void CProviderInterface<P,S>::AddQuoteHandler(const std::string &sSymbol, typename S::quotehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, NewCSymbol( sSymbol ) ) );
    iter = m_mapSymbols.find( sSymbol );
  }
  if ( iter->second->AddQuoteHandler( handler ) ) {
    StartQuoteWatch( iter->second );
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::RemoveQuoteHandler(const std::string &sSymbol, typename S::quotehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, new S( sSymbol ) ) );
  }
  else {
    if ( iter->second->RemoveQuoteHandler( handler ) ) {
      StopQuoteWatch( iter->second );
    }
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::AddTradeHandler(const std::string &sSymbol, typename S::tradehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, NewCSymbol( sSymbol ) ) );
    iter = m_mapSymbols.find( sSymbol );
  }
  if ( iter->second->AddTradeHandler( handler ) ) {
    StartTradeWatch( iter->second );
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::RemoveTradeHandler(const std::string &sSymbol, typename S::tradehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, new S( sSymbol ) ) );
    // should probably raise exception here as trying to remove handler from non-existtant symbol
  }
  else {
    if ( iter->second->RemoveTradeHandler( handler ) ) {
      StopTradeWatch( iter->second );
    }
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::AddOnOpenHandler(const std::string &sSymbol, typename S::tradehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, NewCSymbol( sSymbol ) ) );
    iter = m_mapSymbols.find( sSymbol );
  }
  iter->second->AddOnOpenHandler( handler );
}

template <typename P, typename S>
void CProviderInterface<P,S>::RemoveOnOpenHandler(const std::string &sSymbol, typename S::tradehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( std::pair<string, S*>( sSymbol, new S( sSymbol ) ) );
  }
  else {
    iter->second->RemoveOnOpenHandler( handler );
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::AddDepthHandler(const std::string &sSymbol, typename S::depthhandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, NewCSymbol( sSymbol ) ) );
    iter = m_mapSymbols.find( sSymbol );
  }
  if ( iter->second->AddDepthHandler( handler ) ) {
    StartDepthWatch( iter->second );
  }
}

template <typename P, typename S>
void CProviderInterface<P,S>::RemoveDepthHandler(const std::string &sSymbol, typename S::depthhandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( std::pair<string, S*>( sSymbol, new S( sSymbol ) ) );
  }
  else {
    if ( iter->second->RemoveDepthHandler( handler ) ) {
      StopDepthWatch( iter->second );
    }
  }
}

template <typename P, typename S>
S* CProviderInterface<P,S>::GetSymbol( const string &sSymbol ) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, NewCSymbol( sSymbol ) ) );
    iter = m_mapSymbols.find( sSymbol );
  }
  return iter->second;
}

template <typename P, typename S>
void CProviderInterface<P,S>::PreSymbolDestroy( S* pSymbol ) {
}

template <typename P, typename S>
void CProviderInterface<P,S>::PlaceOrder( COrder *pOrder ) {
  //throw std::runtime_error( "CProviderInterface::PlaceOrder not implemented." );
  pOrder->SetProviderName( m_sName );
}

template <typename P, typename S>
void CProviderInterface<P,S>::CancelOrder( COrder *pOrder ) {
  //throw std::runtime_error( "CProviderInterface::CancelOrder not implemented." );
  pOrder->SetProviderName( m_sName );
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