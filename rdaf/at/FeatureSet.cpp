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

void FeatureSet::QuoteAsk( price_t price, volume_t volume ) {
  if ( v1.priceAsk != price ) {
    v1.priceAsk = price;
    QuotePriceUpdates();
    Diff();
  }
  if ( v1.volumeAsk != volume ) {
    v1.volumeAsk = volume;
    QuoteVolumeUpdates();
    if ( m_pNext ) m_pNext->VolumeAsk( v1.volumeAsk + v1.aggregateVolumeAsk );
  }
}

void FeatureSet::QuoteBid( price_t price, volume_t volume ) {
  if ( v1.priceBid != price ) {
    v1.priceBid = price;
    QuotePriceUpdates();
    Diff();
  }
  if ( v1.volumeBid != volume ) {
    v1.volumeBid = volume;
    QuoteVolumeUpdates();
    if ( m_pNext ) m_pNext->VolumeBid( v1.volumeBid + v1.aggregateVolumeBid );
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

void FeatureSet::PriceAsk( price_t aggregate ) {
  v1.aggregatePriceAsk = aggregate;
  price_t sum( v1.priceAsk + aggregate );
  v4.meanPriceAsk = sum / m_ix;
  v5.sumPriceSpreads =  sum - ( v1.priceBid + v1.aggregatePriceBid );
  if ( m_pNext ) m_pNext->PriceAsk( sum );
}

void FeatureSet::PriceBid( price_t aggregate ) {
  v1.aggregatePriceBid = aggregate;
  price_t sum( v1.priceBid + aggregate );
  v4.meanPriceBid = sum / m_ix;
  v5.sumPriceSpreads = ( v1.priceAsk + v1.aggregatePriceAsk ) - sum;
  if (m_pNext ) m_pNext->PriceBid( sum );
}

void FeatureSet::VolumeAsk( volume_t aggregate ) {
  v1.aggregateVolumeAsk = aggregate;
  volume_t sum( v1.volumeAsk + aggregate );
  v4.meanVolumeAsk = sum / m_ix;
  v5.sumVolumeSpreads = sum - ( v1.volumeBid + v1.aggregateVolumeBid );
  ImbalanceOnAggregate();
  if ( m_pNext ) m_pNext->VolumeAsk( sum );
}

void FeatureSet::VolumeBid( volume_t aggregate ) {
  v1.aggregateVolumeBid = aggregate;
  volume_t sum( v1.volumeBid + aggregate );
  v4.meanVolumeBid = sum / m_ix;
  v5.sumVolumeSpreads = ( v1.volumeAsk + v1.aggregateVolumeAsk ) - sum;
  ImbalanceOnAggregate();
  if ( m_pNext ) m_pNext->VolumeBid( sum );
}
