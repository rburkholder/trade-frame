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

#include "StdAfx.h"

#include <algorithm>

#include <TFTrading/InstrumentManager.h>

#include "NoRiskInterestRateSeries.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

NoRiskInterestRateSeries::NoRiskInterestRateSeries( const vSymbol_t& vSymbol ) 
  : m_bInitialized( false ), m_bWatching( false )
{
  for ( vSymbol_t::const_iterator iter = vSymbol.begin(); vSymbol.end() != iter; ++ iter ) {
    m_vInterestRate.push_back( *iter );
  }

  ou::tf::InstrumentManager& mgr( ou::tf::InstrumentManager::Instance() );
  ou::tf::Instrument::pInstrument_t pInstrument;
  for ( vInterestRate_iter_t iter = m_vInterestRate.begin(); m_vInterestRate.end() != iter; ++iter ) {
    // need to check if it already exists
    if ( mgr.Exists( iter->Symbol, pInstrument ) ) {
    }
    else {
      pInstrument = mgr.ConstructInstrument( iter->Symbol, "INDEX", ou::tf::InstrumentType::Index );
    }
    iter->pWatch.reset( new Watch( pInstrument, m_pProvider ) );
  }
}

NoRiskInterestRateSeries::~NoRiskInterestRateSeries(void) {
}

void NoRiskInterestRateSeries::SetWatchOn( pProvider_t pProvider ) {
  assert( ou::tf::keytypes::EProviderIQF == pProvider->ID() );
  if ( !m_bInitialized) {
    m_bInitialized = true;
    m_pProvider = pProvider;
//    local::Initialize( pProvider );
  }
  if ( !m_bWatching ) {
    m_bWatching = true;
    for ( vInterestRate_iter_t iter = m_vInterestRate.begin(); m_vInterestRate.end() != iter; ++ iter ) {
      iter->pWatch->StartWatch();
    }
  }
}

void NoRiskInterestRateSeries::SetWatchOff( void ) {
  if ( m_bWatching ) {
    m_bWatching = false;
    for ( vInterestRate_iter_t iter = m_vInterestRate.begin(); m_vInterestRate.end() != iter; ++ iter ) {
      iter->pWatch->StopWatch();
    }
  }
}

double NoRiskInterestRateSeries::ValueAt( time_duration td ) {
  assert( m_bWatching );
  assert( td >= m_vInterestRate[ 0 ].td );
  structInterestRate tmp( td, "" );
  vInterestRate_iter_t iter1 = std::lower_bound( m_vInterestRate.begin(), m_vInterestRate.end(), tmp, compareInterestRate() );
  vInterestRate_iter_t iter2;
  if ( m_vInterestRate.end() == iter1 ) {
    // extrapolate beyond end
    --iter1;
    iter2 = iter1;
    -- iter1;
  }
  else {
    if ( iter1->td == td ) {
      // use the assigned value
      return iter1->pWatch->LastTrade().Price();
    }
    else {
      // interpolate
      iter2 = iter1;
      --iter1;
    }
  }
  double rate = iter1->pWatch->LastTrade().Price() + 
    ( ( iter2->pWatch->LastTrade().Price() - iter1->pWatch->LastTrade().Price() ) * 
    ( ( (double)( td - iter1->td ).total_seconds() ) / ( (double) ( iter2->td - iter1->td ).total_seconds() ) ) );
  return rate;
}

void NoRiskInterestRateSeries::EmitYieldCurve( void ) {
  for ( vInterestRate_iter_t iter = m_vInterestRate.begin(); m_vInterestRate.end() != iter; ++ iter ) {
    std::cout << iter->Symbol << " " << iter->pWatch->LastTrade().Price() << std::endl;
  }
}

} // namespace tf
} // namespace ou
