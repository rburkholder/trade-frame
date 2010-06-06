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

#include "StatsInSlidingWindow.h"

CTradeStats::CTradeStats( std::string sName, unsigned int WindowSizeSeconds, unsigned int WindowSizeCount ): 
  CStatsInSlidingWindow( sName, WindowSizeSeconds, WindowSizeCount  ) {
}

CTradeStats::~CTradeStats() {
}

CTrade *CTradeStats::Add(boost::posix_time::ptime dt, CTrade *trade) {
  CStatsInSlidingWindow::Add( dt, trade->Trade() );
  CSlidingWindow::Add( dt, trade );
  return trade;
}

CTrade *CTradeStats::Remove() {
  CTrade *trade = CSlidingWindow::Remove();
  // what is stored in the removal, does dt need to come from trade?
  CStatsInSlidingWindow::Remove( trade->DateTime(), trade->Trade() );
  return trade;
}