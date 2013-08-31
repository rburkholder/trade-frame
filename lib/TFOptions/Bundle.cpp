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
  : m_bWatching( false ), m_stateOptionWatch( EOWSNoWatch ), 
  m_dblUpperTrigger( 0.0 ), m_dblLowerTrigger( 0.0 )
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
  }
  for ( mapStrikes_t::iterator iter = m_mapStrikes.begin(); m_mapStrikes.end() != iter; ++iter ) {
    iter->second.SaveSeries( sPrefix );
  }
}

void Bundle::EmitValues( void ) {
  if ( 0 != m_pwatchUnderlying.get() ) {
    m_pwatchUnderlying->EmitValues();
  }
  for ( mapStrikes_t::iterator iter = m_mapStrikes.begin(); m_mapStrikes.end() != iter; ++iter ) {
    iter->second.EmitValues();
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

Call* Bundle::GetCall( double dblStrike ) {
  mapStrikes_t::iterator iter = FindStrike( dblStrike );
  return iter->second.Call();
}

Put* Bundle::GetPut( double dblStrike ) {
  mapStrikes_t::iterator iter = FindStrike( dblStrike );
  return iter->second.Put();
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

//void Bundle::SetWatchUnderlyingOn( void ) {
//}

//void Bundle::SetWatchUnderlyingOff( void ) {
//}

void Bundle::SetWatchableOn( double dblStrike ) {
  mapStrikes_t::iterator iter = m_mapStrikes.find( dblStrike );
  if ( m_mapStrikes.end() != iter ) {
    iter->second.SetWatchableOn();
  }
  
}

void Bundle::SetWatchableOff( double dblStrike ) {
  mapStrikes_t::iterator iter = m_mapStrikes.find( dblStrike );
  if ( m_mapStrikes.end() != iter ) {
    iter->second.SetWatchableOff();
  }
}

void Bundle::SetWatchOn( double dblStrike, bool bForce ) {
  mapStrikes_t::iterator iter = m_mapStrikes.find( dblStrike );
  if ( bForce ) {
    iter->second.SetWatchableOn();
  }
  iter->second.SetWatchOn();
}

void Bundle::SetWatchOff( double dblStrike, bool bForce ) {
  mapStrikes_t::iterator iter = m_mapStrikes.find( dblStrike );
  iter->second.SetWatchOff();
  if ( bForce ) {
    iter->second.SetWatchableOff();
  }
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

// lower_bound: key value eq or gt than query
// upper_bound: key value ft than query
void Bundle::AdjacentStrikes( double dblValue, double& dblLower, double& dblUpper ) {

  mapStrikes_t::iterator iter = m_mapStrikes.lower_bound( dblValue ); 
  if ( m_mapStrikes.end() == iter ) {
    throw std::runtime_error( "Bundle::AdjacentStrikes: no upper strike available" );
  }
  dblUpper = iter->first;
  if ( dblValue == dblUpper ) {
    dblLower = dblUpper;
  }
  else {
    if ( m_mapStrikes.begin() == iter ) {
      throw std::runtime_error( "Bundle::AdjacentStrikes: already at lower lower end of strkes" );
    }
    --iter;
    dblLower = iter->first;
  }
}

void Bundle::RecalcATMWatch( double dblValue ) {
  mapStrikes_iter_t iterUpper;
  mapStrikes_iter_t iterLower;
  iterUpper = m_mapStrikes.lower_bound( dblValue ); 
  if ( m_mapStrikes.end() == iterUpper ) {
    std::cout << "Bundle::UpdateATMWatch: no upper strike available" << std::endl; // stay in no watch state
    m_stateOptionWatch = EOWSNoWatch;
  }
  else {
    iterLower = iterUpper;
    if ( m_mapStrikes.begin() == iterLower ) {
      std::cout << "Bundle::UpdateATMWatch: no lower strike available" << std::endl;  // stay in no watch state
      m_stateOptionWatch = EOWSNoWatch;
    }
    else {
      --iterLower;
      double dblMidPoint = ( iterUpper->first + iterLower->first ) * 0.5;
      if ( dblValue >= dblMidPoint ) { // third strike is above
        m_iterUpper = iterUpper;
        ++m_iterUpper;
        if ( m_mapStrikes.end() == m_iterUpper ) {
          std::cout << "Bundle::UpdateATMWatch: no upper upper strike available" << std::endl;  // stay in no watch state
          m_stateOptionWatch = EOWSNoWatch;
        }
        else {
          m_iterMid = iterUpper;
          m_iterLower = iterLower;
          m_stateOptionWatch = EOWSWatching;
        }
      }
      else { // third strike is below
        m_iterLower = iterLower;
        if ( m_mapStrikes.begin() == m_iterLower ) {
          std::cout << "Bundle::UpdateATMWatch: no lower lower strike available" << std::endl;  // stay in no watch state
          m_stateOptionWatch = EOWSNoWatch;
        }
        else {
          --m_iterLower;
          m_iterMid = iterLower;
          m_iterUpper = iterUpper;
          m_stateOptionWatch = EOWSWatching;
        }
      }
      if ( EOWSWatching == m_stateOptionWatch ) {
        m_dblUpperTrigger = m_iterUpper->first - ( m_iterUpper->first - m_iterMid->first ) * 0.25;
        m_dblLowerTrigger = m_iterLower->first + ( m_iterMid->first - m_iterLower->first ) * 0.25;
        std::cout << m_dblLowerTrigger << " < " << dblValue << " < " << m_dblUpperTrigger << std::endl;
      }
    }
  }
}

void Bundle::UpdateATMWatch( double dblValue ) {
  switch ( m_stateOptionWatch ) {
  case EOWSNoWatch:
    RecalcATMWatch( dblValue );
    SetWatchOn( m_iterUpper->first, true );
    SetWatchOn( m_iterMid->first, true );
    SetWatchOn( m_iterLower->first, true );
    break;
  case EOWSWatching:
    if ( ( dblValue > m_dblUpperTrigger ) || ( dblValue < m_dblLowerTrigger ) ) {
      mapStrikes_iter_t iterUpper( m_iterUpper );
      mapStrikes_iter_t iterMid( m_iterMid );
      mapStrikes_iter_t iterLower( m_iterLower );
      RecalcATMWatch( dblValue );
      if ( EOWSWatching == m_stateOptionWatch ) { // by setting on before off allows continuity of capture
        SetWatchOn( m_iterUpper->first, true );
        SetWatchOn( m_iterMid->first, true );
        SetWatchOn( m_iterLower->first, true );
      }
      SetWatchOff( iterUpper->first );
      SetWatchOff( iterMid->first );
      SetWatchOff( iterLower->first );
    }
    break;
  }
}

void Bundle::SetExpiry( ptime dt ) {
  m_dtExpiry = dt;
}

} // namespace option
} // namespace tf
} // namespace ou

