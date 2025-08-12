/************************************************************************
 * Copyright(c) 2017, One Unified. All rights reserved.                 *
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

/*
 * File:   ChartEntryPrice.cpp
 * Author: rpb
 *
 * Created on May 6, 2017, 7:03 PM
 */

#include <functional>

#include "ChartEntryPrice.h"

namespace ou { // One Unified

ChartEntryPrice::ChartEntryPrice()
: ChartEntryTime() {
}

ChartEntryPrice::ChartEntryPrice( ChartEntryPrice&& rhs )
: ChartEntryTime( std::move( rhs ) )
, m_vDouble( std::move( rhs.m_vDouble ) )
, m_queue( std::move( rhs.m_queue ) )
{}

ChartEntryPrice::~ChartEntryPrice() {
  ClearQueue();  // clear out prior to m_vDouble disappears.
}

void ChartEntryPrice::Reserve( size_type nSize ) {
  m_vDouble.reserve( nSize );
}

void ChartEntryPrice::Clear() {
  ClearQueue();
  m_vDouble.clear();
  ChartEntryTime::Clear();
}

void ChartEntryPrice::Append( const ou::tf::Price& price) {
  m_queue.Append( price );
}

void ChartEntryPrice::Append( const boost::posix_time::ptime &dt, double price ) {
  Append( ou::tf::Price( dt, price ) );
}

void ChartEntryPrice::ClearQueue() {
  namespace ph = std::placeholders;
  m_queue.Sync( std::bind( &ChartEntryPrice::Pop, this, ph::_1 ) );
  ChartEntryTime::ClearQueue();
}

void ChartEntryPrice::Pop( const ou::tf::Price& price ) {
  ChartEntryTime::AppendFg( price.DateTime() );
  m_vDouble.push_back( price.Value() );
}

bool ChartEntryPrice::AddEntryToChart( XYChart *pXY, structChartAttributes *pAttributes )  {
  bool bAdded( false );
  ClearQueue();
  if ( 0 != this->ChartEntryTime::Size() ) {
    DoubleArray daXData = ChartEntryTime::GetDateTimes();
    if ( 0 != daXData.len ) {
      LineLayer *ll = pXY->addLineLayer( this->GetPrices() );
      ll->setXData( daXData );
      pAttributes->dblXMin = daXData[0];
      pAttributes->dblXMax = daXData[ daXData.len - 1 ];
      DataSet *pds = ll->getDataSet(0);
      pds->setDataColor( m_eColour );
      pds->setDataName( GetName().c_str() );
      bAdded = true;
    }
  }
  return bAdded;
}

} // namespace ou