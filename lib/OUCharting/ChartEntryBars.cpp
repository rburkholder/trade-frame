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

#include "ChartEntryBars.h"

namespace ou { // One Unified

//
// CChartEntryBars, volume portion of bar is in ChartEntryVolume.h
//

ChartEntryBars::ChartEntryBars(void) 
: ChartEntryBaseWithTime()
{
}

ChartEntryBars::ChartEntryBars(size_type nSize) 
: ChartEntryBaseWithTime(nSize)
{
}

ChartEntryBars::~ChartEntryBars(void) {
  m_vOpen.clear();
  m_vHigh.clear();
  m_vLow.clear();
  m_vClose.clear();
}

void ChartEntryBars::Reserve( size_type nSize ) {
  ChartEntryBaseWithTime::Reserve( nSize );
  m_vOpen.reserve( nSize );
  m_vHigh.reserve( nSize );
  m_vLow.reserve( nSize );
  m_vClose.reserve( nSize );
}

void ChartEntryBars::AppendBarPrivate( const ou::tf::Bar& bar ) {
  ChartEntryBaseWithTime::Append( bar.DateTime() );
  m_vOpen.push_back( bar.Open() );
  m_vHigh.push_back( bar.High() );
  m_vLow.push_back( bar.Low() );
  m_vClose.push_back( bar.Close() );
}

void ChartEntryBars::AppendBar(const ou::tf::Bar &bar) {
  if ( m_bUseThreadSafety ) {
    while ( !m_lfBar.push( bar ) ) {};
  }
  else {
    AppendBarPrivate( bar );
  }
  
}

bool ChartEntryBars::AddEntryToChart(XYChart *pXY, structChartAttributes *pAttributes) {

  bool bAdded( false );

  ou::tf::Bar bar;
  while ( m_lfBar.pop( bar ) ) {
    AppendBarPrivate( bar );
  }

  if ( 0 != this->m_vDateTime.size() ) {
    DoubleArray daXData = ChartEntryBaseWithTime::GetDateTimes();
    // this should be replicated to the other Entry Types.
    if ( 0 != daXData.len ) {
      CandleStickLayer *candle = pXY->addCandleStickLayer( 
        this->GetHigh(),
        this->GetLow(),
        this->GetOpen(),
        this->GetClose(),
  //      0x0000ff00, 0x00ff0000, 0xff000000
        0x0000ff00, 0x00ff0000, 0xFFFF0001
        );
      //candle->setDataGap( 0 );
    
      candle->setXData( daXData );
      pAttributes->dblXMin = daXData[0];
      pAttributes->dblXMax = daXData[ daXData.len - 1 ];
      bAdded = true;
    }
  }
  return bAdded;
}

void ChartEntryBars::Clear( void ) {
  m_vOpen.clear();
  m_vHigh.clear();
  m_vLow.clear();
  m_vClose.clear();
  ChartEntryBaseWithTime::Clear();
}

} // namespace ou
