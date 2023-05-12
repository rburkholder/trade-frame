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

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>

#include "FeatureSet_Level_impl.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data

#define FUSION_VECTOR_REFERENCES(z,n,data ) \
  BOOST_PP_COMMA_IF(n) \
  Sentinel( level.BOOST_PP_ARRAY_ELEM(n,ARRAY_NAMES ) )

#define MAP_LUCOLUMN_VALUE(z,n,data) \
  BOOST_PP_COMMA_IF(n) \
  { BOOST_PP_STRINGIZE(BOOST_PP_ARRAY_ELEM(n,ARRAY_NAMES)), n }

const FeatureSet_Column::mapLuColumn_t FeatureSet_Column::m_mapLuColumn = {
  BOOST_PP_REPEAT( ARRAY_NAMES_SIZE, MAP_LUCOLUMN_VALUE, 0 )
};

FeatureSet_Column::FeatureSet_Column( FeatureSet_Level& level )
: m_bSentinelSet( false )
, m_fvSentinel(
    BOOST_PP_REPEAT( ARRAY_NAMES_SIZE, FUSION_VECTOR_REFERENCES, 0 )
)
{}

FeatureSet_Column::~FeatureSet_Column() {}

void FeatureSet_Column::MapColumnNames( const vName_t& vName, rSentinelFlag_t& rSentinelFlag ) {

  for ( bool& entry: rSentinelFlag ) entry = false;

  for ( const vName_t::value_type& name: vName ) {
    mapLuColumn_t::const_iterator iter = m_mapLuColumn.find( name );
    if ( m_mapLuColumn.end() == iter ) {
      std::cout << "FeatureSet column " << name << " not found" << std::endl;
    }
    else {
      rSentinelFlag[ iter->second ] = true;
    }
  }
}

void FeatureSet_Column::SetSentinel( const rSentinelFlag_t& r ) {

  assert( !m_bSentinelSet ); // set only once
  m_bSentinelSet = true;

  rSentinelFlag_t::const_iterator iter( r.begin() );

  boost::fusion::for_each(
    m_fvSentinel,
    [&iter]( auto& element ){
      element.bSentinel = *iter;
      iter++;
    } );

}

void FeatureSet_Column::Changed( bool& bChanged ) {
  boost::fusion::for_each(
    m_fvSentinel,
    [&bChanged]( auto& element ){
      if ( element.bSentinel ) {
        if ( element.original != element.copy ) {
          bChanged = true;
          element.copy = element.original;
        }
      }
    } );
}

} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
