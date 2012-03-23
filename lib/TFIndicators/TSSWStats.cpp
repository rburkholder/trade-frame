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

#include "TSSWStats.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

//
// Trade
//

TSSWStatsTrade::TSSWStatsTrade(CTimeSeries<CTrade>& Series, long WindowSizeSeconds, size_t WindowSizeCount ) 
: TimeSeriesSlidingWindowStats<TSSWStatsTrade, CTrade>( Series, WindowSizeSeconds, WindowSizeCount )
{
}

TSSWStatsTrade::~TSSWStatsTrade( void ) {
}

void TSSWStatsTrade::Add( const CTrade &trade ) {
  time_duration dur = trade.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Add( dif, trade.Trade() );
}

void TSSWStatsTrade::Expire( const CTrade &trade ) {
  time_duration dur = trade.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Remove( dif, trade.Trade() );
}

//
// Quote
//

TSSWStatsQuote::TSSWStatsQuote(CTimeSeries<CQuote>& Series, long WindowSizeSeconds, size_t WindowSizeCount ) 
: TimeSeriesSlidingWindowStats<TSSWStatsQuote, CQuote>( Series, WindowSizeSeconds, WindowSizeCount )
{
}

TSSWStatsQuote::~TSSWStatsQuote( void ) {
}

void TSSWStatsQuote::Add( const CQuote &quote ) {
  time_duration dur = quote.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Add( dif, quote.Bid() );
  m_stats.Add( dif, quote.Ask() );
}

void TSSWStatsQuote::Expire( const CQuote &quote ) {
  time_duration dur = quote.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Remove( dif, quote.Bid() );
  m_stats.Remove( dif, quote.Ask() );
}

//
// MidQuote
//

TSSWStatsMidQuote::TSSWStatsMidQuote(CTimeSeries<CQuote>& Series, long WindowSizeSeconds, size_t WindowSizeCount ) 
: TimeSeriesSlidingWindowStats<TSSWStatsMidQuote, CQuote>( Series, WindowSizeSeconds, WindowSizeCount )
{
}

TSSWStatsMidQuote::~TSSWStatsMidQuote( void ) {
}

void TSSWStatsMidQuote::Add( const CQuote &quote ) {
  time_duration dur = quote.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Add( dif, ( quote.Bid() + quote.Ask() ) / 2.0 );
}

void TSSWStatsMidQuote::Expire( const CQuote &quote ) {
  time_duration dur = quote.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Remove( dif, ( quote.Bid() + quote.Ask() ) / 2.0 );
}

//
// CPrice
//

TSSWStatsPrice::TSSWStatsPrice(CTimeSeries<CPrice>& Series, long WindowSizeSeconds, size_t WindowSizeCount ) 
: TimeSeriesSlidingWindowStats<TSSWStatsPrice, CPrice>( Series, WindowSizeSeconds, WindowSizeCount )
{
}

TSSWStatsPrice::~TSSWStatsPrice( void ) {
}

void TSSWStatsPrice::Add( const CPrice &price ) {
  time_duration dur = price.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Add( dif, price.Price() );
}

void TSSWStatsPrice::Expire( const CPrice &price ) {
  time_duration dur = price.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Remove( dif, price.Price() );
}

} // namespace tf
} // namespace ou
