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

#include "DataRows.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

DataRows::DataRows( double interval )
: m_interval( interval ),
  m_intervalby2( interval / 2.0 )
{
  assert( 0 != interval );
}

DataRows::~DataRows() {
}

int DataRows::Cast( double price ) {
  return std::floor( ( price + m_intervalby2 ) / m_interval );
}

DataRow& DataRows::operator[]( double price ) {
  int ix = Cast( price );
  mapRow_t::iterator iter = m_mapRow.find( ix );
  if ( m_mapRow.end() == iter ) {
    auto pair = m_mapRow.emplace( std::make_pair( ix, DataRow( ix, price ) ) );
    assert( pair.second );
    iter = pair.first;
  }
  return iter->second;
}

} // market depth
} // namespace tf
} // namespace ou
