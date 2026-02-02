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
 * File:    ChartEntryHistogram.hpp
 * Author:  raymond@burkholder.net
 * Project: OUCharting
 * Created: October 25, 2025 09:44:03
 */

#pragma once

#include <TFTimeSeries/DatedDatum.h>
#include <TFTimeSeries/DoubleBuffer.h>

#include "ChartEntryBase.h"

// generic histogram of full time range

namespace ou { // One Unified

class ChartEntryHistogram: public ChartEntryTime {
public:

  ChartEntryHistogram();
  ChartEntryHistogram( ChartEntryHistogram&& );
  virtual ~ChartEntryHistogram();

  void Add( bool direction, const ou::tf::Trade& );

  virtual bool AddEntryToChart( XYChart* pXY, structChartAttributes& ) override;

  virtual void Clear() override;

protected:
private:

  struct queued_trade_t {
    bool bDirection;
    const ou::tf::Trade trade;
    queued_trade_t( bool bDirection_, const ou::tf::Trade& trade_ )
    : bDirection( bDirection_ ), trade( trade_ ) {}
    //queued_trade_t( const queued_trade_t& rhs )
    //: bDirection( rhs.bDirection ), trade( rhs.trade ) {}
    queued_trade_t( queued_trade_t&& rhs )
    : bDirection( rhs.bDirection ), trade( rhs.trade ) {}
  };

  ou::tf::Queue<queued_trade_t> m_queue;

  using volume_t = ou::tf::Trade::volume_t;

  struct volumes_t {
    volume_t at_ask;
    volume_t at_bid;
    volumes_t(): at_ask {}, at_bid {} {}
    //volumes_t( volume_t bid, volume_t ask ): at_bid( bid ), at_ask( ask ) {}
    //const volumes_t& operator+=( const volumes_t& v ) {
    //  at_ask += v.at_ask;
    //  at_bid += v.at_bid;
    //  return *this;
    //}
  };
  using mapVolumeAtPrice_t = std::map<double,volumes_t>;
  mapVolumeAtPrice_t m_mapVolumeAtPrice;

  volumes_t m_volumes_max;

  void Pop( const queued_trade_t& );

  virtual void ClearQueue() override;

};

} // namespace ou