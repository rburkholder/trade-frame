/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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

/*
 * File:    ChartEntryHistogram.cpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: October 25, 2025 09:44:03
 */

#include "ChartEntryHistogram.hpp"

namespace ou { // One Unified

ChartEntryHistogram::ChartEntryHistogram()
: ChartEntryTime()
, m_volume_max {}
{}

ChartEntryHistogram::ChartEntryHistogram( ChartEntryHistogram&& rhs )
: ChartEntryTime( std::move( rhs ) )
, m_queue( std::move( rhs.m_queue ) )
, m_mapVolumeAtPrice( std::move( rhs.m_mapVolumeAtPrice ) )
, m_volumes_max( rhs.m_volumes_max )
, m_volume_max( rhs.m_volume_max )
{
}

ChartEntryHistogram::~ChartEntryHistogram() {}

void ChartEntryHistogram::Add( bool direction, const ou::tf::Trade& trade ) {
  m_queue.Append( queued_trade_t( direction, trade ) );
}

void ChartEntryHistogram::Pop( const queued_trade_t& q ) {

  const bool direction( q.bDirection );
  const auto price( q.trade.Price() );
  const auto volume( q.trade.Volume() );

  mapVolumeAtPrice_t::iterator iterVolumeAtPrice = m_mapVolumeAtPrice.find( price );
  if ( m_mapVolumeAtPrice.end() == iterVolumeAtPrice ) {
    bool bResult;
    std::tie( iterVolumeAtPrice, bResult ) = m_mapVolumeAtPrice.emplace( price, volumes_t() );
    assert( bResult);
  }

  volumes_t& v( iterVolumeAtPrice->second );

  if ( direction ) {
    v.at_ask += volume;
    if ( m_volumes_max.at_ask < v.at_ask ) m_volumes_max.at_ask = v.at_ask;
  }
  else {
    v.at_bid += volume;
    if ( m_volumes_max.at_bid < v.at_bid ) m_volumes_max.at_bid = v.at_bid;
  }
  const auto sum( v.at_ask + v.at_bid );
  if ( m_volume_max < sum ) m_volume_max = sum;
}

void ChartEntryHistogram::ClearQueue() {
  ChartEntryTime::ClearQueue();
  namespace ph = std::placeholders;
  m_queue.Sync( std::bind( &ChartEntryHistogram::Pop, this, ph::_1 ) );
}

bool ChartEntryHistogram::AddEntryToChart( XYChart* pXY, structChartAttributes& attributes ) {

  bool bAdded( false );
  ClearQueue();

  // ToDo:
  //   stage 1: generic historgram of full time range
  //   stage 2: histogram of prices within visible chart

  DoubleArray daXData = ChartEntryTime::GetViewPortDateTimes();
  //if ( 0 != daXData.len ) {
    //LineLayer *ll = pXY->addLineLayer( this->GetPrices() );
    //ll->setXData( daXData );
    //pAttributes->dblXMin = daXData[0];
    //pAttributes->dblXMax = daXData[ daXData.len - 1 ];
    //DataSet *pds = ll->getDataSet(0);
    //pds->setDataColor( m_eColour );
    //pds->setDataName( GetName().c_str() );
    //if ( 0 < )
    bAdded = true;
  //}
  return bAdded;
}

void ChartEntryHistogram::Clear() {
  m_queue.Clear();
  m_mapVolumeAtPrice.clear();
  //ChartEntryTime::Clear();
}

} // namespace ou