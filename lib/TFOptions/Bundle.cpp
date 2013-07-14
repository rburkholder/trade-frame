/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "Bundle.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

Bundle::Bundle(void)
  : m_bWatching( false )
{
}

Bundle::~Bundle(void) {
}

void Bundle::SetUnderlying( pInstrument_t pInstrument, pProvider_t pProvider ) {
  m_pwatchUnderlying.reset( new ou::tf::Watch( pInstrument, pProvider ) );
  if ( m_bWatching ) {
    m_pwatchUnderlying->StartWatch();
  }
}

void Bundle::SaveSeries( const std::string& sPrefix ) {
  if ( 0 != m_pwatchUnderlying.get() ) {
    m_pwatchUnderlying->SaveSeries( sPrefix );
    for ( mapStrikes_t::iterator iter = m_mapStrikes.begin(); m_mapStrikes.end() != iter; ++iter ) {
      iter->second.SaveSeries( sPrefix );
    }
  }
}

void Bundle::SetCall( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider ) {
  mapStrikes_t::iterator iter = FindStrikeAuto( pInstrument->GetStrike() );
  iter->second.AssignCall( pInstrument, pDataProvider, pGreekProvider );
}

void Bundle::SetPut( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider ) {
  mapStrikes_t::iterator iter = FindStrikeAuto( pInstrument->GetStrike() );
  iter->second.AssignPut( pInstrument, pDataProvider, pGreekProvider );
}

void Bundle::SetWatchOn( void ) {
  if ( !m_bWatching ) {
    m_bWatching = true;
    if ( 0 != m_pwatchUnderlying.get() ) m_pwatchUnderlying->StartWatch();
    for ( mapStrikes_t::iterator iter = m_mapStrikes.begin(); m_mapStrikes.end() != iter; ++iter ) {
      iter->second.SetWatchOn();
    }
  }
}

void Bundle::SetWatchOff( void ) {
  if ( m_bWatching ) {
    m_bWatching = false;
    if ( 0 != m_pwatchUnderlying.get() ) m_pwatchUnderlying->StopWatch();
    for ( mapStrikes_t::iterator iter = m_mapStrikes.begin(); m_mapStrikes.end() != iter; ++iter ) {
      iter->second.SetWatchOff();
    }
  }
}

void Bundle::SetWatchableOn( double dblStrike ) {
  mapStrikes_t::iterator iter = m_mapStrikes.find( dblStrike );
  iter->second.SetWatchableOn();
}

void Bundle::SetWatchableOff( double dblStrike ) {
  mapStrikes_t::iterator iter = m_mapStrikes.find( dblStrike );
  iter->second.SetWatchableOff();
}

void Bundle::SetWatchOn( double dblStrike ) {
  mapStrikes_t::iterator iter = m_mapStrikes.find( dblStrike );
  iter->second.SetWatchOn();
}

void Bundle::SetWatchOff( double dblStrike ) {
  mapStrikes_t::iterator iter = m_mapStrikes.find( dblStrike );
  iter->second.SetWatchOff();
}


Bundle::mapStrikes_t::iterator Bundle::FindStrike( double strike ) {
  mapStrikes_t::iterator iter = m_mapStrikes.find( strike );
  if ( m_mapStrikes.end() == iter ) {
    throw std::runtime_error( "Bundle::FindStrike: can't find strike" );
  }
  return iter;
}

Bundle::mapStrikes_t::iterator Bundle::FindStrikeAuto( double strike ) {
  mapStrikes_t::iterator iter = m_mapStrikes.find( strike );
  if ( m_mapStrikes.end() == iter ) {
    m_mapStrikes[ strike ] = Strike( strike );
    iter = m_mapStrikes.find( strike );
  }
  return iter;
}

void Bundle::AdjacentStrikes( double dblStrike, double& dblLower, double& dblUpper ) {
  mapStrikes_t::iterator iter = m_mapStrikes.upper_bound( dblStrike );
  if ( m_mapStrikes.end() == iter ) {
    throw std::runtime_error( "Bundle::AdjacentStrikes: no upper strike available" );
  }
  dblUpper = iter->first;
  if ( m_mapStrikes.begin() == iter ) {
    throw std::runtime_error( "Bundle::AdjacentStrikes: already at lower lower end of strkes" );
  }
  --iter;
  dblLower = iter->first;
}

} // namespace option
} // namespace tf
} // namespace ou

