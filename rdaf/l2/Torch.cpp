/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    Torch.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: 2023/05/15 21:18:40
 */

#include <boost/preprocessor/tuple/enum.hpp>
#include <boost/preprocessor/tuple/to_array.hpp>

#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/array/elem.hpp>

#include <boost/preprocessor/repetition/repeat.hpp>

#include <TFIQFeed/Level2/FeatureSet.hpp>

#include "Torch.hpp"

#define TUPLE_NAMES ( \
    ask.v3.diffToTop \
  , ask.v3.diffToAdjacent \
\
  , ask.v6.dPrice_dt \
  , ask.v6.dVolume_dt \
\
  , ask.v7.intensityLimit \
  , ask.v7.intensityMarket \
  , ask.v7.intensityCancel \
\
  , ask.v8.intensityLimit \
  , ask.v8.intensityMarket \
  , ask.v8.intensityCancel \
\
  , ask.v8.relativeLimit \
  , ask.v8.relativeMarket \
  , ask.v8.relativeCancel \
\
  , ask.v9.accelLimit \
  , ask.v9.accelMarket \
  , ask.v9.accelCancel \
\
  , bid.v3.diffToTop \
  , bid.v3.diffToAdjacent \
\
  , bid.v6.dPrice_dt \
  , bid.v6.dVolume_dt \
\
  , bid.v7.intensityLimit \
  , bid.v7.intensityMarket \
  , bid.v7.intensityCancel \
\
  , bid.v8.intensityLimit \
  , bid.v8.intensityMarket \
  , bid.v8.intensityCancel \
\
  , bid.v8.relativeLimit \
  , bid.v8.relativeMarket \
  , bid.v8.relativeCancel \
\
  , bid.v9.accelLimit \
  , bid.v9.accelMarket \
  , bid.v9.accelCancel \
\
  , cross.v2.spread \
\
  , cross.v2.imbalanceLvl \
\
  , cross.v5.sumPriceSpreads \
  , cross.v5.sumVolumeSpreads \
)

#define ARRAY_NAMES BOOST_PP_TUPLE_TO_ARRAY( TUPLE_NAMES )
#define ARRAY_NAMES_SIZE BOOST_PP_ARRAY_SIZE( ARRAY_NAMES )

namespace Strategy {

Futures_Torch::Futures_Torch() {
}

Futures_Torch::~Futures_Torch() {
}

void Futures_Torch::Accumulate( const ou::tf::iqfeed::l2::FeatureSet& fs ) {
  // 3 levels
  // accumulate values with count

  size_t ix {};
  for ( const ou::tf::iqfeed::l2::FeatureSet::vLevels_t::value_type& vt: fs.FVS() ) {

  }
}

Futures_Torch::Op Futures_Torch::StepModel() {
  Op op( Op::Neutral );

  // calc average from accumulate/count
  // 3 levels
  // maintain 10 minutes x 60 seconds

  return op; // placeholder
}

}