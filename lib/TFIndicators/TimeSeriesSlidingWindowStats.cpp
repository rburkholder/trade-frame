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

#include "TimeSeriesSlidingWindowStats.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

//
// Trade
//

CTimeSeriesSlidingWindowStatsTrade::CTimeSeriesSlidingWindowStatsTrade(CTimeSeries<CTrade> *pSeries, long WindowSizeSeconds, size_t WindowSizeCount ) 
: CTimeSeriesSlidingWindowStats<CTrade>( pSeries, WindowSizeSeconds, WindowSizeCount )
{
}

CTimeSeriesSlidingWindowStatsTrade::~CTimeSeriesSlidingWindowStatsTrade( void ) {
}

void CTimeSeriesSlidingWindowStatsTrade::Add( const CTrade &trade ) {
  time_duration dur = trade.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Add( dif, trade.Trade() );
}

void CTimeSeriesSlidingWindowStatsTrade::Expire( const CTrade &trade ) {
  time_duration dur = trade.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Remove( dif, trade.Trade() );
}

//
// Quote
//

CTimeSeriesSlidingWindowStatsQuote::CTimeSeriesSlidingWindowStatsQuote(CTimeSeries<CQuote> *pSeries, long WindowSizeSeconds, size_t WindowSizeCount ) 
: CTimeSeriesSlidingWindowStats<CQuote>( pSeries, WindowSizeSeconds, WindowSizeCount )
{
}

CTimeSeriesSlidingWindowStatsQuote::~CTimeSeriesSlidingWindowStatsQuote( void ) {
}

void CTimeSeriesSlidingWindowStatsQuote::Add( const CQuote &quote ) {
  time_duration dur = quote.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Add( dif, quote.Bid() );
  m_stats.Add( dif, quote.Ask() );
}

void CTimeSeriesSlidingWindowStatsQuote::Expire( const CQuote &quote ) {
  time_duration dur = quote.DateTime() - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Remove( dif, quote.Bid() );
  m_stats.Remove( dif, quote.Ask() );
}

} // namespace tf
} // namespace ou
