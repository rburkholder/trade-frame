/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include "Strike.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

Strike::Strike()
: m_dblStrike( 0 ),
  m_bWatchable( false ), m_nWatching( 0 )
{
}

Strike::Strike( double dblStrike )
: m_dblStrike( dblStrike ),
  m_bWatchable( false ), m_nWatching( 0 )
{
}

Strike::Strike( const Strike& rhs )
: m_dblStrike( rhs.m_dblStrike ),
  m_call( rhs.m_call ), m_put( rhs.m_put ),
  m_bWatchable( false ), m_nWatching( 0 )
{
  assert( 0 == rhs.m_nWatching );
}

Strike::~Strike() {
}

Strike& Strike::operator=( const Strike& rhs ) {
  assert( 0 == rhs.m_nWatching );
  assert( 0 == m_nWatching );
  m_dblStrike = rhs.m_dblStrike;
  m_call = rhs.m_call;
  m_put = rhs.m_put;
  return *this;
};

void Strike::AssignCall( Instrument::pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider ) {
  assert( 0 == m_call.use_count() );
  assert( ou::tf::OptionSide::Call == pInstrument->GetOptionSide() );
  if ( 0 != m_call.use_count() ) { // look at this: not needed given the above assertion
    if ( 0 < m_nWatching ) m_call->StopWatch();
  }
  m_call.reset( new ou::tf::option::Call( pInstrument, pDataProvider, pGreekProvider ) );
  if ( 0 < m_nWatching ) m_call->StartWatch();
};

void Strike::AssignPut( Instrument::pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider )  {
  assert( 0 == m_put.use_count() );
  assert( ou::tf::OptionSide::Put == pInstrument->GetOptionSide() );
  if ( 0 != m_put.use_count() ) { // look at this: not needed given the above assertion
    if ( 0 < m_nWatching ) m_put->StopWatch();
  }
  m_put.reset( new ou::tf::option::Put( pInstrument, pDataProvider, pGreekProvider ) );
  if ( 0 < m_nWatching ) m_put->StartWatch();
};

void Strike::AssignCall( Instrument::pInstrument_t pInstrument, pProvider_t pDataProvider ) {
  assert( 0 == m_call.use_count() );
  assert( ou::tf::OptionSide::Call == pInstrument->GetOptionSide() );
  if ( 0 != m_call.use_count() ) { // look at this: not needed given the above assertion
    if ( 0 < m_nWatching ) m_call->StopWatch();
  }
  m_call.reset( new ou::tf::option::Call( pInstrument, pDataProvider ) );
  if ( 0 < m_nWatching ) m_call->StartWatch();
};

void Strike::AssignPut( Instrument::pInstrument_t pInstrument, pProvider_t pDataProvider )  {
  assert( 0 == m_put.use_count() );
  assert( ou::tf::OptionSide::Put == pInstrument->GetOptionSide() );
  if ( 0 != m_put.use_count() ) { // look at this: not needed given the above assertion
    if ( 0 < m_nWatching ) m_put->StopWatch();
  }
  m_put.reset( new ou::tf::option::Put( pInstrument, pDataProvider ) );
  if ( 0 < m_nWatching ) m_put->StartWatch();
};

void Strike::EmitValues() {
  if ( m_call ) m_call->EmitValues();
  std::cout << std::endl;
  if ( m_put  ) m_put->EmitValues();
  std::cout << std::endl;
}

void Strike::SaveSeries( const std::string& sPrefix ) {
  if ( m_call ) m_call->SaveSeries( sPrefix );
  if ( m_put  ) m_put->SaveSeries( sPrefix );
}

void Strike::SetWatchableOn() {
  if ( !m_bWatchable ) {
    m_bWatchable = true;
    // won't be watching anything, as can't set watching without first having watchable
  }
}

void Strike::SetWatchableOff() {
  if ( m_bWatchable ) {
    m_bWatchable = false;
    if ( 0 != m_nWatching ) { // turn off watching, if enabled, or do we let this go down naturally instead?
//      WatchStop();
//      m_nWatching = 0; // this is kinda dangerous
    }
  }
}

void Strike::WatchStart() {
  if ( m_bWatchable ) {
    ++m_nWatching;
    if ( 1 == m_nWatching ) {
      if ( m_call ) m_call->StartWatch();
      if ( m_put  ) m_put->StartWatch();
    }
  }
}

void Strike::WatchStop() {
  assert( 0 != m_nWatching );
  -- m_nWatching;
  if ( 0 == m_nWatching ) {
    if ( m_call ) m_call->StopWatch();
    if ( m_put  ) m_put->StopWatch();
  }
}

} // namespace option
} // namespace tf
} // namespace ou

