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

#include "StatsInSlidingWindow.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

CTradeStats::CTradeStats( std::string sName, unsigned int WindowSizeSeconds, unsigned int WindowSizeCount ): 
  CStatsInSlidingWindow( sName, WindowSizeSeconds, WindowSizeCount  ) {
}

CTradeStats::~CTradeStats() {
}

Trade* CTradeStats::Add(boost::posix_time::ptime dt, Trade *trade) {
  CStatsInSlidingWindow::Add( dt, trade->Price() );
  CSlidingWindow::Add( dt, trade );
  return trade;
}

Trade* CTradeStats::Remove() {
  Trade* trade = CSlidingWindow::Remove();
  // what is stored in the removal, does dt need to come from trade?
  CStatsInSlidingWindow::Remove( trade->DateTime(), trade->Price() );
  return trade;
}

} // namespace tf
} // namespace ou
