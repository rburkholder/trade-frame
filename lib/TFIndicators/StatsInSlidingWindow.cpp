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

#include "StatsInSlidingWindow.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

TradeStats::TradeStats( std::string sName, unsigned int WindowSizeSeconds, unsigned int WindowSizeCount ):
  StatsInSlidingWindow( sName, WindowSizeSeconds, WindowSizeCount  ) {
}

TradeStats::~TradeStats() {
}

Trade* TradeStats::Add(boost::posix_time::ptime dt, Trade *trade) {
  StatsInSlidingWindow::Add( dt, trade->Price() );
  SlidingWindow::Add( dt, trade );
  return trade;
}

Trade* TradeStats::Remove() {
  Trade* trade = SlidingWindow::Remove();
  // what is stored in the removal, does dt need to come from trade?
  StatsInSlidingWindow::Remove( trade->DateTime(), trade->Price() );
  return trade;
}

} // namespace tf
} // namespace ou
