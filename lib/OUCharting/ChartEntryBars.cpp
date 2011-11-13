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

ChartEntryBars::ChartEntryBars(unsigned int nSize) 
: ChartEntryBaseWithTime(nSize)
{
}

ChartEntryBars::~ChartEntryBars(void) {
  m_vOpen.clear();
  m_vHigh.clear();
  m_vLow.clear();
  m_vClose.clear();
}

void ChartEntryBars::Reserve( unsigned int nSize ) {
  ChartEntryBaseWithTime::Reserve( nSize );
  m_vOpen.reserve( nSize );
  m_vHigh.reserve( nSize );
  m_vLow.reserve( nSize );
  m_vClose.reserve( nSize );
}

void ChartEntryBars::AddBar(const ou::tf::CBar &bar) {/*
  if ( m_vOpen.capacity() == m_vOpen.size() ) {
    int sz = m_vOpen.size() + ( m_vOpen.size() / 5 ); // expand by 20%
    //CChartEntryBaseWithTime::Reserve( sz );
    m_vOpen.reserve( sz ); 
    m_vHigh.reserve( sz ); 
    m_vLow.reserve( sz ); 
    m_vClose.reserve( sz ); 
  }
  */
  ChartEntryBaseWithTime::Add( bar.DateTime() );
  m_vOpen.push_back( bar.Open() );
  m_vHigh.push_back( bar.High() );
  m_vLow.push_back( bar.Low() );
  m_vClose.push_back( bar.Close() );
}

void ChartEntryBars::AddDataToChart(XYChart *pXY, structChartAttributes *pAttributes) const {
  if ( 0 != this->m_vDateTime.size() ) {
    CandleStickLayer *candle = pXY->addCandleStickLayer( 
      this->GetHigh(),
      this->GetLow(),
      this->GetOpen(),
      this->GetClose(),
      0x00ff00, 0xff0000
      );
    //candle->setDataGap( 0 );
    DoubleArray daXData = ChartEntryBaseWithTime::GetDateTimes();
    candle->setXData( daXData );
    pAttributes->dblXMin = daXData[0];
    pAttributes->dblXMax = daXData[ daXData.len - 1 ];
  }
}

} // namespace ou
