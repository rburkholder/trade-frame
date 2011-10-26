/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include <vector>
#include <string>
#include <map>

#include <boost/thread/barrier.hpp>

#include <TFIQFeed/IQFeedInstrumentFile.h>
#include <TFIQFeed/IQFeedHistoryBulkQuery.h>
#include <TFTimeSeries/TimeSeries.h>

class ScanHistory;
typedef ou::tf::CIQFeedHistoryBulkQuery<ScanHistory> HistoryBulkQuery_t;

class ScanHistory:
  public HistoryBulkQuery_t {
  friend HistoryBulkQuery_t;
public:

  ScanHistory( void );
  ~ScanHistory(void);

  void Run( void );

  template<typename T, typename F> // F(T)
  void GetInfo( F );  // F is called with reference to structure of type T

protected:

  typedef HistoryBulkQuery_t::structQueryState structQueryState;

  // CRTP from CIQFeedHistoryBulkQuery
  void OnHistorySummaryData( structQueryState* pqs, IQFeedHistoryStructs::structSummary* pDP );
  void OnHistoryRequestDone( structQueryState* );
  void OnCompletion( void );  // when all queries are done

private:

  static const unsigned int m_nRequestedBars = 10;

  unsigned int m_cntSub;
  unsigned int m_cntTotal;

  ou::tf::CInstrumentFile m_file;

  struct structSymbolInfo {
    bool bPass;  // symbol passed primary selection criteria
    unsigned int cntR1, cntS1, cntPV;
    double S3, S2, S1, PV, R1, R2, R3;
    double dblClose;
    structSymbolInfo( void ): bPass( true ), cntR1( 0 ), cntS1( 0 ), cntPV( 0 ) {};
  };

  typedef std::map<std::string, structSymbolInfo> mapSymbolInfo_t;
  typedef mapSymbolInfo_t:: iterator mapSymbolInfo_iter_t;
  typedef mapSymbolInfo_t::const_iterator mapSymbolInfo_citer_t;
  mapSymbolInfo_t m_mapSymbolInfo;

  typedef std::multimap<unsigned int, std::string> m_mapPivotCrossings_t;
  m_mapPivotCrossings_t m_mapPivotCrossings;

  std::vector<std::string> m_vExchanges;
  std::vector<std::string> m_vSymbolNames;
  std::vector<std::string>::const_iterator m_iterSymbol;

  boost::barrier m_barrier;

  void Process( const ou::tf::structSymbolRecord& sym );

};

template<typename T, typename F> // F(T)
void ScanHistory::GetInfo( F f ) {
  for ( m_mapPivotCrossings_t::const_reverse_iterator iter = m_mapPivotCrossings.rbegin(); m_mapPivotCrossings.rend() != iter; iter++ ) {
    structSymbolInfo& si( m_mapSymbolInfo[ iter->second ] );
    T t;
    t.sName = iter->second;
    t.R3 = si.R3;
    t.R2 = si.R2;
    t.R1 = si.R1;
    t.PV = si.PV;
    t.S1 = si.S1;
    t.S2 = si.S2;
    t.S3 = si.S3;
    t.dblClose = si.dblClose;
    f( t );
  }
};
