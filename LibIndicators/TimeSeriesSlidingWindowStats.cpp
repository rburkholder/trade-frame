#include "StdAfx.h"

#include "TimeSeriesSlidingWindowStats.h"

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
  time_duration dur = trade.m_dt - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Add( dif, trade.m_dblTrade );
}

void CTimeSeriesSlidingWindowStatsTrade::Expire( const CTrade &trade ) {
  time_duration dur = trade.m_dt - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Remove( dif, trade.m_dblTrade );
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
  time_duration dur = quote.m_dt - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Add( dif, quote.m_dblBid );
  m_stats.Add( dif, quote.m_dblAsk );
}

void CTimeSeriesSlidingWindowStatsQuote::Expire( const CQuote &quote ) {
  time_duration dur = quote.m_dt - m_dtZero;
  double dif = (double) dur.total_seconds();
  m_stats.Remove( dif, quote.m_dblBid );
  m_stats.Remove( dif, quote.m_dblAsk );
}

