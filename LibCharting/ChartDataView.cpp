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

#include "ChartDataView.h"

//
// CChartDataViewCarrier
//

CChartDataViewCarrier::CChartDataViewCarrier( void ) 
: m_nLogicalChart( 0 ), m_nActualChart( 0 ), m_pChartEntry( NULL )
{
}

CChartDataViewCarrier::CChartDataViewCarrier( size_t nChart, CChartEntryBase *pChartEntry ) 
: m_nLogicalChart( nChart ), m_nActualChart( 0 ), m_pChartEntry( pChartEntry )
{
}

CChartDataViewCarrier::CChartDataViewCarrier( const CChartDataViewCarrier &carrier ) 
: m_nLogicalChart( carrier.m_nLogicalChart ), m_nActualChart( carrier.m_nActualChart ), 
  m_pChartEntry( carrier.m_pChartEntry )
{
}

CChartDataViewCarrier::~CChartDataViewCarrier() {
  m_nLogicalChart = 0;
  m_nActualChart = 0;
  m_pChartEntry = NULL;
}


// 
// CChartDataView
//

CChartDataView::CChartDataView( const std::string &sStrategy, const std::string &sName )
: m_bClosed( false ), m_sStrategy( sStrategy ), m_sName( sName ), m_bChanged( false )
{
  m_vChartDataViewEntry.reserve( 10 );
}

CChartDataView::~CChartDataView(void) {
  assert( m_bClosed );
  m_vChartDataViewEntry.clear();
}

void CChartDataView::Add(size_t nChart, CChartEntryBase *pChartEntry) {
  CChartDataViewCarrier carrier( nChart, pChartEntry );
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
    for ( std::vector<CChartDataViewCarrier>::iterator iter2 = m_vChartDataViewEntry.begin();
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

void CChartDataView::Close() {
  assert( !m_bClosed );
  OnClosing( this );
  m_bClosed = true;
}

