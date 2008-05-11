#include "StdAfx.h"
#include "StatsInSlidingWindow.h"

CTradeStats::CTradeStats( string sName, unsigned int WindowSizeSeconds, unsigned int WindowSizeCount ): 
  CStatsInSlidingWindow( sName, WindowSizeSeconds, WindowSizeCount  ) {
}

CTradeStats::~CTradeStats() {
}

CTrade *CTradeStats::Add(boost::posix_time::ptime dt, CTrade *trade) {
  CStatsInSlidingWindow::Add( dt, trade->m_dblTrade );
  CSlidingWindow::Add( dt, trade );
  return trade;
}

CTrade *CTradeStats::Remove() {
  CTrade *trade = CSlidingWindow::Remove();
  // what is stored in the removal, does dt need to come from trade?
  CStatsInSlidingWindow::Remove( trade->m_dt, trade->m_dblTrade );
  return trade;
}