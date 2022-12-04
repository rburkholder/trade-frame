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
 * File:    DataRows.h
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: November 11, 2021 16:46
 */

#pragma once

#include <map>
#include <mutex>

#include "PriceRow.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

class PriceRows {
public:

  PriceRows();
  PriceRows( double interval );
  ~PriceRows();

  void SetInterval( double );

  int Cast( double price ) const; // price to index
  double Cast( int ix ) const;    // index to price

  PriceRow& operator[]( double ); // by price
  PriceRow& operator[]( int );    // by index

  using fForEach_t = std::function<void(int,PriceRow&)>;
  void ForEach( fForEach_t&& );

protected:
private:

  double m_interval;
  double m_intervalby2;

  // to consider: build total ladder in memory?
  // however, locks used only on map expansion (for now)
  std::mutex m_mutexMap; // prevent foreground, l1, l2 thread interference

  using mapRow_t = std::map<int,PriceRow>;
  mapRow_t m_mapRow;

};

} // market depth
} // namespace tf
} // namespace ou
