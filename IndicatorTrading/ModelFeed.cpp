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
 * File:    ModelFeed.cpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: 2022/11/21 13:42:06
 */

#include <TFVuTrading/MarketDepth/PanelTrade.hpp>

#include "ModelFeed.hpp"
#include "InteractiveChart.h"

ModelFeed::ModelFeed( pWatch_t pWatch, size_t nLevels )
: m_pWatchUnderlying( pWatch )
, m_bTriggerFeatureSetDump( false )
, m_dblImbalanceMean {}, m_dblImbalanceSlope {}
, m_pPanelTrade( nullptr )
, m_pInteractiveChart( nullptr )
{
  StartDepthByOrder( nLevels );
}

void ModelFeed::Set( ou::tf::l2::PanelTrade* pPanelTrade ) {
  m_pPanelTrade = pPanelTrade;
}

void ModelFeed::Set( InteractiveChart* pInteractiveChart ) {
  m_pInteractiveChart = pInteractiveChart;
}

void ModelFeed::Connect() {
  if ( m_pDispatch ) {
    assert( m_pDispatch );
    m_pDispatch->Connect();
    assert( m_pWatchUnderlying );
    m_pWatchUnderlying->OnQuote.Add( MakeDelegate( this, &ModelFeed::HandleQuote ) );
    m_pWatchUnderlying->OnTrade.Add( MakeDelegate( this, &ModelFeed::HandleTrade ) );
  }
  else std::cout << "ModelFeed: no dispatch" << std::endl;
}

void ModelFeed::Disconnect() {
  if ( m_pDispatch ) {
    m_pDispatch->Disconnect();
    assert( m_pWatchUnderlying );
    m_pWatchUnderlying->OnQuote.Remove( MakeDelegate( this, &ModelFeed::HandleQuote ) );
    m_pWatchUnderlying->OnTrade.Remove( MakeDelegate( this, &ModelFeed::HandleTrade ) );
  }
}

void ModelFeed::HandleQuote( const ou::tf::Quote& quote ) {
  if ( !quote.IsValid() ) {
    return;
  }

  if ( m_pPanelTrade ) {
    m_pPanelTrade->OnQuote( quote );
  }
}

void ModelFeed::HandleTrade( const ou::tf::Trade& trade ) {
  ou::tf::Trade::price_t price = trade.Price();
  const double mid = m_pWatchUnderlying->LastQuote().Midpoint();
  if ( price >= mid ) {
    m_nMarketOrdersAsk++;
  }
  else {
    m_nMarketOrdersBid++;
  }

  if ( m_pPanelTrade ) {
    m_pPanelTrade->OnTrade( trade );
  }
}

void ModelFeed::FeatureSetDump() {
  m_bTriggerFeatureSetDump = true;
}

void ModelFeed::StartDepthByOrder( size_t nLevels ) { // see AppDoM as reference

  using EState = ou::tf::iqfeed::l2::OrderBased::EState;

  m_OrderBased.Set(
    [this]( ou::tf::iqfeed::l2::EOp op, unsigned int ix, const ou::tf::Depth& depth ){ // fBookChanges_t&& fBid_

      ou::tf::Trade::price_t price( depth.Price() );
      ou::tf::Trade::volume_t volume( depth.Volume() );

      if ( 0 != ix ) {
        if ( m_bTriggerFeatureSetDump ) {
          std::cout << "fs dump (bid) "
            << (int)op
            << "," << ix
            << "," << depth.MsgType()
            << "," << depth.Price() << "," << depth.Volume()
            << "," << depth.Side()
            << std::endl;
          m_FeatureSet.Emit();
        }

        //m_FeatureSet.IntegrityCheck();
        m_FeatureSet.HandleBookChangesBid( op, ix, depth );
        //m_FeatureSet.IntegrityCheck();

        if ( m_bTriggerFeatureSetDump ) {
          m_FeatureSet.Emit();
          m_bTriggerFeatureSetDump = false;
        }
      }

      switch ( m_OrderBased.State() ) {
        case EState::Add:
        case EState::Delete:
          switch ( op ) {
            case ou::tf::iqfeed::l2::EOp::Increase:
            case ou::tf::iqfeed::l2::EOp::Insert:
              if ( 0 != ix ) {
                m_FeatureSet.Bid_IncLimit( ix, depth );
              }
              break;
            case ou::tf::iqfeed::l2::EOp::Decrease:
            case ou::tf::iqfeed::l2::EOp::Delete:
              if ( 1 == ix ) {
                uint32_t nTicks = m_nMarketOrdersBid.load();
                // TODO: does arrival rate of deletions affect overall Market rate?
                if ( 0 == nTicks ) {
                  m_FeatureSet.Bid_IncCancel( 1, depth );
                }
                else {
                  --m_nMarketOrdersBid;
                  m_FeatureSet.Bid_IncMarket( 1, depth );
                }
              }
              else { // 1 < ix
                if ( 0 != ix ) {
                  m_FeatureSet.Bid_IncCancel( ix, depth );
                }
              }
              break;
            default:
              break;
          }
          break;
        case EState::Update:
          // simply a change, no interesting statistics
          break;
        case EState::Clear:
          break;
        case EState::Ready:
          assert( false ); // not allowed
          break;
      }

      m_pPanelTrade->OnQuoteBid( price, volume );
//      m_pPanelSideBySide->OnL2Bid( price, volume, ou::tf::iqfeed::l2::EOp::Delete != op );

      if ( ( 1 == ix ) || ( 2 == ix ) ) { // may need to recalculate at any level change instead
        Imbalance( depth );
      }
    },
    [this]( ou::tf::iqfeed::l2::EOp op, unsigned int ix, const ou::tf::Depth& depth ){ // fBookChanges_t&& fAsk_

      ou::tf::Trade::price_t price( depth.Price() );
      ou::tf::Trade::volume_t volume( depth.Volume() );

      if ( 0 != ix ) {
        if ( m_bTriggerFeatureSetDump ) {
          std::cout << "fs dump (ask) "
            << (int)op
            << "," << ix
            << "," << depth.MsgType()
            << "," << depth.Price() << "," << depth.Volume()
            << "," << depth.Side()
            << std::endl;
          m_FeatureSet.Emit();
        }

        //m_FeatureSet.IntegrityCheck();
        m_FeatureSet.HandleBookChangesAsk( op, ix, depth );
        //m_FeatureSet.IntegrityCheck();

        if ( m_bTriggerFeatureSetDump ) {
          m_FeatureSet.Emit();
          m_bTriggerFeatureSetDump = false;
        }
      }

      switch ( m_OrderBased.State() ) {
        case EState::Add:
        case EState::Delete:
          switch ( op ) {
            case ou::tf::iqfeed::l2::EOp::Increase:
            case ou::tf::iqfeed::l2::EOp::Insert:
              if ( 0 != ix ) {
                m_FeatureSet.Ask_IncLimit( ix, depth );            }
              break;
            case ou::tf::iqfeed::l2::EOp::Decrease:
            case ou::tf::iqfeed::l2::EOp::Delete:
              if ( 1 == ix ) {
                uint32_t nTicks = m_nMarketOrdersAsk.load();
                if ( 0 == nTicks ) {
                  m_FeatureSet.Ask_IncCancel( 1, depth );
                }
                else {
                  --m_nMarketOrdersAsk;
                  m_FeatureSet.Ask_IncMarket( 1, depth );
                }
              }
              else { // 1 < ix
                if ( 0 != ix ) {
                  m_FeatureSet.Ask_IncCancel( ix, depth );
                }
              }
              break;
            default:
              break;
          }
        case EState::Update:
          // simply a change, no interesting statistics
          break;
        case EState::Clear:
          break;
        case EState::Ready:
          assert( false ); // not allowed
          break;
      }

      m_pPanelTrade->OnQuoteAsk( price, volume );
//      m_pPanelSideBySide->OnL2Ask( price, volume, ou::tf::iqfeed::l2::EOp::Delete != op );

      if ( ( 1 == ix ) || ( 2 == ix ) ) { // may need to recalculate at any level change instead
        Imbalance( depth );
      }
    }
  );

  m_pDispatch = std::make_unique<ou::tf::iqfeed::l2::Symbols>(
    [ this, nLevels ](){
      m_FeatureSet.Set( nLevels );  // use this many levels in the order book for feature vector set
      m_pDispatch->Single( true );
      m_pDispatch->WatchAdd(
        m_pWatchUnderlying->GetInstrument()->GetInstrumentName( ou::tf::keytypes::eidProvider_t::EProviderIQF ),
        [this]( const ou::tf::DepthByOrder& depth ){
          m_OrderBased.MarketDepth( depth );
        }
        );
    } );

}

void ModelFeed::Imbalance( const ou::tf::Depth& depth ) {

  static const double w1( 19.0 / 20.0 );
  assert( 1.0 > w1 );
  static const double w2( 1.0 - w1 );

  ou::tf::RunningStats::Stats stats;
  m_FeatureSet.ImbalanceSummary( stats );

  m_dblImbalanceMean = w1 * m_dblImbalanceMean + w2 * stats.meanY;  // exponential moving average
  //m_dblImbalanceSlope = w1 * m_dblImbalanceSlope + w2 * stats.b1;

  //double state = 0.0;
  //if ( ( 0.0 == m_dblImbalanceMean ) || ( 0.0 == m_dblImbalanceSlope ) ) {} // nothing
  //else {
  //  if ( 0.0 < m_dblImbalanceMean ) {
  //    if ( 0.0 < m_dblImbalanceSlope ) state = 1.0;
  //    else state = 2.0;
  //  }
  //  else {
  //    if ( 0.0 < m_dblImbalanceSlope ) state = -1.0;
  //    else state = -2.0;
  //  }
  //}
  //m_ceImbalanceState.Append( depth.DateTime(), state );

  m_pInteractiveChart->UpdateImbalance( depth.DateTime(), stats.meanY, m_dblImbalanceMean );

}

/*
    m_pPanelTrade->SetOnTimer(
      [this](){
        if ( 0 == m_cntLoops ) {
          m_pPanelStatistics->Update( m_valuesStatistics );
          m_valuesStatistics.Zero();
          m_cntLoops = 5;
        }
        else m_cntLoops--;
        for ( const vMA_t::value_type& vt: m_vMA ) {
          m_pPanelTrade->UpdateDynamicIndicator( vt.sName, vt.Latest() );
        }
      });
*/
