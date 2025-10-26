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

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include "ChartEntryBars.h"

namespace ou { // One Unified

//
// CChartEntryBars, volume portion of bar is in ChartEntryVolume.h
//

ChartEntryBars::ChartEntryBars()
: ChartEntryTime()
{
}

//ChartEntryBars::ChartEntryBars(size_type nSize)
//: ChartEntryBaseWithTime(nSize)
//{
//}

ChartEntryBars::~ChartEntryBars() {
  Clear();
}

void ChartEntryBars::Reserve( size_type nSize ) {
  ChartEntryTime::Reserve( nSize );
  m_vOpen.reserve( nSize );
  m_vHigh.reserve( nSize );
  m_vLow.reserve( nSize );
  m_vClose.reserve( nSize );
}


void ChartEntryBars::AppendBar( const ou::tf::Bar &bar ) {
//  if ( m_bUseThreadSafety ) {
//    while ( !m_lfBar.push( bar ) ) {};
//  }
//  else {
//    AppendBarPrivate( bar );
//  }
  m_queueBars.Append( bar );
}

void ChartEntryBars::Pop( const ou::tf::Bar& bar ) {
  ChartEntryTime::AppendFg( bar.DateTime() );
  m_vOpen.push_back( bar.Open() );
  m_vHigh.push_back( bar.High() );
  m_vLow.push_back( bar.Low() );
  m_vClose.push_back( bar.Close() );
}

bool ChartEntryBars::AddEntryToChart( XYChart *pXY, structChartAttributes& attributes ) {

  bool bAdded( false );

  //ou::tf::Bar bar;
  //while ( m_lfBar.pop( bar ) ) {
//    AppendBarPrivate( bar );
//  }

  ClearQueue();

  if ( 0 != ChartEntryTime::Size() ) {
    DoubleArray daXData = ChartEntryTime::GetDateTimes();
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
      candle->setLineWidth( 3 );

      candle->setXData( daXData );
      attributes.dblXMin = daXData[0];
      attributes.dblXMax = daXData[ daXData.len - 1 ];
      int count = candle->getDataSetCount();
      if ( 0 < count ) {
        DataSet* pds = candle->getDataSet(0);
        //pds->setDataColor( m_eColour, 0xff000000, 0xff000000 );
        pds->setDataName( GetName().c_str() );
      }
      bAdded = true;
    }
  }
  return bAdded;
}

void ChartEntryBars::ClearQueue() {
  namespace args = boost::phoenix::placeholders;
  m_queueBars.Sync( boost::phoenix::bind( &ChartEntryBars::Pop, this, args::arg1 ) );
  ChartEntryTime::ClearQueue();
}

void ChartEntryBars::Clear() {
  ChartEntryTime::Clear();
  //ClearQueue();
  m_vOpen.clear();
  m_vHigh.clear();
  m_vLow.clear();
  m_vClose.clear();
}

} // namespace ou
