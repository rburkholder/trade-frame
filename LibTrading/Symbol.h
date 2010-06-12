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

#include <string>

#include <LibCommon/Delegate.h>
#include <LibTimeSeries/DatedDatum.h>

// change the Add/Remove...Handlers from virtual to CRTP?
//  probably not, 

template <typename S>  // S for Symbol
class CSymbol {
public:
  CSymbol( const std::string sName );
  virtual ~CSymbol(void);

  const std::string& Name( void ) { return m_sSymbolName; };

  typedef const CQuote& quote_t;
  typedef Delegate<quote_t>::OnMessageHandler quotehandler_t;

  typedef const CTrade& trade_t;
  typedef Delegate<trade_t>::OnMessageHandler tradehandler_t;

  typedef const CMarketDepth& depth_t;
  typedef Delegate<depth_t>::OnMessageHandler depthhandler_t;

  virtual bool AddQuoteHandler( quotehandler_t );
  virtual bool RemoveQuoteHandler( quotehandler_t );
  size_t GetQuoteHandlerCount( void ) { return m_OnQuote.Size(); };

  virtual void AddOnOpenHandler( tradehandler_t );
  virtual void RemoveOnOpenHandler( tradehandler_t );
  size_t GetOpenHandlerCount( void ) { return m_OnOpen.Size(); };
  
  virtual bool AddTradeHandler( tradehandler_t ); 
  virtual bool RemoveTradeHandler( tradehandler_t );
  size_t GetTradeHandlerCount( void ) { return m_OnTrade.Size(); };

  virtual bool AddDepthHandler( depthhandler_t );
  virtual bool RemoveDepthHandler( depthhandler_t );
  size_t GetDepthHandlerCount( void ) { return m_OnDepth.Size(); };

  bool  OpenWatchNeeded( void ) { return !m_OnOpen.IsEmpty(); };
  bool QuoteWatchNeeded( void ) { return !m_OnQuote.IsEmpty(); };
  bool TradeWatchNeeded( void ) { return !m_OnTrade.IsEmpty(); };
  bool DepthWatchNeeded( void ) { return !m_OnDepth.IsEmpty(); };

protected:

  std::string m_sSymbolName;

  Delegate<trade_t> m_OnOpen;  // first value upon market opening
  Delegate<quote_t> m_OnQuote; 
  Delegate<trade_t> m_OnTrade;
  Delegate<depth_t> m_OnDepth;

private:

};

template <typename S> 
CSymbol<S>::CSymbol( const std::string sName ): 
  m_sSymbolName( sName )
{
}

template <typename S> 
CSymbol<S>::~CSymbol(void) {
}

template <typename S> 
bool CSymbol<S>::AddQuoteHandler(quotehandler_t handler) {
  Delegate<quote_t>::vsize_t size = m_OnQuote.Size();
  m_OnQuote.Add( handler );
  assert( size == ( m_OnQuote.Size() - 1 ) );
  return ( 1 == m_OnQuote.Size() );  // start watch for the symbol
}

template <typename S> 
bool CSymbol<S>::RemoveQuoteHandler(quotehandler_t handler) {
  assert( 0 < m_OnQuote.Size() );
  m_OnQuote.Remove( handler );
  return ( 0 == m_OnQuote.Size() );  // no more so stop watch
}

template <typename S> 
void CSymbol<S>::AddOnOpenHandler(tradehandler_t handler ) {
  m_OnOpen.Add( handler );
}

template <typename S> 
void CSymbol<S>::RemoveOnOpenHandler( tradehandler_t handler ) {
  m_OnOpen.Remove( handler );
}

template <typename S> 
bool CSymbol<S>::AddTradeHandler(tradehandler_t handler) {
  Delegate<trade_t>::vsize_t size = m_OnTrade.Size();
  m_OnTrade.Add( handler );
  assert( size == ( m_OnTrade.Size() - 1 ) );
  return ( 1 == m_OnTrade.Size() ); // start watch on first handler
}

template <typename S> 
bool CSymbol<S>::RemoveTradeHandler(tradehandler_t handler) {
  assert( 0 < m_OnTrade.Size() );
  m_OnTrade.Remove( handler );
  return ( 0 == m_OnTrade.Size() ); // no more so stop watch
}

template <typename S> 
bool CSymbol<S>::AddDepthHandler(depthhandler_t handler) {
  Delegate<depth_t>::vsize_t size = m_OnDepth.Size();
  m_OnDepth.Add( handler );
  assert( size == ( m_OnDepth.Size() - 1 ) );
  return ( 1 == m_OnDepth.Size() );  // when true, start watch
}

template <typename S> 
bool CSymbol<S>::RemoveDepthHandler(depthhandler_t handler) {
  assert( 0 < m_OnDepth.Size() );
  m_OnDepth.Remove( handler );
  return ( 0 == m_OnDepth.Size() );  // when true, stop watch
}



