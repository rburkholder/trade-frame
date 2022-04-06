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

#include "DataRow.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

class DataRows {
public:

  DataRows();
  DataRows( double interval );
  ~DataRows();

  void SetInterval( double );

  int Cast( double price ) const;
  double Cast( int ix ) const;

  DataRow& operator[]( double );
  DataRow& operator[]( int );

protected:
private:

  // to consider: build total ladder in memory?
  // however, locks used only on map expansion (for now)
  std::mutex m_mutexMap; // prevent foreground, l1, l2 thread interference

  using mapRow_t = std::map<int,DataRow>;

  double m_interval;
  double m_intervalby2;

  mapRow_t m_mapRow;

};

} // market depth
} // namespace tf
} // namespace ou
