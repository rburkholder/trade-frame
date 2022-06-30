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
#include "TFIndicators/RunningStats.h"

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
  assert( 3 <= nLevels );  // need at least 3 levels
  assert( 0 == m_nLevels );  // one time set only
  m_nLevels = nLevels;

  m_vLevels.resize( m_nLevels + 1 );  // level 0 not used, levels numbered 1 - 10

  m_vLevels[ 1 ].Set( 1, nullptr, &m_vLevels[ 2 ] );
  for ( int ix = 2; ix < m_nLevels; ix++ ) {
    m_vLevels[ ix ].Set( ix, &m_vLevels[ 1 ], &m_vLevels[ ix + 1 ] );
  }
  m_vLevels[ m_nLevels ].Set( m_nLevels, &m_vLevels[ 1 ], nullptr );

}

void FeatureSet::HandleBookChangesAsk( ou::tf::iqfeed::l2::EOp op, unsigned int ix, const ou::tf::Depth& depth ) {
  if ( ( 0 == ix ) || ( m_nLevels < ix ) ) {
    assert( 0 != ix );
    assert( m_nLevels > ix  );
  }
  else {
    switch ( op ) {
      case ou::tf::iqfeed::l2::EOp::Insert:
        if ( m_nLevels > ix ) {
          m_vLevels[ ix + 1 ].Ask_CopyFrom( m_vLevels[ ix ] );
        }
        m_vLevels[ ix ].Ask_Activate( true );
        m_vLevels[ ix ].Ask_Quote( depth );
        break;
      case ou::tf::iqfeed::l2::EOp::Increase:
      case ou::tf::iqfeed::l2::EOp::Decrease:
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
  if ( ( 0 == ix ) || ( m_nLevels < ix ) ) {
    assert( 0 != ix );
    assert( m_nLevels > ix  );
  }
  else {
    switch ( op ) {
      case ou::tf::iqfeed::l2::EOp::Insert:
        if ( m_nLevels > ix ) {
          m_vLevels[ ix + 1 ].Bid_CopyFrom( m_vLevels[ ix ] );
        }
        m_vLevels[ ix ].Bid_Activate( true );
        m_vLevels[ ix ].Bid_Quote( depth );
        break;
      case ou::tf::iqfeed::l2::EOp::Increase:
      case ou::tf::iqfeed::l2::EOp::Decrease:
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

// v7 Ask
void FeatureSet::Ask_IncLimit(  unsigned int ix, const ou::tf::Depth& depth ) {
  m_vLevels[ ix ].Ask_IncLimit( depth );
}

void FeatureSet::Ask_IncMarket( unsigned int ix, const ou::tf::Depth& depth ) {
  m_vLevels[ ix ].Ask_IncMarket( depth );
}

void FeatureSet::Ask_IncCancel( unsigned int ix, const ou::tf::Depth& depth ) {
  m_vLevels[ ix ].Ask_IncCancel( depth );
}

// v7 Bid
void FeatureSet::Bid_IncLimit(  unsigned int ix, const ou::tf::Depth& depth ) {
  m_vLevels[ ix ].Bid_IncLimit( depth );
}

void FeatureSet::Bid_IncMarket( unsigned int ix, const ou::tf::Depth& depth ) {
  m_vLevels[ ix ].Bid_IncMarket( depth );
}

void FeatureSet::Bid_IncCancel( unsigned int ix, const ou::tf::Depth& depth ) {
  m_vLevels[ ix ].Bid_IncCancel( depth );
}

void FeatureSet::ImbalanceSummary( ou::tf::RunningStats::Stats& stats ) const {
  double ix( 1.0 );
  ou::tf::RunningStats rs;
  for ( const vLevels_t::value_type& vt: m_vLevels ) {
    rs.Add( ix, vt.cross.v2.imbalanceAgg );  // not sure which of the two are most appropriate
    //rs.Add( ix, vt.cross.v2.imbalanceLvl );
    ix += 1.0;
  }
  rs.CalcStats( stats );
}

void FeatureSet::Emit() const {
  for ( const vLevels_t::value_type& vt: m_vLevels ) {
    vt.Emit();
  }
}

bool FeatureSet::IntegrityCheck() const {

  bool bFine( true );

  double bidPrice {}, askPrice {};
  bool bidActive( false ), askActive( false );

  enum ELevel { Zero, One, Remaining } level( Zero );

  for ( const vLevels_t::value_type& vt: m_vLevels ) {
    switch ( level ) {
      case ELevel::Zero:
        assert( vt.ask.v1.price == 0.0 );
        assert( !vt.ask.bActive );
        assert( vt.bid.v1.price == 0.0 );
        assert( !vt.bid.bActive );
        level = ELevel::One;
        break;
      case ELevel::One:
        if ( vt.ask.bActive ) {
          askActive = true;
          askPrice = vt.ask.v1.price;
        }
        if ( vt.bid.bActive ) {
          bidActive = true;
          bidPrice = vt.bid.v1.price;
        }
        level = ELevel::Remaining;
        break;
      case ELevel::Remaining:
        if ( vt.ask.bActive ) {
          assert( askActive ); // prior needs to be active
          assert( askPrice < vt.ask.v1.price );
          askPrice = vt.ask.v1.price;
          askActive = true;
        }
        if ( vt.bid.bActive ) {
          assert( bidActive ); // prior needs to be active
          assert( bidPrice > vt.bid.v1.price );
          bidPrice = vt.bid.v1.price;
          bidActive = true;
        }
        break;
    }
  }
  return bFine;
}

} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
