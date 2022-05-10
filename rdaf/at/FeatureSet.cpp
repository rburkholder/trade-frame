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
 * File:    FeatureSet.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/at
 * Created: May 7, 2022 15:39
 */

#include "FeatureSet.hpp"

FeatureSet::FeatureSet()
: m_pTop( nullptr )
, m_pNext( nullptr )
{}

void FeatureSet::Set( int ix, FeatureSet* pTop, FeatureSet* pNext ) {
  m_ix = ix;
  m_pTop = pTop;
  m_pNext = pNext;
}

void FeatureSet::QuoteAsk( const ou::tf::Depth& depth ) {

  price_t price( depth.Price() );
  volume_t volume( depth.Volume() );

  DerivativesAsk( depth ); // requires use of current value for

  if ( v1.priceAsk != price ) {
    v1.priceAsk = price;
    QuotePriceUpdates();
    Diff();
  }
  if ( v1.volumeAsk != volume ) {
    v1.volumeAsk = volume;
    QuoteVolumeUpdates();
    if ( m_pNext ) m_pNext->AggregateAsk( v1.volumeAsk + v1.aggregateVolumeAsk );
  }
}

void FeatureSet::QuoteBid( const ou::tf::Depth& depth ) {

  price_t price( depth.Price() );
  volume_t volume( depth.Volume() );

  DerivativesBid( depth ); // requires use of current value for

  if ( v1.priceBid != price ) {
    v1.priceBid = price;
    QuotePriceUpdates();
    Diff();
  }
  if ( v1.volumeBid != volume ) {
    v1.volumeBid = volume;
    QuoteVolumeUpdates();
    if ( m_pNext ) m_pNext->AggregateBid( v1.volumeBid + v1.aggregateVolumeBid );
  }
}

void FeatureSet::QuotePriceUpdates() {
  v2.spread = v1.priceAsk - v1.priceBid;
  v2.mid = ( v1.priceAsk + v1.priceBid ) / 2.0;
}

void FeatureSet::QuoteVolumeUpdates() {
  v2.imbalanceLvl = (double)( v1.volumeBid - v1.volumeAsk ) / (double)( v1.volumeBid + v1.volumeAsk );
}

void FeatureSet::ImbalanceOnAggregate() {
  volume_t sumAsk = v1.volumeAsk + v1.aggregateVolumeAsk;
  volume_t sumBid = v1.volumeBid + v1.aggregateVolumeBid;
  v2.imbalanceAgg = (double)( sumBid - sumAsk ) / (double)( sumBid + sumAsk );
}

void FeatureSet::Diff() { // if not all levels present, then some bad numbers?
  if ( m_pTop ) {
    v3.diffToTopAsk = v1.priceAsk - m_pTop->v1.priceAsk;
    v3.diffToTopBid = m_pTop->v1.priceBid - v1.priceBid;
  }
  else {
    v3.diffToTopAsk = v3.diffToTopBid = 0.0;
  }

  if ( m_pNext ) {
    v3.diffToAdjacentAsk = m_pNext->v1.priceAsk - v1.priceAsk;
    v3.diffToAdjacentBid = v1.priceBid - m_pNext->v1.priceBid;
  }
  else {
    v3.diffToAdjacentAsk = v3.diffToAdjacentBid = 0.0;
  }
}

void FeatureSet::AggregateAsk( price_t aggregate ) {
  v1.aggregatePriceAsk = aggregate;
  price_t sum( v1.priceAsk + aggregate );
  v4.meanPriceAsk = sum / m_ix;
  v5.sumPriceSpreads =  sum - ( v1.priceBid + v1.aggregatePriceBid );
  if ( m_pNext ) m_pNext->AggregateAsk( sum );
}

void FeatureSet::AggregateBid( price_t aggregate ) {
  v1.aggregatePriceBid = aggregate;
  price_t sum( v1.priceBid + aggregate );
  v4.meanPriceBid = sum / m_ix;
  v5.sumPriceSpreads = ( v1.priceAsk + v1.aggregatePriceAsk ) - sum;
  if (m_pNext ) m_pNext->AggregateBid( sum );
}

void FeatureSet::AggregateAsk( volume_t aggregate ) {
  v1.aggregateVolumeAsk = aggregate;
  volume_t sum( v1.volumeAsk + aggregate );
  v4.meanVolumeAsk = sum / m_ix;
  v5.sumVolumeSpreads = sum - ( v1.volumeBid + v1.aggregateVolumeBid );
  ImbalanceOnAggregate();
  if ( m_pNext ) m_pNext->AggregateAsk( sum );
}

void FeatureSet::AggregateBid( volume_t aggregate ) {
  v1.aggregateVolumeBid = aggregate;
  volume_t sum( v1.volumeBid + aggregate );
  v4.meanVolumeBid = sum / m_ix;
  v5.sumVolumeSpreads = ( v1.volumeAsk + v1.aggregateVolumeAsk ) - sum;
  ImbalanceOnAggregate();
  if ( m_pNext ) m_pNext->AggregateBid( sum );
}

namespace {
  // exponential sliding window over 20 values
  static const double dblWeightTail = 19.0 / 20.0;
  static const double dblWeightHead =  1.0 / 20.0;
}

void FeatureSet::DerivativesAsk( const ou::tf::Depth& depth ) {

  if ( boost::posix_time::not_a_date_time == v6.dtLastAsk ) {
    v6.deltaArrivalAsk = 0.0;
  }
  else {
    auto diff = ( depth.DateTime() - v6.dtLastAsk ).total_microseconds(); // might be delete -> update
    if ( 0 < diff ) {
      v6.deltaArrivalAsk = (double)diff;
      v6.dPriceAsk_dt  = dblWeightTail * v6.dPriceAsk_dt  + dblWeightHead * ( depth.Price()  / v6.deltaArrivalAsk ); // slope = rise / run
      v6.dVolumeAsk_dt = dblWeightTail * v6.dVolumeAsk_dt + dblWeightHead * ( depth.Volume() / v6.deltaArrivalAsk ); // slope = rise / run
    }
  }
  v6.dtLastAsk = depth.DateTime();
}

void FeatureSet::DerivativesBid( const ou::tf::Depth& depth ) {

  if ( boost::posix_time::not_a_date_time == v6.dtLastBid ) {
    v6.deltaArrivalBid = 0.0;
  }
  else {
    auto diff = ( depth.DateTime() - v6.dtLastBid ).total_microseconds(); // might be delete -> update
    if ( 0 < diff ) {
      v6.deltaArrivalBid = (double)diff;
      v6.dPriceBid_dt  = dblWeightTail * v6.dPriceBid_dt  + dblWeightHead * ( depth.Price()  / v6.deltaArrivalBid ); // slope = rise / run
      v6.dVolumeBid_dt = dblWeightTail * v6.dVolumeBid_dt + dblWeightHead * ( depth.Volume() / v6.deltaArrivalBid ); // slope = rise / run
    }
  }
  v6.dtLastBid = depth.DateTime();
}

FeatureSet& FeatureSet::operator=( const FeatureSet& rhs ) {
  if ( this != &rhs ) {
    v1 = rhs.v1;
    v2 = rhs.v2;
    v3 = rhs.v3;
    v4 = rhs.v4;
    v5 = rhs.v5;
    v6 = rhs.v6;
    v7 = rhs.v7;
    v8 = rhs.v8;
    v9 = rhs.v9;
  }
  return *this;
}

void FeatureSet::CopyFromHere( const FeatureSet& rhs ) {
  if ( m_pNext ) m_pNext->CopyFromHere( *this );
  *this = rhs;
}

void FeatureSet::CopyToHere( FeatureSet& lhs ) {
  lhs = *this;
  if ( m_pNext ) m_pNext->CopyToHere( *this );
 }

