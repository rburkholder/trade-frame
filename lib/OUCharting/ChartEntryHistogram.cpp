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
{}

ChartEntryHistogram::ChartEntryHistogram( ChartEntryHistogram&& rhs )
: ChartEntryTime( std::move( rhs ) )
{
  //, m_vDouble( std::move( rhs.m_vDouble ) )
  //m_queue( std::move( rhs.m_queue ) );
}

ChartEntryHistogram::~ChartEntryHistogram() {}

void ChartEntryHistogram::Add( bool direction, const ou::tf::Trade& trade ) {

  const auto price( trade.Price() );
  const auto volume( trade.Volume() );

  mapVolumeAtPrice_t::iterator iterVolumeAtPrice = m_mapVolumeAtPrice.find( price );
  if ( m_mapVolumeAtPrice.end() == iterVolumeAtPrice ) {
    bool bResult;
    std::tie( iterVolumeAtPrice, bResult ) = m_mapVolumeAtPrice.emplace( price, volumes_t() );
    assert( bResult);
  }

  volumes_t& v( iterVolumeAtPrice->second );

  if ( direction ) {
    v.at_ask += volume;
    //m_ceTradeVolumeUp.Append( dt, volume );
  }
  else {
    v.at_bid += volume;
    //m_ceTradeVolumeDn.Append( dt, -volume );
  }
}

void ChartEntryHistogram::Clear() {}

bool ChartEntryHistogram::AddEntryToChart( XYChart* pXY, structChartAttributes* pAttributes ) {
  return false;
}

} // namespace ou