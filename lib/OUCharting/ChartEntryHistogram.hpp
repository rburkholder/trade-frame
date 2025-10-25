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
 * Project: SP500
 * Created: October 25, 2025 09:44:03
 */

#pragma once

#include <TFTimeSeries/DatedDatum.h>
#include <TFTimeSeries/DoubleBuffer.h>

#include "ChartEntryBase.h"

namespace ou { // One Unified

class ChartEntryHistogram: public ChartEntryTime {
public:

  ChartEntryHistogram();
  ChartEntryHistogram( ChartEntryHistogram&& );
  virtual ~ChartEntryHistogram();

  void Add( bool direction, const ou::tf::Trade& );

  virtual bool AddEntryToChart( XYChart* pXY, structChartAttributes* pAttributes ) override;

  virtual void Clear() override;

protected:
private:

  ou::tf::Queue<ou::tf::Trade> m_queue;

  struct volumes_t {
    using volume_t = ou::tf::Trade::volume_t;
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


};

} // namespace ou