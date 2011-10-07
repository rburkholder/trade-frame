/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include "Symbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

CSymbolBase::CSymbolBase( pInstrument_t pInstrument )
: 
  m_id( pInstrument->GetInstrumentName() ),  // use the generic name, but in provider, use provider specific name
  m_pInstrument( pInstrument )
  {
  // need to set using the provider specific from CInstrument, but need provider id to do so. .. see other constructor
}

CSymbolBase::CSymbolBase( pInstrument_t pInstrument, const std::string& sName )
: 
  m_id( sName ),  // use the generic name, but in provider, use provider specific name
  m_pInstrument( pInstrument )
  {
}

CSymbolBase::~CSymbolBase(void) {
}

void CSymbolBase::AddOnOpenHandler(tradehandler_t handler ) {
  m_OnOpen.Add( handler );
}

void CSymbolBase::RemoveOnOpenHandler( tradehandler_t handler ) {
  m_OnOpen.Remove( handler );
}

bool CSymbolBase::AddQuoteHandler(quotehandler_t handler) {
  ou::Delegate<quote_t>::vsize_t size = m_OnQuote.Size();
  m_OnQuote.Add( handler );
  assert( size == ( m_OnQuote.Size() - 1 ) );
  return ( 1 == m_OnQuote.Size() );  // start watch for the symbol
}

bool CSymbolBase::RemoveQuoteHandler(quotehandler_t handler) {
  assert( 0 < m_OnQuote.Size() );
  m_OnQuote.Remove( handler );
  return ( 0 == m_OnQuote.Size() );  // no more so stop watch
}

bool CSymbolBase::AddTradeHandler(tradehandler_t handler) {
  ou::Delegate<trade_t>::vsize_t size = m_OnTrade.Size();
  m_OnTrade.Add( handler );
  assert( size == ( m_OnTrade.Size() - 1 ) );
  return ( 1 == m_OnTrade.Size() ); // start watch on first handler
}

bool CSymbolBase::RemoveTradeHandler(tradehandler_t handler) {
  assert( 0 < m_OnTrade.Size() );
  m_OnTrade.Remove( handler );
  return ( 0 == m_OnTrade.Size() ); // no more so stop watch
}

bool CSymbolBase::AddDepthHandler(depthhandler_t handler) {
  ou::Delegate<depth_t>::vsize_t size = m_OnDepth.Size();
  m_OnDepth.Add( handler );
  assert( size == ( m_OnDepth.Size() - 1 ) );
  return ( 1 == m_OnDepth.Size() );  // when true, start watch
}

bool CSymbolBase::RemoveDepthHandler(depthhandler_t handler) {
  assert( 0 < m_OnDepth.Size() );
  m_OnDepth.Remove( handler );
  return ( 0 == m_OnDepth.Size() );  // when true, stop watch
}

bool CSymbolBase::AddGreekHandler ( greekhandler_t handler ) {
  ou::Delegate<greek_t>::vsize_t size = m_OnGreek.Size();
  m_OnGreek.Add( handler );
  assert( size == ( m_OnGreek.Size() - 1 ) );
  return ( 1 == m_OnGreek.Size() );  // when true, start watch
}

bool CSymbolBase::RemoveGreekHandler( greekhandler_t handler ) {
  assert( 0 < m_OnGreek.Size() );
  m_OnGreek.Remove( handler );
  return ( 0 == m_OnGreek.Size() );  // when true, stop watch
}

} // namespace tf
} // namespace ou


