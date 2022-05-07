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

#pragma once

#include <TFTimeSeries/DatedDatum.h>

using price_t = ou::tf::Trade::price_t;
using volume_t = ou::tf::Trade::volume_t;

class FeatureSet {
public:
protected:
private:

  struct v1 { // absolute
    price_t priceAsk;
    volume_t volumeAsk;
    price_t priceBid;
    volume_t volumeBid;
  };

  struct v2 {
    price_t spread; // diff
    price_t mid;    // absolute
    double imbalance;  // (volBid - volAsk ) / ( volBid + volAsk ) -- not in the paper
  };

  struct v3 { // diff
    price_t diffToTopAsk;
    price_t diffToTopBid;
    price_t diffToAdjacentAsk;
    price_t diffToAdjacentBid;
  };

  struct v4 { // absolute
    price_t meanPriceAsk;
    price_t meanPriceBid;
    volume_t meanVolumeAsk;
    volume_t meanVolumeBid;
  };

  struct v5 { // sum(diff)
    price_t sumPriceSpreads;
    volume_t sumVolumeSpreads;
  };

  struct v6 { // derivative per unit time (1 sec)
    price_t dPriceAsk_dt;
    price_t dPriceBid_dt;
    volume_t dVolumeAsk_dt;
    volume_t dVolumeBid_dt;
  };

  struct v7 { // intensity over per unit time (1 sec)
    double intensityLimitAsk;
    double intensityLimtBid;
    double intensityMarketAsk;
    double intensityMarketBid;
    double intensityCancelAsk;
    double intensityCancelBid;
  };

  struct v8 { // relative intensity of short period vs long period (10s vs 900s)
    double relativeLimitAsk;
    double relativeLimitBid;
    double relativeaMarketAsk;
    double relativeMarketBid;
  };

  struct v9 { // accelleration of trading type per unit time (vs previous 1 sec)
    double accellLimitAsk;
    double accellLimitBid;
    double accellMarketAsk;
    double accellMarketBid;
  };

};