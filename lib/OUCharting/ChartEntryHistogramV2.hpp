/************************************************************************
 * Copyright(c) 2026, One Unified. All rights reserved.                 *
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
 * File:    ChartEntryHistogramV2.hpp
 * Author:  raymond@burkholder.net
 * Project: OUCharting
 * Created: February 1, 2026 13:14:16
 */

#pragma once

#include <TFTimeSeries/DatedDatum.h>
#include <TFTimeSeries/DoubleBuffer.h>
#include <boost-1_90/boost/date_time/posix_time/ptime.hpp>

#include "ChartEntryBase.h"

// keep time series, dynamically build volume at price map based upon viewport end points

namespace ou { // One Unified

class ChartEntryHistogram_v2: public ChartEntryTime {
public:

  ChartEntryHistogram_v2();
  ChartEntryHistogram_v2( ChartEntryHistogram_v2&& );
  virtual ~ChartEntryHistogram_v2();

  struct Datum {
    using ptime = boost::posix_time::ptime;
    ptime dt;
    double dblPrice;
    double dblVolume;
    bool bDirection;

    Datum( ptime dt_, double price, double volume, bool direction )
    : dt( dt_ ), dblPrice( price ), dblVolume( volume ), bDirection( direction )
    {}

    Datum( const Datum& rhs )
    : dt( rhs.dt ), dblPrice( rhs.dblPrice ), dblVolume( rhs.dblVolume ), bDirection( rhs.bDirection )
    {}
  };

  void Append( const Datum& );

  virtual bool AddEntryToChart( XYChart* pXY, structChartAttributes& ) override;

  virtual void Clear() override;

protected:
private:

  size_t m_ixStart;     // viewport
  size_t m_cntElement;  // viewport

  struct VolumeAtPrice_t {
    double atAsk;
    double atBid;
    double sumVolume;
    double price;
    VolumeAtPrice_t(): atAsk {}, atBid {}, sumVolume {}, price {}  {}
    VolumeAtPrice_t( double price_ )
    : atAsk {}, atBid {}, sumVolume {}, price( price_ )
    {}
    VolumeAtPrice_t( double atBid_, double atAsk_ )
    : atAsk( atAsk_ ), atBid( atBid_ ), sumVolume( atBid_ + atAsk_ ), price {}
    {}
    VolumeAtPrice_t( double atBid_, double atAsk_, double price_ )
    : atAsk( atAsk_ ), atBid( atBid_ ), sumVolume( atBid_ + atAsk_ ), price( price_ )
    {}
    VolumeAtPrice_t( const VolumeAtPrice_t& rhs )
    : atAsk( rhs.atAsk ), atBid( rhs.atBid ), sumVolume( rhs.sumVolume ), price( rhs.price )
    {}
    VolumeAtPrice_t( VolumeAtPrice_t&& rhs )
    : atAsk( rhs.atAsk ), atBid( rhs.atBid ), sumVolume( rhs.sumVolume ), price( rhs.price )
    {}

    VolumeAtPrice_t& operator+=( const VolumeAtPrice_t& rhs ) {
      assert( price == rhs.price );
      atAsk += rhs.atAsk;
      atBid += rhs.atBid;
      sumVolume += rhs.sumVolume;
      return *this;
    }

    VolumeAtPrice_t& operator=( const VolumeAtPrice_t& rhs ) {
      if ( &rhs != this ) {
        atAsk = rhs.atAsk;
        atBid = rhs.atBid;
        sumVolume = rhs.sumVolume;
        price = rhs.price;
      }
      return *this;
    }

    void max( const VolumeAtPrice_t& vap ) {
      if ( atAsk < vap.atAsk ) atAsk = vap.atAsk;
      if ( atBid < vap.atBid ) atBid = vap.atBid;
      if ( sumVolume < vap.sumVolume ) sumVolume = vap.sumVolume;
    }
  };

  // matches size of ChartEntryTime
  using vVolumeAtPrice_t = std::vector<VolumeAtPrice_t>;
  vVolumeAtPrice_t m_vVolumeAtPrice_raw;

  VolumeAtPrice_t m_vap_max; // for ratio'ing

  using mapVolumeAtPrice_t = std::map<double,VolumeAtPrice_t>;
  mapVolumeAtPrice_t m_mapVolumeAtPrice;

  ou::tf::Queue<Datum> m_queue;

  void Pop( const Datum& );

  virtual void ClearQueue() override;

};

} // namespace ou