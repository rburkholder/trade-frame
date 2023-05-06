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
 * File:    FeatureSet_Level_impl.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFIQFeed/Level2
 * Created: May 5, 2023 18:19:22
 */

#include "FeatureSet_Level_impl.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data

#define FUSION_VECTOR_REFERENCES(z,n,data ) \
  BOOST_PP_COMMA_IF(n) \
  level.BOOST_PP_ARRAY_ELEM(n,ARRAY_NAMES )

#define QUOTED( VALUE ) #VALUE

#define MAP_LUCOLUMN_VALUE(z,n,data) \
  BOOST_PP_COMMA_IF(n) \
  { QUOTED(BOOST_PP_ARRAY_ELEM(n,ARRAY_NAMES)), n }

const FeatureSet_Column::mapLuColumn_t FeatureSet_Column::m_mapLuColumn = {
  BOOST_PP_REPEAT( ARRAY_NAMES_SIZE, MAP_LUCOLUMN_VALUE, 0 )
};

FeatureSet_Column::FeatureSet_Column( FeatureSet_Level& level )
: m_vColumns(
    BOOST_PP_REPEAT( ARRAY_NAMES_SIZE, FUSION_VECTOR_REFERENCES, 0 )
)
{
  using test_t = boost::fusion::vector<
    decltype( level.ask.v1.aggregatePrice )&
  , decltype( level.ask.v4.meanVolume )&
  >;

  test_t test( level.ask.v1.aggregatePrice, level.ask.v4.meanVolume );
}

} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
