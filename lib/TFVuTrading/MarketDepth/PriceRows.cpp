/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    PriceRows.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: November 11, 2021 16:46
 */

#include "PriceRows.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

PriceRows::PriceRows() {
  SetInterval( 0.01 );
}


PriceRows::PriceRows( double interval ) {
  SetInterval( interval );
}

PriceRows::~PriceRows() {
}

void PriceRows::SetInterval( double interval ) {
  assert( 0.0 < interval );
  m_interval = interval;
  m_intervalby2 = interval / 2.0;
}

int PriceRows::Cast( double price ) const {
  return std::floor( ( price + m_intervalby2 ) / m_interval );
}

double PriceRows::Cast( int ix ) const {
  return ix * m_interval;
}

PriceRow& PriceRows::operator[]( double price ) {
  //std::scoped_lock<std::mutex> lock( m_mutexMap ); // this might be a bit excessive, put back, and do double lookup
  int ix = Cast( price );
  mapRow_t::iterator iter = m_mapRow.find( ix );
  if ( m_mapRow.end() == iter ) {
    std::scoped_lock<std::mutex> lock( m_mutexMap ); // is this fine to reduce locking?
    iter = m_mapRow.find( ix ); // try again to confrirm
    if ( m_mapRow.end() == iter ) {
      auto pair = m_mapRow.emplace( std::make_pair( ix, PriceRow( price, rung::vElement ) ) );
      assert( pair.second );
      iter = pair.first;
    }
  }
  return iter->second;
}

PriceRow& PriceRows::operator[]( int ix ) {
  //std::scoped_lock<std::mutex> lock( m_mutexMap ); // this might be a bit excessive, put back, and do double lookup
  mapRow_t::iterator iter = m_mapRow.find( ix );
  if ( m_mapRow.end() == iter ) {
    std::scoped_lock<std::mutex> lock( m_mutexMap ); // is this fine to reduce locking
    iter = m_mapRow.find( ix ); // try again to confirm inside lock
    if ( m_mapRow.end() == iter ) {
      auto pair = m_mapRow.emplace( std::make_pair( ix, PriceRow( Cast( ix ), rung::vElement ) ) );
      assert( pair.second );
      iter = pair.first;
    }
  }
  return iter->second;
}

void PriceRows::ForEach( fForEach_t&& fForEach ) {
  if ( fForEach ) {
    std::scoped_lock<std::mutex> lock( m_mutexMap );
    for ( mapRow_t::value_type& vt: m_mapRow ) {
      fForEach( vt.first, vt.second );
    }
  }
}

} // market depth
} // namespace tf
} // namespace ou
