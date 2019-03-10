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

ChartDataViewCarrier::ChartDataViewCarrier( size_t nChart, ChartEntryBase* pChartEntry )
: m_nLogicalChart( nChart ), m_nActualChart( 0 ), m_pChartEntry( pChartEntry )
{
}

ChartDataViewCarrier::ChartDataViewCarrier( const ChartDataViewCarrier& rhs )
: m_nLogicalChart( rhs.m_nLogicalChart ),
  m_nActualChart( rhs.m_nActualChart ),
  m_pChartEntry( rhs.m_pChartEntry )
{
}

ChartDataViewCarrier::~ChartDataViewCarrier() {
  m_nLogicalChart = 0;
  m_nActualChart = 0;
  m_pChartEntry = 0;
}

} // local

//
// CChartDataView
//

ChartDataView::ChartDataView( void )
  : m_bChanged( false ), m_bThreadSafe( false ),
    m_dtViewPortBegin( boost::posix_time::not_a_date_time ),
    m_dtViewPortEnd( boost::posix_time::not_a_date_time )
{
}

ChartDataView::~ChartDataView( void ) {
//  assert( m_bClosed );
  m_vChartDataViewCarrier.clear();
  m_mapCntChartIndexes.clear();
}

void ChartDataView::SetChanged(void) {
  m_bChanged = true;
}

bool ChartDataView::GetChanged(void) {
  bool b = m_bChanged;
  if ( b ) {
    m_bChanged = false;
  }
  return b;
}


void ChartDataView::SetThreadSafe( bool bThreadSafe ) {
  m_bThreadSafe = bThreadSafe;
  for ( vChartDataViewCarrier_t::iterator iter = m_vChartDataViewCarrier.begin(); m_vChartDataViewCarrier.end() != iter; ++iter ) {
    iter->GetChartEntry()->SetThreadSafe( bThreadSafe );
  }
}

void ChartDataView::Add(size_t nChart, ChartEntryBase* pEntry ) {
  pEntry->SetThreadSafe( m_bThreadSafe );
  local::ChartDataViewCarrier carrier( nChart, pEntry );
  m_vChartDataViewCarrier.push_back( carrier );
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
    for ( vChartDataViewCarrier_t::iterator iter2 = m_vChartDataViewCarrier.begin();
      m_vChartDataViewCarrier.end() != iter2; ++iter2 ) {
        iter3 = m_mapCntChartIndexes.find( (*iter2).GetLogicalChartId() );
        (*iter2).SetActualChartId( iter3->second.ixActualChartId );
    }
  }
  else {
    ++(iter1->second.nCharts);
    m_vChartDataViewCarrier.back().SetActualChartId( iter1->second.ixActualChartId );
  }

}

void ChartDataView::Clear( void ) {
  m_mapCntChartIndexes.clear();
  m_vChartDataViewCarrier.clear();
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
  for ( vChartDataViewCarrier_t::iterator iter = m_vChartDataViewCarrier.begin(); m_vChartDataViewCarrier.end() != iter; ++iter ) {
    ChartEntryTime* p( dynamic_cast<ChartEntryTime*>( iter->GetChartEntry() ) );
    if ( 0 != p ) {
      p->SetViewPort( dtBegin, dtEnd );
    }
  }
}

} // namespace ou
