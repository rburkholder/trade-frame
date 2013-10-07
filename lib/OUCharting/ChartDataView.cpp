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

//#include "StdAfx.h"

#include "ChartDataView.h"

namespace ou { // One Unified
namespace local {

//
// CChartDataViewCarrier
//

//ChartDataViewCarrier::ChartDataViewCarrier( void ) 
//: m_nLogicalChart( 0 ), m_nActualChart( 0 ), m_pChartEntry( 0 )
//{
//}

ChartDataViewCarrier::ChartDataViewCarrier( size_t nChart, ChartEntryBase* pChartEntry ) 
: m_nLogicalChart( nChart ), m_nActualChart( 0 ), m_pChartEntry( pChartEntry )
{
}

//ChartDataViewCarrier::ChartDataViewCarrier( ChartDataViewCarrier &carrier ) 
//: m_nLogicalChart( carrier.m_nLogicalChart ), m_nActualChart( carrier.m_nActualChart ), 
//  m_pChartEntry( carrier.m_pChartEntry )
//{
//}

ChartDataViewCarrier::~ChartDataViewCarrier() {
  m_nLogicalChart = 0;
  m_nActualChart = 0;
  m_pChartEntry = 0;
}

} // local


// 
// CChartDataView
//

ChartDataView::ChartDataView( const std::string &sStrategy, const std::string &sName )
: m_sStrategy( sStrategy ), m_sName( sName ), m_bChanged( false ) //, m_bClosed( false )
{
  m_vChartDataViewEntry.reserve( 10 );
}

ChartDataView::~ChartDataView(void) {
//  assert( m_bClosed );
  m_vChartDataViewEntry.clear();
}

void ChartDataView::Add(size_t nChart, ChartEntryBase* pEntry ) {
  local::ChartDataViewCarrier carrier( nChart, pEntry );
  m_vChartDataViewEntry.push_back( carrier );
  mapCntChartIndexes_t::iterator iter1, iter3;
  iter1 = m_mapCntChartIndexes.find( nChart );
  if ( m_mapCntChartIndexes.end() == iter1 ) { 
    // need to add new mapping
    structChartMapping mapping;
    mapping.nCharts = 1;
    m_mapCntChartIndexes[ nChart ] = mapping;
    // need to recalc ActualChartId's in our map
    size_t ix = 0;
    for ( iter3 = m_mapCntChartIndexes.begin(); m_mapCntChartIndexes.end() != iter3; ++iter3 ) {
      iter3->second.ixActualChartId = ix++;
    }
    // need to update actualchartid's in all the carriers.
    for ( vChartDataViewEntry_t::iterator iter2 = m_vChartDataViewEntry.begin();
      m_vChartDataViewEntry.end() != iter2; ++iter2 ) {
        iter3 = m_mapCntChartIndexes.find( (*iter2).GetLogicalChartId() );
        (*iter2).SetActualChartId( iter3->second.ixActualChartId );
    }
  }
  else {
    ++(iter1->second.nCharts);
    m_vChartDataViewEntry.back().SetActualChartId( iter1->second.ixActualChartId );
  }
  
}

void ChartDataView::Clear( void ) {
  m_mapCntChartIndexes.clear();
  m_vChartDataViewEntry.clear();
}

//void ChartDataView::Close() {
//  assert( !m_bClosed );
//  OnClosing( this );
//  m_bClosed = true;
//}

void ChartDataView::SetViewPort( boost::posix_time::ptime dtBegin, boost::posix_time::ptime dtEnd ) {
  m_dtViewPortBegin = dtBegin;
  m_dtViewPortEnd = dtEnd;
  // need to change DataArrays in each entry
  for ( vChartDataViewEntry_t::iterator iter = m_vChartDataViewEntry.begin(); m_vChartDataViewEntry.end() != iter; ++iter ) {
    dynamic_cast<ChartEntryBaseWithTime*>( iter->GetChartEntry() )->SetViewPort( dtBegin, dtEnd );
  }
}

} // namespace ou
