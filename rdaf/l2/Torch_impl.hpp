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
 * File:    Torch_impl.hpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: 2023/05/16 18:00:31
 */

#pragma once

#include <array>

#include <boost/preprocessor/tuple/enum.hpp>
#include <boost/preprocessor/tuple/to_array.hpp>

#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/array/elem.hpp>

#include <boost/preprocessor/repetition/repeat.hpp>

#include <boost/fusion/container/vector.hpp>

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

class Torch_impl {
public:

  Torch_impl( const ou::tf::iqfeed::l2::FeatureSet& );
  ~Torch_impl();

  void Accumulate();
  Torch::Op StepModel( boost::posix_time::ptime );

protected:
private:

  template <typename type>
  struct Accumulator {
    const type& feature;
    double accumulate;
    double count;
    // TODO: convert to exponential moving average?
    //   ema lags less than ma
    Accumulator( const type& feature_ )
    : feature( feature_ )
    , accumulate {}, count {}
    {}
    void Clear() {
      accumulate = 0.0;
      count = 0;
    }
  };

  #define FUSION_VECTOR_Accumulator(z,n,data ) \
    BOOST_PP_COMMA_IF(n) \
    Accumulator<decltype(ou::tf::iqfeed::l2::FeatureSet_Level::BOOST_PP_ARRAY_ELEM(n,ARRAY_NAMES ))>

  using fvAccumulator_t = boost::fusion::vector<
    BOOST_PP_REPEAT( ARRAY_NAMES_SIZE, FUSION_VECTOR_Accumulator, 0 )
  >;

  fvAccumulator_t m_fvAccumulator_l1;
  fvAccumulator_t m_fvAccumulator_l2;
  fvAccumulator_t m_fvAccumulator_l3;

  static const size_t c_nLevels = 3;
  static const size_t c_nTimeSteps = 10 * 60; // seconds

  using rTimeStep_Averages_t = std::array<double, c_nLevels * ARRAY_NAMES_SIZE>;
  using rTimeSteps_t = std::array<rTimeStep_Averages_t, c_nTimeSteps>;

  rTimeSteps_t m_rTimeSteps;

  rTimeSteps_t::size_type m_ixTimeStep; // entry to be filled
  bool m_bTimeStepsFilled;  // ie wrapped

};

} // namespace Strategy