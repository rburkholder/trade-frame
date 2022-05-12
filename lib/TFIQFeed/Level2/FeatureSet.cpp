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
 * Project: lib/TFIQFeed/Level2
 * Created: May 11, 2022 15:24
 */

#include "FeatureSet.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data

FeatureSet::FeatureSet()
: m_nLevels {}
{}

FeatureSet::~FeatureSet()
{}

void FeatureSet::Set( size_t nLevels ) {

  assert( 0 < nLevels );
  assert( 0 == m_nLevels );
  m_nLevels = nLevels;

  m_vLevels.resize( m_nLevels + 1 );  // level 0 not used, levels numbered 1 - 10

  m_vLevels[ 1 ].Set( 1, nullptr, &m_vLevels[ 2 ] );
  for ( int ix = 2; ix < m_nLevels; ix++ ) {
    m_vLevels[ ix ].Set( ix, &m_vLevels[ 1 ], &m_vLevels[ ix + 1 ] );
  }
  m_vLevels[ m_nLevels ].Set( m_nLevels, &m_vLevels[ 1 ], nullptr );

}

void FeatureSet::HandleBookChangesAsk( ou::tf::iqfeed::l2::EOp op, unsigned int ix, const ou::tf::Depth& depth ) {
  if ( 0 != ix ) {
    assert( m_nLevels >= ix );
    switch ( op ) {
      case ou::tf::iqfeed::l2::EOp::Insert:
        if ( m_nLevels > ix ) {
          m_vLevels[ ix + 1 ].Ask_CopyFrom( m_vLevels[ ix ] );
        }
        m_vLevels[ ix ].Ask_Activate( true );
        m_vLevels[ ix ].Ask_Quote( depth );
        break;
      case ou::tf::iqfeed::l2::EOp::Update:
        m_vLevels[ ix ].Ask_Quote( depth );
        break;
      case ou::tf::iqfeed::l2::EOp::Delete:
        if ( m_nLevels > ix ) {
          m_vLevels[ ix + 1 ].Ask_CopyTo( m_vLevels[ ix ] );
        }
        m_vLevels[ m_nLevels ].Ask_Activate( false );
        break;
    }
  }
}

void FeatureSet::HandleBookChangesBid( ou::tf::iqfeed::l2::EOp op, unsigned int ix, const ou::tf::Depth& depth ) {
  if ( 0 != ix ) {
    assert( m_nLevels >= ix );
    switch ( op ) {
      case ou::tf::iqfeed::l2::EOp::Insert:
        if ( m_nLevels > ix ) {
          m_vLevels[ ix + 1 ].Bid_CopyFrom( m_vLevels[ ix ] );
        }
        m_vLevels[ ix ].Bid_Activate( true );
        m_vLevels[ ix ].Bid_Quote( depth );
        break;
      case ou::tf::iqfeed::l2::EOp::Update:
        m_vLevels[ ix ].Bid_Quote( depth );
        break;
      case ou::tf::iqfeed::l2::EOp::Delete:
        if ( m_nLevels > ix ) {
          m_vLevels[ ix + 1 ].Bid_CopyTo( m_vLevels[ ix ] );
        }
        m_vLevels[ m_nLevels ].Bid_Activate( false );
        break;
    }
  }
}

} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
