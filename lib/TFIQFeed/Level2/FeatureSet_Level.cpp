/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    FeatureSet_Level.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFIQFeed/Level2
 * Created: May 7, 2022 15:39
 */

#include "FeatureSet_Level.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data

FeatureSet_Level::FeatureSet_Level()
: m_ix {}
, m_pTop( nullptr )
, m_pNext( nullptr )
{}

void FeatureSet_Level::Set( int ix, FeatureSet_Level* pTop, FeatureSet_Level* pNext ) {
  m_ix = ix;
  m_pTop = pTop;
  m_pNext = pNext;
}

void FeatureSet_Level::Ask_Quote( const ou::tf::Depth& depth ) {

  price_t price( depth.Price() );
  volume_t volume( depth.Volume() );

  Ask_Derivatives( depth ); // requires use of current value for

  if ( ask.v1.price != price ) {
    ask.v1.price = price;
    QuotePriceUpdates();
    Ask_Diff();
  }
  if ( ask.v1.volume != volume ) {
    ask.v1.volume = volume;
    QuoteVolumeUpdates(); // updates imbalanceLvl
    if ( 1 == m_ix ) {
      //ask.v1.aggregateVolume = 0.0; // TODO: fix, should always be 0.0 (something about level changing)
      Ask_AggregateV( 0.0 );
    }
    else {
      if ( m_pNext ) m_pNext->Ask_AggregateV( ask.v1.volume + ask.v1.aggregateVolume );
    }
  }
}

void FeatureSet_Level::Bid_Quote( const ou::tf::Depth& depth ) {

  price_t price( depth.Price() );
  volume_t volume( depth.Volume() );

  Bid_Derivatives( depth ); // requires use of current value for

  if ( bid.v1.price != price ) {
    bid.v1.price = price;
    QuotePriceUpdates();
    Bid_Diff();
  }
  if ( bid.v1.volume != volume ) {
    bid.v1.volume = volume;
    QuoteVolumeUpdates(); // updates imbalanceLvl
    if ( 1 == m_ix ) {
      //bid.v1.aggregateVolume = 0.0; // TODO: fix, should always be 0.0 (something about level changing)
      Bid_AggregateV( 0.0 );
    }
    else {
      if ( m_pNext ) m_pNext->Bid_AggregateV( bid.v1.volume + bid.v1.aggregateVolume );
    }
  }
}

void FeatureSet_Level::QuotePriceUpdates() {
  cross.v2.spread = ask.v1.price - bid.v1.price;
  cross.v2.mid = ( ask.v1.price + bid.v1.price ) / 2.0;
}

void FeatureSet_Level::QuoteVolumeUpdates() {
  cross.v2.imbalanceLvl
    = ( (double)bid.v1.volume - (double)ask.v1.volume )
    / (double)( bid.v1.volume + ask.v1.volume );
}

void FeatureSet_Level::ImbalanceOnAggregate() {
  double sumAsk = ask.v1.volume + ask.v1.aggregateVolume;
  double sumBid = bid.v1.volume + bid.v1.aggregateVolume;
  cross.v2.imbalanceAgg = ( sumBid - sumAsk ) / ( sumBid + sumAsk );
}

void FeatureSet_Level::Ask_Diff() {
  // if not all levels present, then some bad numbers?
  if ( m_pTop ) {
    ask.v3.diffToTop = ask.v1.price - m_pTop->ask.v1.price;
  }
  else {
    ask.v3.diffToTop = 0.0;
  }

  if ( m_pNext ) {
    ask.v3.diffToAdjacent = m_pNext->ask.v1.price - ask.v1.price;
  }
  else {
    ask.v3.diffToAdjacent = 0.0;
  }
}

void FeatureSet_Level::Bid_Diff() {
  // if not all levels present, then some bad numbers?
  if ( m_pTop ) {
    bid.v3.diffToTop = m_pTop->bid.v1.price - bid.v1.price;
  }
  else {
    bid.v3.diffToTop = 0.0;
  }

  if ( m_pNext ) {
    bid.v3.diffToAdjacent = bid.v1.price - m_pNext->bid.v1.price;
  }
  else {
    bid.v3.diffToAdjacent = 0.0;
  }
}

void FeatureSet_Level::Ask_AggregateP( price_t aggregate ) {
  ask.v1.aggregatePrice = aggregate;
  price_t sum( ask.v1.price + aggregate );
  ask.v4.meanPrice = sum / m_ix;
  cross.v5.sumPriceSpreads =  sum - ( bid.v1.price + bid.v1.aggregatePrice );
  if ( m_pNext ) m_pNext->Ask_AggregateP( sum );
}

void FeatureSet_Level::Bid_AggregateP( price_t aggregate ) {
  bid.v1.aggregatePrice = aggregate;
  price_t sum( bid.v1.price + aggregate );
  bid.v4.meanPrice = sum / m_ix;
  cross.v5.sumPriceSpreads = ( ask.v1.price + ask.v1.aggregatePrice ) - sum;
  if ( m_pNext ) m_pNext->Bid_AggregateP( sum );
}

void FeatureSet_Level::Ask_AggregateV( double aggregate ) {
  ask.v1.aggregateVolume = aggregate;
  double sum( ask.v1.volume + aggregate );
  ask.v4.meanVolume = sum / m_ix;
  cross.v5.sumVolumeSpreads = sum - ( bid.v1.volume + bid.v1.aggregateVolume );
  ImbalanceOnAggregate();
  if ( m_pNext ) m_pNext->Ask_AggregateV( sum );
}

void FeatureSet_Level::Bid_AggregateV( double aggregate ) {
  bid.v1.aggregateVolume = aggregate;
  double sum( bid.v1.volume + aggregate );
  bid.v4.meanVolume = sum / m_ix;
  cross.v5.sumVolumeSpreads = ( ask.v1.volume + ask.v1.aggregateVolume ) - sum;
  ImbalanceOnAggregate();
  if ( m_pNext ) m_pNext->Bid_AggregateV( sum );
}

namespace { // TODO turn into constexpr

  // exponential over 20 values
  static const double dblWeightShort = 20.0;
  static const double dblWeightHeadShort =                    1.0   / dblWeightShort;
  static const double dblWeightTailShort = ( dblWeightShort - 1.0 ) / dblWeightShort;

  // exponential over 200 values
  static const double dblWeightLong = 200.0;
  static const double dblWeightHeadLong =                   1.0   / dblWeightLong;
  static const double dblWeightTailLong = ( dblWeightLong - 1.0 ) / dblWeightLong;
}

void FeatureSet_Level::Ask_Derivatives( const ou::tf::Depth& depth ) {

  if ( boost::posix_time::not_a_date_time == ask.v6.dtLast ) {
    ask.v6.deltaArrival = 0.0;
  }
  else {
    auto diff = ( depth.DateTime() - ask.v6.dtLast ).total_microseconds(); // might be delete -> update
    if ( 0 < diff ) {
      ask.v6.deltaArrival = (double)diff / 1000000.0; // rate per second
      ask.v6.dPrice_dt  = dblWeightTailShort * ask.v6.dPrice_dt  + dblWeightHeadShort * ( depth.Price()  / ask.v6.deltaArrival ); // slope = rise / run
      ask.v6.dVolume_dt = dblWeightTailShort * ask.v6.dVolume_dt + dblWeightHeadShort * ( depth.Volume() / ask.v6.deltaArrival ); // slope = rise / run
    }
  }
  ask.v6.dtLast = depth.DateTime();
}

void FeatureSet_Level::Bid_Derivatives( const ou::tf::Depth& depth ) {

  if ( boost::posix_time::not_a_date_time == bid.v6.dtLast ) {
    bid.v6.deltaArrival = 0.0;
  }
  else {
    auto diff = ( depth.DateTime() - bid.v6.dtLast ).total_microseconds(); // might be delete -> update
    if ( 0 < diff ) {
      bid.v6.deltaArrival = (double)diff / 1000000.0; // rate per second
      bid.v6.dPrice_dt  = dblWeightTailShort * bid.v6.dPrice_dt  + dblWeightHeadShort * ( depth.Price()  / bid.v6.deltaArrival ); // slope = rise / run
      bid.v6.dVolume_dt = dblWeightTailShort * bid.v6.dVolume_dt + dblWeightHeadShort * ( depth.Volume() / bid.v6.deltaArrival ); // slope = rise / run
    }
  }
  bid.v6.dtLast = depth.DateTime();
}

FeatureSet_Level::BookLevel& FeatureSet_Level::BookLevel::operator=( const FeatureSet_Level::BookLevel& rhs ) {
  if ( this != &rhs ) {
    bActive = rhs.bActive;
    v1 = rhs.v1;
    v3 = rhs.v3;
    v4 = rhs.v4;
    v6 = rhs.v6;
    v7 = rhs.v7;
    v8 = rhs.v8;
    v9 = rhs.v9;
  }
  return *this;
}

// prepare for insertion, shuffle all upwards
void FeatureSet_Level::Ask_CopyFrom( const FeatureSet_Level& rhs ) {
  if ( m_pNext ) m_pNext->Ask_CopyFrom( *this );
  ask = rhs.ask;
}

// after deletion, shuffle all downwards
void FeatureSet_Level::Ask_CopyTo( FeatureSet_Level& lhs ) {
  lhs.ask = ask;
  if ( m_pNext ) m_pNext->Ask_CopyTo( *this );
 }

// prepare for insertion, shuffle all upwards
void FeatureSet_Level::Bid_CopyFrom( const FeatureSet_Level& rhs ) {
  if ( m_pNext ) m_pNext->Bid_CopyFrom( *this );
  bid = rhs.bid;
}

// after deletion, shuffle all downwards
void FeatureSet_Level::Bid_CopyTo( FeatureSet_Level& lhs ) {
  lhs.bid = bid;
  if ( m_pNext ) m_pNext->Bid_CopyTo( *this );
 }

// v7, v8, v9 - common code
void FeatureSet_Level::Intensity( const ou::tf::Depth& depth, ptime& dtLast, double& intensityShort, double& intensityLong, double& accelShort ) {
  if ( boost::posix_time::not_a_date_time == dtLast ) {
  }
  else {
    auto diff = ( depth.DateTime() - dtLast).total_microseconds();
    if ( 0 < diff ) {
      double intensityShortPrevious = intensityShort;
      double deltaArrival = (double)diff / 1000000.0; // rate per second
      intensityShort = dblWeightTailShort * intensityShort + dblWeightHeadShort / deltaArrival;
      intensityLong  = dblWeightTailLong  * intensityLong  + dblWeightHeadLong  / deltaArrival;

      double diffIntensity = intensityShort - intensityShortPrevious;
      accelShort     = dblWeightTailShort * accelShort     + dblWeightHeadShort * diffIntensity / deltaArrival;
    }
  }
  dtLast = depth.DateTime();
}

void FeatureSet_Level::Ask_IncLimit(  const ou::tf::Depth& depth ) {
  Intensity( depth, ask.v7.dtLastLimit, ask.v7.intensityLimit, ask.v8.intensityLimit, ask.v9.accelLimit );
  if ( 0.0 < ask.v8.intensityLimit ) {
    ask.v8.relativeLimit = ask.v7.intensityLimit / ask.v8.intensityLimit;
  }
}

void FeatureSet_Level::Ask_IncMarket( const ou::tf::Depth& depth ) {
  Intensity( depth, ask.v7.dtLastMarket, ask.v7.intensityMarket, ask.v8.intensityMarket, ask.v9.accelMarket );
  if ( 0.0 < ask.v8.intensityMarket ) {
    ask.v8.relativeMarket = ask.v7.intensityMarket / ask.v8.intensityMarket;
  }
}

void FeatureSet_Level::Ask_IncCancel( const ou::tf::Depth& depth ) {
  Intensity( depth, ask.v7.dtLastCancel, ask.v7.intensityCancel, ask.v8.intensityCancel, ask.v9.accelCancel );
  if ( 0.0 < ask.v8.intensityCancel ) {
    ask.v8.relativeCancel = ask.v7.intensityCancel / ask.v8.intensityCancel;
  }
}

void FeatureSet_Level::Bid_IncLimit(  const ou::tf::Depth& depth ) {
  Intensity( depth, bid.v7.dtLastLimit, bid.v7.intensityLimit, bid.v8.intensityLimit, bid.v9.accelLimit  );
  if ( 0.0 < bid.v8.intensityLimit ) {
    bid.v8.relativeLimit = bid.v7.intensityLimit / bid.v8.intensityLimit;
  }
}

void FeatureSet_Level::Bid_IncMarket( const ou::tf::Depth& depth ) {
  Intensity( depth, bid.v7.dtLastMarket, bid.v7.intensityMarket, bid.v8.intensityMarket, bid.v9.accelMarket );
  if ( 0.0 < bid.v8.intensityMarket ) {
    bid.v8.relativeMarket = bid.v7.intensityMarket / bid.v8.intensityMarket;
  }
}

void FeatureSet_Level::Bid_IncCancel( const ou::tf::Depth& depth ) {
  Intensity( depth, bid.v7.dtLastCancel, bid.v7.intensityCancel, bid.v8.intensityCancel, bid.v9.accelCancel );
  if ( 0.0 < bid.v8.intensityCancel ) {
    bid.v8.relativeCancel = bid.v7.intensityCancel / bid.v8.intensityCancel;
  }
}

void FeatureSet_Level::Emit() const {
  std::cout
    << "lvl " << m_ix
    << " ask: "
    << ask.v1.volume << "@" << ask.v1.price
    << "=>" << ask.v1.aggregateVolume
    << " bid: "
    << bid.v1.volume << "@" << bid.v1.price
    << "=>" << bid.v1.aggregateVolume
    << ",cross: "
    << "lvl=" << cross.v2.imbalanceLvl << ",agg=" << cross.v2.imbalanceAgg
    << std::endl;
}

} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
