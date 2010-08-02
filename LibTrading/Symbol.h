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

#include "Instrument.h"

// change the Add/Remove...Handlers from virtual to CRTP?
//  probably not, 

template <typename S, typename ID>  // S for Provider specific Symbol type, ID for map look up type
class CSymbol {
public:

  typedef typename CInstrument::pInstrument_t pInstrument_t;
  typedef typename CInstrument::pInstrument_cref pInstrument_cref;
  typedef typename ID symbol_id_t;

  CSymbol( symbol_id_t id, pInstrument_t pInstrument ); // class should only be constructed with valid instrument
  virtual ~CSymbol(void);

  pInstrument_t GetInstrument( void ) { return m_pInstrument; };
  const symbol_id_t& GetId( void ) { return m_id; };  // used for mapping and indexing

  typedef const CQuote& quote_t;
  typedef Delegate<quote_t>::OnMessageHandler quotehandler_t;

  typedef const CTrade& trade_t;
  typedef Delegate<trade_t>::OnMessageHandler tradehandler_t;

  typedef const CMarketDepth& depth_t;
  typedef Delegate<depth_t>::OnMessageHandler depthhandler_t;

  typedef const CGreek& greek_t;
  typedef Delegate<greek_t>::OnMessageHandler greekhandler_t;

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

  virtual void AddGreekHandler( greekhandler_t );
  virtual void RemoveGreekHandler( greekhandler_t );
  size_t GetGreekHandlerCount( void ) { return m_OnGreek.Size(); };

  bool  OpenWatchNeeded( void ) { return !m_OnOpen.IsEmpty(); };
  bool QuoteWatchNeeded( void ) { return !m_OnQuote.IsEmpty(); };
  bool TradeWatchNeeded( void ) { return !m_OnTrade.IsEmpty(); };
  bool DepthWatchNeeded( void ) { return !m_OnDepth.IsEmpty(); };
  bool GreekWatchNeeded( void ) { return !m_OnGreek.Isempty(); };

protected:

  Delegate<trade_t> m_OnOpen;  // first value upon market opening
  Delegate<quote_t> m_OnQuote; 
  Delegate<trade_t> m_OnTrade;
  Delegate<depth_t> m_OnDepth;
  Delegate<greek_t> m_OnGreek;

private:

  symbol_id_t m_id;
  CInstrument::pInstrument_t m_pInstrument;  // composition rather than inheritance as same instrument refers to different market data and order providers

};

template <typename S, typename ID>
CSymbol<S,ID>::CSymbol( symbol_id_t id, pInstrument_t pInstrument )
: 
  m_id( id ),
  m_pInstrument( pInstrument )
{
}

template <typename S, typename ID>
CSymbol<S,ID>::~CSymbol(void) {
}

template <typename S, typename ID>
bool CSymbol<S,ID>::AddQuoteHandler(quotehandler_t handler) {
  Delegate<quote_t>::vsize_t size = m_OnQuote.Size();
  m_OnQuote.Add( handler );
  assert( size == ( m_OnQuote.Size() - 1 ) );
  return ( 1 == m_OnQuote.Size() );  // start watch for the symbol
}

template <typename S, typename ID>
bool CSymbol<S,ID>::RemoveQuoteHandler(quotehandler_t handler) {
  assert( 0 < m_OnQuote.Size() );
  m_OnQuote.Remove( handler );
  return ( 0 == m_OnQuote.Size() );  // no more so stop watch
}

template <typename S, typename ID>
void CSymbol<S,ID>::AddOnOpenHandler(tradehandler_t handler ) {
  m_OnOpen.Add( handler );
}

template <typename S, typename ID>
void CSymbol<S,ID>::RemoveOnOpenHandler( tradehandler_t handler ) {
  m_OnOpen.Remove( handler );
}

template <typename S, typename ID>
bool CSymbol<S,ID>::AddTradeHandler(tradehandler_t handler) {
  Delegate<trade_t>::vsize_t size = m_OnTrade.Size();
  m_OnTrade.Add( handler );
  assert( size == ( m_OnTrade.Size() - 1 ) );
  return ( 1 == m_OnTrade.Size() ); // start watch on first handler
}

template <typename S, typename ID>
bool CSymbol<S,ID>::RemoveTradeHandler(tradehandler_t handler) {
  assert( 0 < m_OnTrade.Size() );
  m_OnTrade.Remove( handler );
  return ( 0 == m_OnTrade.Size() ); // no more so stop watch
}

template <typename S, typename ID>
bool CSymbol<S,ID>::AddDepthHandler(depthhandler_t handler) {
  Delegate<depth_t>::vsize_t size = m_OnDepth.Size();
  m_OnDepth.Add( handler );
  assert( size == ( m_OnDepth.Size() - 1 ) );
  return ( 1 == m_OnDepth.Size() );  // when true, start watch
}

template <typename S, typename ID>
bool CSymbol<S,ID>::RemoveDepthHandler(depthhandler_t handler) {
  assert( 0 < m_OnDepth.Size() );
  m_OnDepth.Remove( handler );
  return ( 0 == m_OnDepth.Size() );  // when true, stop watch
}

template <typename S, typename ID>
void CSymbol<S,ID>::AddGreekHandler ( greekhandler_t handler ) {
  m_OnGreek.Add( handler );
}

template <typename S, typename ID>
void CSymbol<S,ID>::RemoveGreekHandler( greekhandler_t handler ) {
  m_OnGreek.Remove( handler );
}



