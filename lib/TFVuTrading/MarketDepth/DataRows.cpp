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
 * File:    DataRows.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: November 11, 2021 16:46
 */

#include "DataRows.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

DataRows::DataRows() {
  SetInterval( 0.01 );
}


DataRows::DataRows( double interval ) {
  SetInterval( interval );
}

DataRows::~DataRows() {
}

void DataRows::SetInterval( double interval ) {
  assert( 0.0 < interval );
  m_interval = ( interval ),
  m_intervalby2 = interval / 2.0;
}

int DataRows::Cast( double price ) {
  return std::floor( ( price + m_intervalby2 ) / m_interval );
}

DataRow& DataRows::operator[]( double price ) {
  std::scoped_lock<std::mutex> lock( m_mutexMap ); // this might be a bit excessive
  int ix = Cast( price );
  mapRow_t::iterator iter = m_mapRow.find( ix );
  if ( m_mapRow.end() == iter ) {
    auto pair = m_mapRow.emplace( std::make_pair( ix, DataRow( ix, price ) ) );
    assert( pair.second );
    iter = pair.first;
  }
  return iter->second;
}

DataRow& DataRows::operator[]( int ix ) {
  std::scoped_lock<std::mutex> lock( m_mutexMap ); // this might be a bit excessive
  mapRow_t::iterator iter = m_mapRow.find( ix );
  if ( m_mapRow.end() == iter ) {
    auto pair = m_mapRow.emplace( std::make_pair( ix, DataRow( ix, m_interval * ix  ) ) );
    assert( pair.second );
    iter = pair.first;
  }
  return iter->second;
}

} // market depth
} // namespace tf
} // namespace ou
