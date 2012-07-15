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

#include "ScanHistory.h"

#include <TFIndicators/Pivots.h>

ScanHistory::ScanHistory( void ): m_barrier( 2 )
{
  m_vExchanges.push_back( "NYSE" );
  m_vExchanges.push_back( "NYSE_AMEX" );
  m_vExchanges.push_back( "NASDAQ" );
}

ScanHistory::~ScanHistory(void) {
}

void ScanHistory::Run( void ) {
  m_file.OpenIQFSymbols( "symbols.db4" );
  m_cntTotal = 0;
  for ( std::vector<std::string>::const_iterator iter = m_vExchanges.begin(); m_vExchanges.end() != iter; iter++ ) {
    m_cntSub = 0;
    m_file.SetSearchExchange( (*iter).c_str() );
    ou::tf::structSymbolRecord* p = m_file.RetrieveSymbolRecordByExchange( DB_SET );
    while ( 0 != p ) {
      Process( *p );
      p = m_file.RetrieveSymbolRecordByExchange( DB_NEXT_DUP );
    }
    std::cout << "sub cnt=" << m_cntSub << std::endl;
  }
  m_file.CloseIQFSymbols();
  std::cout << "tot cnt=" << m_cntTotal << std::endl;
  std::sort( m_vSymbolNames.begin(), m_vSymbolNames.end() );

  SetSymbols( m_vSymbolNames.begin(), m_vSymbolNames.end() );
  this->SetMaxSimultaneousQueries( 15 );
  DailyBars( m_nRequestedBars );

  // need to wait here for async history request to complete
  m_barrier.wait();
}

void ScanHistory::Process( const ou::tf::structSymbolRecord& sym ) {
  const ou::tf::structSymbolRecord::bitsSymbolClassifier_t& sc( sym.GetSymbolClassifier() );
  if ( sc.test( ou::tf::structSymbolRecord::Equity ) 
    && sc.test( ou::tf::structSymbolRecord::HasOptions ) 
    && ( !sc.test( ou::tf::structSymbolRecord::Mutual ) ) 
    ) {
    std::string s( sym.GetSymbol() );
    std::cout << sym.GetExchange() << ", " << s << ", " << sym.GetDescription() << std::endl;
    m_vSymbolNames.push_back( s );
    structSymbolInfo info;
    m_mapSymbolInfo[ s ] = info;
    m_cntSub++;
    m_cntTotal++;
  }
}

void ScanHistory::OnHistorySummaryData( structQueryState* pqs, IQFeedHistoryStructs::structSummary* pDP ) {
  bool& bPass( m_mapSymbolInfo[ pqs->bars->sSymbol ].bPass );
  if ( 1000000 > pDP->PeriodVolume ) bPass = false;
  if ( 35 > pDP->Close ) bPass = false;
  if ( 120 < pDP->Close ) bPass = false;
}

void ScanHistory::OnHistoryRequestDone( structQueryState* pqs ) {
  std::cout << pqs->bars->sSymbol << ": " << pqs->bars->bars.Size() << std::endl;
  structSymbolInfo& info( m_mapSymbolInfo[ pqs->bars->sSymbol ] );
  if ( info.bPass ) {
    if ( m_nRequestedBars == pqs->bars->bars.Size() ) {
      // need to check for time as well
      // may decide not to trade a day with a large pivot
      unsigned int cnt = 0;
      ou::tf::Bars::const_iterator iter = pqs->bars->bars.begin();
      for ( int ix = 0; ix < ( m_nRequestedBars - 1 ); ix++ ) { // n - 1 bars, extra bar is pivot calc
        ou::tf::CPivotSet pivot( "", *iter );
        iter++;
        double pv = pivot.GetPivotValue( ou::tf::CPivotSet::PV );
        if ( iter->Open() > pv ) if ( iter->Low() <= pv ) info.cntPV++;
        if ( iter->Open() < pv ) if ( iter->High() >= pv ) info.cntPV++;
        if ( iter->High() >= pivot.GetPivotValue( ou::tf::CPivotSet::R1 ) ) info.cntR1++;
        if ( iter->Low() <= pivot.GetPivotValue( ou::tf::CPivotSet::S1 ) ) info.cntS1++;
      }
      //iter++;
      ou::tf::CPivotSet pivot( "", *iter );  // generate pivot for final bar
      info.S3 = pivot.GetPivotValue( ou::tf::CPivotSet::S3 );
      info.S2 = pivot.GetPivotValue( ou::tf::CPivotSet::S2 );
      info.S1 = pivot.GetPivotValue( ou::tf::CPivotSet::S1 );
      info.PV = pivot.GetPivotValue( ou::tf::CPivotSet::PV );
      info.R1 = pivot.GetPivotValue( ou::tf::CPivotSet::R1 );
      info.R2 = pivot.GetPivotValue( ou::tf::CPivotSet::R2 );
      info.R3 = pivot.GetPivotValue( ou::tf::CPivotSet::R3 );
      info.dblClose = iter->Close();
      unsigned int metric = info.cntR1 + info.cntS1;
      if ( m_nRequestedBars <= metric ) {
        m_mapPivotCrossings.insert( std::pair<unsigned int, std::string>( metric, pqs->bars->sSymbol ) );
      }
    }
  }
}

void ScanHistory::OnCompletion( void ) {
  for ( m_mapPivotCrossings_t::iterator iter = m_mapPivotCrossings.begin(); m_mapPivotCrossings.end() != iter; iter++ ) {
    std::cout << iter->second << " " << iter->first << std::endl;
  }
  std::cout << "#symbols=" << m_mapPivotCrossings.size() << std::endl;
  m_barrier.wait();
}