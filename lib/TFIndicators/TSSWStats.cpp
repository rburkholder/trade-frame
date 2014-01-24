/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

TSSWStatsTrade::TSSWStatsTrade(TimeSeries<Trade>& Series, time_duration tdWindowWidth, size_t WindowSizeCount ) 
: TimeSeriesSlidingWindowStats<TSSWStatsTrade, Trade>( Series, tdWindowWidth, WindowSizeCount )
{
}

TSSWStatsTrade::TSSWStatsTrade( const TSSWStatsTrade& rhs )
  : TimeSeriesSlidingWindowStats<TSSWStatsTrade, Trade>( rhs )
{
}

TSSWStatsTrade::~TSSWStatsTrade( void ) {
}

void TSSWStatsTrade::Add( const Trade &trade ) {
  time_duration dur = trade.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Add( dif, trade.Price() );
}

void TSSWStatsTrade::Expire( const Trade &trade ) {
  time_duration dur = trade.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Remove( dif, trade.Price() );
}

//
// Quote
//

TSSWStatsQuote::TSSWStatsQuote(TimeSeries<Quote>& Series, time_duration tdWindowWidth, size_t WindowSizeCount ) 
: TimeSeriesSlidingWindowStats<TSSWStatsQuote, Quote>( Series, tdWindowWidth, WindowSizeCount )
{
}

TSSWStatsQuote::TSSWStatsQuote( const TSSWStatsQuote& rhs )
  : TimeSeriesSlidingWindowStats<TSSWStatsQuote, Quote>( rhs )
{
}

TSSWStatsQuote::~TSSWStatsQuote( void ) {
}

void TSSWStatsQuote::Add( const Quote &quote ) {
  time_duration dur = quote.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Add( dif, quote.Bid() );
  m_stats.Add( dif, quote.Ask() );
}

void TSSWStatsQuote::Expire( const Quote &quote ) {
  time_duration dur = quote.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Remove( dif, quote.Bid() );
  m_stats.Remove( dif, quote.Ask() );
}

//
// MidQuote
//

TSSWStatsMidQuote::TSSWStatsMidQuote(Quotes& Series, time_duration tdWindowWidth, size_t WindowSizeCount ) 
: TimeSeriesSlidingWindowStats<TSSWStatsMidQuote, Quote>( Series, tdWindowWidth, WindowSizeCount )
{
}

TSSWStatsMidQuote::TSSWStatsMidQuote( const TSSWStatsMidQuote& rhs )
  : TimeSeriesSlidingWindowStats<TSSWStatsMidQuote, Quote>( rhs )
{
}

TSSWStatsMidQuote::~TSSWStatsMidQuote( void ) {
}

void TSSWStatsMidQuote::Add( const Quote &quote ) {
  time_duration dur = quote.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Add( dif, quote.Midpoint() );
}

void TSSWStatsMidQuote::Expire( const Quote &quote ) {
  time_duration dur = quote.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Remove( dif, quote.Midpoint() );
}

//
// Price
//

TSSWStatsPrice::TSSWStatsPrice(TimeSeries<Price>& Series, time_duration tdWindowWidth, size_t WindowSizeCount ) 
: TimeSeriesSlidingWindowStats<TSSWStatsPrice, Price>( Series, tdWindowWidth, WindowSizeCount )
{
}

TSSWStatsPrice::TSSWStatsPrice( const TSSWStatsPrice& rhs )
  : TimeSeriesSlidingWindowStats<TSSWStatsPrice, Price>( rhs )
{
}

TSSWStatsPrice::~TSSWStatsPrice( void ) {
}

void TSSWStatsPrice::Add( const Price& price ) {
  time_duration dur = price.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Add( dif, price.Value() );
}

void TSSWStatsPrice::Expire( const Price& price ) {
  time_duration dur = price.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Remove( dif, price.Value() );
}

} // namespace tf
} // namespace ou
