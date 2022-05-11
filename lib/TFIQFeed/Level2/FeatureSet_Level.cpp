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

FeatureSet::FeatureSet()
: m_ix {}
, m_pTop( nullptr )
, m_pNext( nullptr )
{}

void FeatureSet::Set( int ix, FeatureSet* pTop, FeatureSet* pNext ) {
  m_ix = ix;
  m_pTop = pTop;
  m_pNext = pNext;
}

void FeatureSet::Ask_Quote( const ou::tf::Depth& depth ) {

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
    QuoteVolumeUpdates();
    if ( m_pNext ) m_pNext->Ask_Aggregate( ask.v1.volume + ask.v1.aggregateVolume );
  }
}

void FeatureSet::Bid_Quote( const ou::tf::Depth& depth ) {

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
    QuoteVolumeUpdates();
    if ( m_pNext ) m_pNext->Bid_Aggregate( bid.v1.volume + bid.v1.aggregateVolume );
  }
}

void FeatureSet::QuotePriceUpdates() {
  cross.v2.spread = ask.v1.price - bid.v1.price;
  cross.v2.mid = ( ask.v1.price + bid.v1.price ) / 2.0;
}

void FeatureSet::QuoteVolumeUpdates() {
  cross.v2.imbalanceLvl
    = (double)( bid.v1.volume - ask.v1.volume )
    / (double)( bid.v1.volume + ask.v1.volume );
}

void FeatureSet::ImbalanceOnAggregate() {
  volume_t sumAsk = ask.v1.volume + ask.v1.aggregateVolume;
  volume_t sumBid = bid.v1.volume + bid.v1.aggregateVolume;
  cross.v2.imbalanceAgg = (double)( sumBid - sumAsk ) / (double)( sumBid + sumAsk );
}

void FeatureSet::Ask_Diff() {
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

void FeatureSet::Bid_Diff() {
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

void FeatureSet::Ask_Aggregate( price_t aggregate ) {
  ask.v1.aggregatePrice = aggregate;
  price_t sum( ask.v1.price + aggregate );
  ask.v4.meanPrice = sum / m_ix;
  cross.v5.sumPriceSpreads =  sum - ( bid.v1.price + bid.v1.aggregatePrice );
  if ( m_pNext ) m_pNext->Ask_Aggregate( sum );
}

void FeatureSet::Bid_Aggregate( price_t aggregate ) {
  bid.v1.aggregatePrice = aggregate;
  price_t sum( bid.v1.price + aggregate );
  bid.v4.meanPrice = sum / m_ix;
  cross.v5.sumPriceSpreads = ( ask.v1.price + ask.v1.aggregatePrice ) - sum;
  if ( m_pNext ) m_pNext->Bid_Aggregate( sum );
}

void FeatureSet::Ask_Aggregate( volume_t aggregate ) {
  ask.v1.aggregateVolume = aggregate;
  volume_t sum( ask.v1.volume + aggregate );
  ask.v4.meanVolume = sum / m_ix;
  cross.v5.sumVolumeSpreads = sum - ( bid.v1.volume + bid.v1.aggregateVolume );
  ImbalanceOnAggregate();
  if ( m_pNext ) m_pNext->Ask_Aggregate( sum );
}

void FeatureSet::Bid_Aggregate( volume_t aggregate ) {
  bid.v1.aggregateVolume = aggregate;
  volume_t sum( bid.v1.volume + aggregate );
  bid.v4.meanVolume = sum / m_ix;
  cross.v5.sumVolumeSpreads = ( ask.v1.volume + ask.v1.aggregateVolume ) - sum;
  ImbalanceOnAggregate();
  if ( m_pNext ) m_pNext->Bid_Aggregate( sum );
}

namespace {
  // exponential sliding window over 20 values
  static const double dblWeightTail = 19.0 / 20.0;
  static const double dblWeightHead =  1.0 / 20.0;
}

void FeatureSet::Ask_Derivatives( const ou::tf::Depth& depth ) {

  if ( boost::posix_time::not_a_date_time == ask.v6.dtLast ) {
    ask.v6.deltaArrival = 0.0;
  }
  else {
    auto diff = ( depth.DateTime() - ask.v6.dtLast ).total_microseconds(); // might be delete -> update
    if ( 0 < diff ) {
      ask.v6.deltaArrival = (double)diff / 1000000.0; // rate per second
      ask.v6.dPrice_dt  = dblWeightTail * ask.v6.dPrice_dt  + dblWeightHead * ( depth.Price()  / ask.v6.deltaArrival ); // slope = rise / run
      ask.v6.dVolume_dt = dblWeightTail * ask.v6.dVolume_dt + dblWeightHead * ( depth.Volume() / ask.v6.deltaArrival ); // slope = rise / run
    }
  }
  ask.v6.dtLast = depth.DateTime();
}

void FeatureSet::Bid_Derivatives( const ou::tf::Depth& depth ) {

  if ( boost::posix_time::not_a_date_time == bid.v6.dtLast ) {
    bid.v6.deltaArrival = 0.0;
  }
  else {
    auto diff = ( depth.DateTime() - bid.v6.dtLast ).total_microseconds(); // might be delete -> update
    if ( 0 < diff ) {
      bid.v6.deltaArrival = (double)diff / 1000000.0; // rate per second
      bid.v6.dPrice_dt  = dblWeightTail * bid.v6.dPrice_dt  + dblWeightHead * ( depth.Price()  / bid.v6.deltaArrival ); // slope = rise / run
      bid.v6.dVolume_dt = dblWeightTail * bid.v6.dVolume_dt + dblWeightHead * ( depth.Volume() / bid.v6.deltaArrival ); // slope = rise / run
    }
  }
  bid.v6.dtLast = depth.DateTime();
}

//FeatureSet& FeatureSet::operator=( const FeatureSet& rhs ) {
//  if ( this != &rhs ) {
//    ask = rhs.ask;
//    bid = rhs.bid;
//  }
//  return *this;
//}

FeatureSet::BookLevel& FeatureSet::BookLevel::operator=( const FeatureSet::BookLevel& rhs ) {
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
void FeatureSet::Ask_CopyFrom( const FeatureSet& rhs ) {
  if ( m_pNext ) m_pNext->Ask_CopyFrom( *this );
  this->ask = rhs.ask;
}

// after deletion, shuffle all downwards
void FeatureSet::Ask_CopyTo( FeatureSet& lhs ) {
  lhs.ask = this->ask;
  if ( m_pNext ) m_pNext->Ask_CopyTo( *this );
 }

// prepare for insertion, shuffle all upwards
void FeatureSet::Bid_CopyFrom( const FeatureSet& rhs ) {
  if ( m_pNext ) m_pNext->Bid_CopyFrom( *this );
  this->bid = rhs.bid;
}

// after deletion, shuffle all downwards
void FeatureSet::Bid_CopyTo( FeatureSet& lhs ) {
  lhs.bid = this->bid;
  if ( m_pNext ) m_pNext->Bid_CopyTo( *this );
 }

} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
