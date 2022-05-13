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
 * File:    FeatureSet_Level.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFIQFeed/Level2
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

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data

class FeatureSet_Level {
public:

  // if a level changes, change those plus deeper
  // if a level is added / removed, recalc all levels

  using price_t = ou::tf::Trade::price_t;
  using volume_t = ou::tf::Trade::volume_t;

  struct V1 { // absolute

    price_t price;
    volume_t volume;

    volume_t aggregateVolume;

    price_t aggregatePrice;

    bool bNew; // can't tell a removal, but an addition might be significant

    V1()
    : price {}, volume {}
    , aggregateVolume {}
    , aggregatePrice {}
    , bNew( false ) {}
  };

  struct V3 { // diff
    price_t diffToTop;
    price_t diffToAdjacent;
    V3(): diffToTop {}, diffToAdjacent {}
    {}
  };

  struct V4 { // absolute
    price_t meanPrice;
    volume_t meanVolume;
    V4(): meanPrice {}, meanVolume {}
    {}
  };

  struct V6 { // derivative per unit time

    ptime dtLast;
    double deltaArrival;

    price_t dPrice_dt;

    volume_t dVolume_dt;

    V6()
    : dtLast( boost::posix_time::not_a_date_time )
    , deltaArrival {}
    , dPrice_dt {}, dVolume_dt{}
    {}
  };

  struct V7 { // intensity over per unit time (1 sec)

    ptime dtLastLimit;
    double intensityLimit; // short term intensity

    ptime dtLastMarket;
    double intensityMarket; // short term intensity

    ptime dtLastCancel;
    double intensityCancel; // short term intensity
    V7()
    : dtLastLimit(  boost::posix_time::not_a_date_time ), intensityLimit  {}
    , dtLastMarket( boost::posix_time::not_a_date_time ), intensityMarket {}
    , dtLastCancel( boost::posix_time::not_a_date_time ), intensityCancel {}
    {}
  };

  struct V8 { // relative intensity of short period vs long period (10s vs 900s)

    double intensityLimit; // long term intensity
    double relativeLimit; // ratio of short term intensity vs long term intensity

    double intensityMarket; // long term intensity
    double relativeMarket; // ratio of short term intensity vs long term intensity

    double intensityCancel; // long term intensity
    double relativeCancel; // ratio of short term intensity vs long term intensity

    V8()
    : intensityLimit {},  relativeLimit {}
    , intensityMarket {}, relativeMarket {}
    , intensityCancel {}, relativeCancel {}
    {}
  };

  struct V9 { // accelleration of trading type per unit time (vs previous 1 sec)
    double BookChangeFunctions;
    double accellLimit;
    double accellMarket;
    V9()
    : BookChangeFunctions {}
    , accellLimit {}
    , accellMarket {}
    {}
  };

  struct BookLevel {

    bool bActive; // level has data

    V1 v1;
    V3 v3;
    V4 v4;
    V6 v6;
    V7 v7;
    V8 v8;
    V9 v9;

    BookLevel(): bActive( false ) {}
    BookLevel& operator=( const BookLevel& );
  };

  BookLevel ask;
  BookLevel bid;

  struct V2 {
    price_t spread; // diff
    price_t mid;    // absolute
    double imbalanceLvl;  // (volBid - volAsk ) / ( volBid + volAsk ) -- not in the paper
    double imbalanceAgg;  // (volBid - volAsk ) / ( volBid + volAsk ) -- not in the paper
    V2(): spread {}, mid {}, imbalanceLvl {}, imbalanceAgg {} {}
  };

  struct V5 { // sum(diff)
    price_t sumPriceSpreads;
    volume_t sumVolumeSpreads;
    V5(): sumPriceSpreads {}, sumVolumeSpreads {} {}
  };

  struct CrossLevel {
    V2 v2;
    V5 v5;
  };

  CrossLevel cross;

  FeatureSet_Level();

  void Set( int ix, FeatureSet_Level* pTop, FeatureSet_Level* pNext );

  void Ask_Activate( bool bActive ) { ask.bActive = bActive; }
  void Bid_Activate( bool bActive ) { bid.bActive = bActive; }

  void Ask_CopyFrom( const FeatureSet_Level& ); // shuffle for insertion
  void Ask_CopyTo( FeatureSet_Level& ); // shuffle after deletion
  void Bid_CopyFrom( const FeatureSet_Level& ); // shuffle for insertion
  void Bid_CopyTo( FeatureSet_Level& ); // shuffle after deletion

  void Ask_Quote( const ou::tf::Depth& );
  void Bid_Quote( const ou::tf::Depth& );

  void Ask_IncLimit(  const ou::tf::Depth& depth );
  void Ask_IncMarket( const ou::tf::Depth& depth );
  void Ask_IncCancel( const ou::tf::Depth& depth );

  void Bid_IncLimit(  const ou::tf::Depth& depth );
  void Bid_IncMarket( const ou::tf::Depth& depth );
  void Bid_IncCancel( const ou::tf::Depth& depth );

protected:
private:

  int m_ix; // used as diviser for level number

  FeatureSet_Level* m_pTop;
  FeatureSet_Level* m_pNext;

  FeatureSet_Level& operator=( const FeatureSet_Level& ) = delete;

  void QuotePriceUpdates();
  void QuoteVolumeUpdates();
  void Ask_Aggregate( price_t aggregate ); // aggregate price from previous level
  void Bid_Aggregate( price_t aggregate ); // aggregate price from previous level
  void Ask_Aggregate( volume_t aggregate );  // aggregate volume from previous level
  void Bid_Aggregate( volume_t aggregate );  // aggregate volume from previous level
  void Ask_Diff();
  void Bid_Diff();
  void ImbalanceOnAggregate();
  void Ask_Derivatives( const ou::tf::Depth& );
  void Bid_Derivatives( const ou::tf::Depth& );

  // v7 - common code
  void Intensity( const ou::tf::Depth&, ptime&, double&, double& );

};

} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
