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
 * File:    FeatureSet_Level_impl.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFIQFeed/Level2
 * Created: May 5, 2023 18:19:22
 */

#pragma once

#include <map>
#include <array>
#include <string_view>

#include <boost/preprocessor/tuple/enum.hpp>
#include <boost/preprocessor/tuple/to_array.hpp>

#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/array/elem.hpp>

#include <boost/preprocessor/repetition/repeat.hpp>

#include <boost/fusion/container/vector.hpp>

#include "FeatureSet_Level.hpp"

#define TUPLE_NAMES ( \
    ask.v1.volume \
  , ask.v1.price \
  , ask.v1.aggregateVolume \
  , ask.v1.aggregatePrice \
\
  , ask.v3.diffToTop \
  , ask.v3.diffToAdjacent \
\
  , ask.v4.meanPrice \
  , ask.v4.meanVolume \
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
  , bid.v1.volume \
  , bid.v1.price \
  , bid.v1.aggregateVolume \
  , bid.v1.aggregatePrice \
\
  , bid.v3.diffToTop \
  , bid.v3.diffToAdjacent \
\
  , bid.v4.meanPrice \
  , bid.v4.meanVolume \
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
  , cross.v2.mid \
\
  , cross.v2.imbalanceLvl \
  , cross.v2.imbalanceAgg \
\
  , cross.v5.sumPriceSpreads \
  , cross.v5.sumVolumeSpreads \
)

#define ARRAY_NAMES BOOST_PP_TUPLE_TO_ARRAY( TUPLE_NAMES )
#define ARRAY_NAMES_SIZE BOOST_PP_ARRAY_SIZE( ARRAY_NAMES )

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data

class FeatureSet_Column {
public:

  using vName_t = std::vector<std::string>;
  using rSentinelFlag_t = std::array<bool,ARRAY_NAMES_SIZE>;

  FeatureSet_Column( FeatureSet_Level& );
  ~FeatureSet_Column();

  static void MapColumnNames( const vName_t&, rSentinelFlag_t& );

  void SetSentinel( const rSentinelFlag_t& );

  void Changed( bool& );

protected:
private:

  using mapLuColumn_t = std::map<std::string_view,size_t>;
  static const mapLuColumn_t m_mapLuColumn;

  template<typename Element>
  struct Sentinel {
    bool bSentinel;
    const Element& original;
    Element copy;
    Sentinel( const Element& original_ ): original( original_ ), copy {} {}
  };

  #define FUSION_VECTOR_Element(z,n,data ) \
    BOOST_PP_COMMA_IF(n) \
    Sentinel<decltype(FeatureSet_Level::BOOST_PP_ARRAY_ELEM(n,ARRAY_NAMES ))>

  using fvSentinel_t = boost::fusion::vector<
    BOOST_PP_REPEAT( ARRAY_NAMES_SIZE, FUSION_VECTOR_Element, 0 )
  >;

  fvSentinel_t m_fvSentinel;

  bool m_bSentinelSet;

};

} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
