/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#pragma once

#include <boost/thread/mutex.hpp>

#include <TFIQFeed/IQFeedHistoryBulkQuery.h>

class SymbolSelection: public ou::tf::iqfeed::HistoryBulkQuery<SymbolSelection> {
  friend class ou::tf::iqfeed::HistoryBulkQuery<SymbolSelection>;
public:
  explicit SymbolSelection( size_t nBarsExpected );
  ~SymbolSelection(void);

  
protected:
private:
  size_t m_nBarsExpected;  // number of expected bars
  boost::mutex m_mutexProcessBarsScopeLock;
//  void OnHistoryIntervalData( structQueryState* pqs, ou::tf::iqfeed::HistoryStructs::structInterval* pDP );
//  void OnHistorySummaryData( structQueryState* pqs, ou::tf::iqfeed::HistoryStructs::structSummary* pDP );
  void OnHistoryRequestDone( structQueryState* pqs );
  void OnCompletion( void );
};

