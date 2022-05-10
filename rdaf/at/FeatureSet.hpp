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
 * File:    FeatureSet.hpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/at
 * Created: May 7, 2022 15:39
 */

 // based upon the ppaper:
 // Modeling high-frequency limit order book dynamics with support vector machines
 // October 24, 2013, Alec N.Kercheval, Yuan Zhang
 // page 16, table 2, Feature Vector Sets

 // will need to determine limit orders (easy), market orders (hard), cancel orders (easy + market)

 // not really integrated into L2Base, as we only need n levels here, not all levels in the real book
 // need a signal for level deleted

#pragma once

#include <TFTimeSeries/DatedDatum.h>

using price_t = ou::tf::Trade::price_t;
using volume_t = ou::tf::Trade::volume_t;

struct FeatureSet {

  // if a level changes, change those plus deeper
  // if a level is added / removed, recalc all levels

  struct V1 { // absolute

    price_t priceAsk;
    volume_t volumeAsk;
    price_t priceBid;
    volume_t volumeBid;

    volume_t aggregateVolumeAsk;
    volume_t aggregateVolumeBid;

    price_t aggregatePriceAsk;
    price_t aggregatePriceBid;

    bool bNew; // can't tell a removal, but an addition might be significant
    V1()
    : priceAsk {}, volumeAsk {}, priceBid {}, volumeBid {}
    , aggregateVolumeAsk {}, aggregateVolumeBid {}
    , aggregatePriceAsk {}, aggregatePriceBid {}
    , bNew( false ) {}
  } v1;

  struct V2 {
    price_t spread; // diff
    price_t mid;    // absolute
    double imbalanceLvl;  // (volBid - volAsk ) / ( volBid + volAsk ) -- not in the paper
    double imbalanceAgg;  // (volBid - volAsk ) / ( volBid + volAsk ) -- not in the paper
    V2(): spread {}, mid {}, imbalanceLvl {}, imbalanceAgg {} {}
  } v2;

  struct V3 { // diff
    price_t diffToTopAsk;
    price_t diffToTopBid;
    price_t diffToAdjacentAsk;
    price_t diffToAdjacentBid;
    V3(): diffToTopAsk {}, diffToTopBid {}, diffToAdjacentAsk {}, diffToAdjacentBid {} {}
  } v3;

  struct V4 { // absolute
    price_t meanPriceAsk;
    price_t meanPriceBid;
    volume_t meanVolumeAsk;
    volume_t meanVolumeBid;
    V4(): meanPriceAsk {}, meanPriceBid {}, meanVolumeAsk {}, meanVolumeBid {} {}
  } v4;

  struct V5 { // sum(diff)
    price_t sumPriceSpreads;
    volume_t sumVolumeSpreads;
    V5(): sumPriceSpreads {}, sumVolumeSpreads {} {}
  } v5;

  struct V6 { // derivative per unit time

    ptime dtLastAsk;
    double deltaArrivalAsk;

    ptime dtLastBid;
    double deltaArrivalBid;

    price_t dPriceAsk_dt;
    price_t dPriceBid_dt;

    volume_t dVolumeAsk_dt;
    volume_t dVolumeBid_dt;

    V6()
    : dtLastAsk( boost::posix_time::not_a_date_time ), deltaArrivalAsk {}
    , dtLastBid( boost::posix_time::not_a_date_time ), deltaArrivalBid {}
    , dPriceAsk_dt {}, dPriceBid_dt {}, dVolumeAsk_dt{}, dVolumeBid_dt {} {}
  } v6;

  struct V7 { // intensity over per unit time (1 sec)
    double intensityLimitAsk;
    double intensityLimtBid;
    double intensityMarketAsk;
    double intensityMarketBid;
    double intensityCancelAsk;
    double intensityCancelBid;
    V7()
    : intensityLimitAsk {}, intensityLimtBid {}
    , intensityMarketAsk {}, intensityMarketBid {}
    , intensityCancelAsk {}, intensityCancelBid {}
    {}
  } v7;

  struct V8 { // relative intensity of short period vs long period (10s vs 900s)
    double relativeLimitAsk;
    double relativeLimitBid;
    double relativeaMarketAsk;
    double relativeMarketBid;
    V8(): relativeLimitAsk {}, relativeLimitBid {}, relativeaMarketAsk {}, relativeMarketBid {} {}
  } v8;

  struct V9 { // accelleration of trading type per unit time (vs previous 1 sec)
    double BookChangeFunctions;
    double accellLimitBid;
    double accellMarketAsk;
    double accellMarketBid;
    V9(): BookChangeFunctions {}, accellLimitBid {}, accellMarketAsk {}, accellMarketBid {} {}
  } v9;

  int m_ix;

  FeatureSet* m_pTop;
  FeatureSet* m_pNext;

  FeatureSet();

  void Set( int ix, FeatureSet* pTop, FeatureSet* pNext );

  FeatureSet& operator=( const FeatureSet& );

  void CopyFromHere( const FeatureSet& ); // make room for insertion
  void CopyToHere( FeatureSet& ); // deletion

  void QuoteAsk( const ou::tf::Depth& );
  void QuoteBid( const ou::tf::Depth& );
  void QuotePriceUpdates();
  void QuoteVolumeUpdates();
  void AggregateAsk( price_t aggregate ); // aggregate price from previous level
  void AggregateBid( price_t aggregate ); // aggregate price from previous level
  void AggregateAsk( volume_t aggregate );  // aggregate volume from previous level
  void AggregateBid( volume_t aggregate );  // aggregate volume from previous level
  void Diff();
  void ImbalanceOnAggregate();
  void DerivativesAsk( const ou::tf::Depth& );
  void DerivativesBid( const ou::tf::Depth& );

};
