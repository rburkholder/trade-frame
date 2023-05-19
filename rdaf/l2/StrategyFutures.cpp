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
 * File:    StrategyFutures.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: December 9, 2022  16:18:19
 */

// https://indico.cern.ch/event/697389/contributions/3062036/attachments/1712790/2761904/Support_for_SIMD_Vectorization_in_ROOT_ROOT_Workshop_2018.pdf
// https://www.intel.com/content/www/us/en/develop/documentation/advisor-user-guide/top/analyze-cpu-roofline.html
// https://stackoverflow.com/questions/52653025/why-is-march-native-used-so-rarely

#include <array>
#include <chrono>

// no longer use iostream:  std::cout has a multithread contention problem
//   due to it being captured to the gui, and is not thread safe
#include <boost/log/trivial.hpp>

#include <boost/lexical_cast.hpp>

#include <cmath>

#if RDAF
#include <rdaf/TH2.h>
#include <rdaf/TTree.h>
#include <rdaf/TFile.h>
#endif

#include <OUCharting/ChartDataView.h>

#include <TFVuTrading/TreeItem.hpp>

#include "StrategyFutures.hpp"

using pWatch_t = ou::tf::Watch::pWatch_t;

namespace {
  static const unsigned int max_ix = 10; // TODO need to obtain from elsewhere & sync with Symbols
  static const int k_up = 80;
  static const int k_lo = 20;
  static const boost::posix_time::time_duration filter_stoch( 0, 0, 1 );
}

namespace Strategy {

Futures::Futures(
  const ou::tf::config::symbol_t& config
, TreeItem* pTreeItem
, fTelegram_t&& fTelegram
//, pFile_t pFile
//, pFile_t pFileUtility
)
: Base( config, pTreeItem )
, ou::tf::DailyTradeTimeFrame<Futures>()
, m_fTelegram( std::move( fTelegram ) )
//, m_pFile( pFile )
//, m_pFileUtility( pFileUtility )
, m_stateTrade( EStateTrade::Init )
, m_ceLongEntry( ou::ChartEntryShape::EShape::Long, ou::Colour::Blue )
//, m_ceLongFill( ou::ChartEntryShape::EShape::FillLong, ou::Colour::Blue )
, m_ceLongExit( ou::ChartEntryShape::EShape::LongStop, ou::Colour::Blue )
, m_ceShortEntry( ou::ChartEntryShape::EShape::Short, ou::Colour::Red )
//, m_ceShortFill( ou::ChartEntryShape::EShape::FillShort, ou::Colour::Red )
, m_ceShortExit( ou::ChartEntryShape::EShape::ShortStop, ou::Colour::Red )
, m_dblProfitMax {}, m_dblUnRealized {}, m_dblProfitMin {}
, m_dblStopDeltaProposed {}
, m_dblStopActiveDelta {}, m_dblStopActiveActual {}
, m_bfQuotes01Sec( 1 )
, m_nEmitted {}
, m_nEmitSuppressed {}
{
//  assert( m_pFile );
//  assert( m_pFileUtility );

  m_cemZero.AddMark( 0, ou::Colour::Black,  "0" );

  m_ceProfitUnRealized.SetColour( ou::Colour::Purple );
  m_ceProfitRealized.SetColour( ou::Colour::DeepSkyBlue );
  m_ceCommissionsPaid.SetColour( ou::Colour::DarkGreen );
  m_ceProfit.SetColour( ou::Colour::Green );

  m_cemStochastic.AddMark(  100, ou::Colour::Black,    "" );
  m_cemStochastic.AddMark( k_up, ou::Colour::Red,   boost::lexical_cast<std::string>( k_up ) + "%" );
  m_cemStochastic.AddMark(   50, ou::Colour::Green, "50%" );
  m_cemStochastic.AddMark( k_lo, ou::Colour::Blue,  boost::lexical_cast<std::string>( k_lo ) + "%" );
  m_cemStochastic.AddMark(    0, ou::Colour::Black,    "" );

  m_ceRelativeMA1.SetName( "ma0-ma3" );
  m_ceRelativeMA2.SetName( "ma1-ma3" );
  m_ceRelativeMA3.SetName( "ma2-ma3" );

  m_ceProfitUnRealized.SetName( "UnRealized" );
  m_ceProfitRealized.SetName( "Realized" );
  m_ceCommissionsPaid.SetName( "Commissions" );
  m_ceProfit.SetName( "Profit" );

  //m_ceExecutionTime.SetName( "Execution Time" );

  m_bfQuotes01Sec.SetOnBarComplete( MakeDelegate( this, &Futures::HandleBarQuotes01Sec ) );

  m_cdMarketDepthAsk.SetName( "MarketDepth Ask" );
  m_cdMarketDepthAsk.SetColour( ou::Colour::Red );

  m_cdMarketDepthBid.SetName( "MarketDepth Bid" );
  m_cdMarketDepthBid.SetColour( ou::Colour::Blue );
#if FVS
  m_ceFVS_Ask_Lvl1RelLmt.SetName( "AskRelLmit" );
  m_ceFVS_Ask_Lvl1RelMkt.SetName( "AskRelMkt" );
  m_ceFVS_Ask_Lvl1RelCncl.SetName( "AskRelCncl" );

  m_ceFVS_Ask_Lvl1RelLmt.SetColour( ou::Colour::DeepSkyBlue );
  m_ceFVS_Ask_Lvl1RelMkt.SetColour( ou::Colour::Green );
  m_ceFVS_Ask_Lvl1RelCncl.SetColour( ou::Colour::Purple );

  m_ceFVS_Bid_Lvl1RelLmt.SetName( "BidRelLmit" );
  m_ceFVS_Bid_Lvl1RelMkt.SetName( "BidRelMkt" );
  m_ceFVS_Bid_Lvl1RelCncl.SetName( "BidRelCncl" );

  m_ceFVS_Bid_Lvl1RelLmt.SetColour( ou::Colour::DeepSkyBlue );
  m_ceFVS_Bid_Lvl1RelMkt.SetColour( ou::Colour::Green );
  m_ceFVS_Bid_Lvl1RelCncl.SetColour( ou::Colour::Purple );
#endif
}

Futures::~Futures() {
  m_pOrderBased.reset();
  Clear();
}

void Futures::SetupChart() {

  m_cdv.Add( EChartSlot::Price, &m_ceLongEntry );
  //m_cdv.Add( EChartSlot::Price, &m_ceLongFill );
  m_cdv.Add( EChartSlot::Price, &m_ceLongExit );
  m_cdv.Add( EChartSlot::Price, &m_ceShortEntry );
  //m_cdv.Add( EChartSlot::Price, &m_ceShortFill );
  m_cdv.Add( EChartSlot::Price, &m_ceShortExit );

  //m_cdv.Add( EChartSlot::Price, &m_ceEhlersLoPassFilter );

  m_cdv.Add( EChartSlot::MASlope, &m_cemZero );

  for ( vMovingAverageSlope_t::value_type& mas: m_vMovingAverageSlope ) {
    mas.AddToView( m_cdv, EChartSlot::Price, EChartSlot::SD, EChartSlot::MASlope );
  }

  m_cdv.Add( EChartSlot::Stoch, &m_cemStochastic );

  for ( vStochastic_t::value_type& vt: m_vStochastic ) {
    vt->AddToView( m_cdv, EChartSlot::Price, EChartSlot::Stoch );
  }

  m_cdv.Add( EChartSlot::MA, &m_cemZero );
  m_cdv.Add( EChartSlot::MA, &m_ceRelativeMA1 );
  m_cdv.Add( EChartSlot::MA, &m_ceRelativeMA2 );
  m_cdv.Add( EChartSlot::MA, &m_ceRelativeMA3 );

  m_cdv.Add( EChartSlot::ImbalanceMean, &m_cemZero );

  m_ceImbalanceRawMean.SetName( "imbalance mean" );
  m_ceImbalanceRawMean.SetColour( ou::Colour::LightGreen );
  m_cdv.Add( EChartSlot::ImbalanceMean, &m_ceImbalanceRawMean );

  m_ceImbalanceSmoothMean.SetColour( ou::Colour::DarkGreen );
  m_cdv.Add( EChartSlot::ImbalanceMean, &m_ceImbalanceSmoothMean );

  m_cdv.Add( EChartSlot::PL1, &m_cemZero );

  m_cdv.Add( EChartSlot::PL1, &m_ceProfitUnRealized );
  m_cdv.Add( EChartSlot::PL2, &m_ceProfitRealized );
  m_cdv.Add( EChartSlot::PL2, &m_ceCommissionsPaid );
  m_cdv.Add( EChartSlot::PL2, &m_ceProfit );

#if FVS
  m_cdv.Add( EChartSlot::FVS_v8_rel, &m_cemZero );

  m_cdv.Add( EChartSlot::FVS_v8_rel, & m_ceFVS_Ask_Lvl1RelMkt );
  m_cdv.Add( EChartSlot::FVS_v8_rel, & m_ceFVS_Ask_Lvl1RelCncl );
  m_cdv.Add( EChartSlot::FVS_v8_rel, & m_ceFVS_Ask_Lvl1RelLmt );

  m_cdv.Add( EChartSlot::FVS_v8_rel, & m_ceFVS_Bid_Lvl1RelLmt );
  m_cdv.Add( EChartSlot::FVS_v8_rel, & m_ceFVS_Bid_Lvl1RelCncl );
  m_cdv.Add( EChartSlot::FVS_v8_rel, & m_ceFVS_Bid_Lvl1RelMkt );
#endif
  //m_cdv.Add( EChartSlot::ET, &m_ceExecutionTime );

  //m_cdv.Add( EChartSlot::MarketDepth, &m_cdMarketDepthAsk );
  //m_cdv.Add( EChartSlot::MarketDepth, &m_cdMarketDepthBid );

}

void Futures::SetPosition( pPosition_t pPosition ) {

  Clear();

  Base::SetPosition( pPosition );

  pWatch_t pWatch = m_pPosition->GetWatch();

  assert( 0 < m_config.nPeriodWidth );
  time_duration td = time_duration( 0, 0, m_config.nPeriodWidth );

  //m_rHiPass[0].Init( m_config.nPeriodWidth, ou::Colour::Coral, "HP1" );
  //m_rHiPass[1].Init( m_config.nPeriodWidth, ou::Colour::Gold, "HP2" );
//  m_rHiPass[2].Init( m_config.nPeriodWidth, ou::Colour::Brown, "HP3" );
  //m_rHiPass[3].Init( m_config.nMA1Periods, ou::Colour::Green, "HP4" );

  // stochastic

  m_vStochastic.emplace_back( std::make_unique<Stochastic>( "1", m_quotes, m_config.nStochastic1Periods, td, ou::Colour::DeepSkyBlue ) );
  m_vStochastic.emplace_back( std::make_unique<Stochastic>( "2", m_quotes, m_config.nStochastic2Periods, td, ou::Colour::DodgerBlue ) );  // is dark: MediumSlateBlue; MediumAquamarine is greenish; MediumPurple is dark; Purple is dark
  m_vStochastic.emplace_back( std::make_unique<Stochastic>( "3", m_quotes, m_config.nStochastic3Periods, td, ou::Colour::MediumSlateBlue ) ); // no MediumTurquoise, maybe Indigo

  // moving average

  using vMAPeriods_t = std::vector<int>;
  vMAPeriods_t vMAPeriods;

  vMAPeriods.push_back( m_config.nMA1Periods );
  vMAPeriods.push_back( m_config.nMA2Periods );
  vMAPeriods.push_back( m_config.nMA3Periods );

  assert( 3 == vMAPeriods.size() );
  for ( vMAPeriods_t::value_type value: vMAPeriods ) {
    assert( 0 < value );
  }

  m_vMovingAverageSlope.emplace_back( ou::tf::MovingAverageSlope( m_quotes,            1,  td, ou::Colour::Green, "ma0" ) ); // for ehlers
  m_vMovingAverageSlope.emplace_back( ou::tf::MovingAverageSlope( m_quotes, vMAPeriods[0], td, ou::Colour::Brown, "ma1" ) );
  m_vMovingAverageSlope.emplace_back( ou::tf::MovingAverageSlope( m_quotes, vMAPeriods[1], td, ou::Colour::Coral, "ma2" ) );
  m_vMovingAverageSlope.emplace_back( ou::tf::MovingAverageSlope( m_quotes, vMAPeriods[2], td, ou::Colour::Gold,  "ma3" ) );

  m_ceRelativeMA1.SetColour( ou::Colour::Green );
  m_ceRelativeMA2.SetColour( ou::Colour::Brown );
  m_ceRelativeMA3.SetColour( ou::Colour::Coral );

  SetupChart(); // comes after stochastic initialization

#if RDAF
  InitRdaf();
#endif

  pWatch->RecordSeries( false ); // use Collector for keeping data
  pWatch->OnQuote.Add( MakeDelegate( this, &Futures::HandleQuote ) );
  pWatch->OnTrade.Add( MakeDelegate( this, &Futures::HandleTrade ) );

  switch ( m_config.eFeed ) {
    case ou::tf::config::symbol_t::EFeed::L1:
      break;
    case ou::tf::config::symbol_t::EFeed::L2M:  // L2 via MarketMaker (nasdaq equities)
      pWatch->OnDepthByMM.Add( MakeDelegate( this, &Futures::HandleDepthByMM ) );

      m_pMarketMaker = ou::tf::iqfeed::l2::MarketMaker::Factory();
      m_pMarketMaker->Set(
        [this]( double price, int volume, bool bAdd ){ // fVolumeAtPrice_t&& fBid_
          HandleUpdateL2Bid( price, volume, bAdd );
        },
        [this]( double price, int volume, bool bAdd ){ // fVolumeAtPrice_t&& fAsk_
          HandleUpdateL2Ask( price, volume, bAdd );
        }
      );
      break;
    case ou::tf::config::symbol_t::EFeed::L2O:  // L2 via OrderBased (CME/ICE futures)
      StartDepthByOrder();
      break;
  }

  if ( m_config.bTradable ) {}
  else {
    BOOST_LOG_TRIVIAL(info) << "Strategy::SetPosition " << m_config.sSymbol_IQFeed << ": no trading";
    m_stateTrade = EStateTrade::NoTrade;
  }

}

void Futures::FVSStreamStart( const std::string& sPath ) {

  if ( m_config.bEmitFVS ) {

    bool bOpen( false );

    if ( sPath.empty() ) {
      if ( m_streamFVS.is_open() ) {
        m_streamFVS.close();
      }
      m_sFVSPath.clear();
    }
    else {
      if ( sPath == m_sFVSPath ) {
        if ( m_streamFVS.is_open() ) {} // leave as is
        else bOpen = true;
      }
      else {
        if ( m_streamFVS.is_open() ) {
          m_streamFVS.close();
        }
        m_sFVSPath = sPath;
        bOpen = true;
      }
    }

    if ( bOpen ) {
      m_streamFVS.open( m_sFVSPath, std::ios_base::trunc );
      assert( m_streamFVS.is_open() );
      m_FeatureSet.Set( m_config.vSentinel );
      m_streamFVS << "datetime," << m_FeatureSet.Header() << std::endl;
    }
  }
}

void Futures::FVSStreamStop( int ) {
  m_streamFVS.close();

  BOOST_LOG_TRIVIAL(info)
    << "Emission stats: "
    << m_nEmitted << " emitted, "
    << m_nEmitSuppressed << " suppressed"
    ;
}

// from IndicatorTrading/FeedModel.cpp
void Futures::StartDepthByOrder() {

  using EState = ou::tf::iqfeed::l2::OrderBased::EState;

  m_FeatureSet.Set( 10 );
  m_pTorch = std::make_unique<Torch>( m_config.sTorchModelPath, m_FeatureSet );

  m_pOrderBased = ou::tf::iqfeed::l2::OrderBased::Factory();

  m_pOrderBased->Set(
    [this]( ou::tf::iqfeed::l2::EOp op, unsigned int ix, const ou::tf::Depth& depth ){ // fBookChanges_t&& fBid_

      ptime dt( depth.DateTime() );

      ou::tf::Trade::price_t price( depth.Price() );
      ou::tf::Trade::volume_t volume( depth.Volume() );

      if ( 0 != ix ) {
        //m_FeatureSet.IntegrityCheck();
        m_FeatureSet.HandleBookChangesBid( op, ix, depth );
        //m_FeatureSet.IntegrityCheck();
      }

      switch ( m_pOrderBased->State() ) {
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

      if ( ( 1 == ix ) || ( 2 == ix ) ) { // may need to recalculate at any level change instead
        Imbalance( depth );
        if ( 1 == ix ) {
#if FVS
          m_ceFVS_Bid_Lvl1RelLmt.Append( dt, -m_FeatureSet.FVS()[ 1 ].bid.v8.relativeLimit );
          m_ceFVS_Bid_Lvl1RelMkt.Append( dt, -m_FeatureSet.FVS()[ 1 ].bid.v8.relativeMarket );
          m_ceFVS_Bid_Lvl1RelCncl.Append( dt, -m_FeatureSet.FVS()[ 1 ].bid.v8.relativeCancel );
#endif
        }
      }

      bool bChanged( false );
      m_FeatureSet.Changed( bChanged );

      if ( bChanged ) {
        m_nEmitted++;
        m_pTorch->Accumulate();

        if ( m_config.bEmitFVS ) {
          if ( m_streamFVS.is_open() ) {
              m_streamFVS
                << boost::posix_time::to_iso_string( depth.DateTime() )
                << ',' << m_FeatureSet
                << std::endl;
          }
        }
      }
      else {
        m_nEmitSuppressed++;
      }

    },
    [this]( ou::tf::iqfeed::l2::EOp op, unsigned int ix, const ou::tf::Depth& depth ){ // fBookChanges_t&& fAsk_

      ptime dt( depth.DateTime() );

      ou::tf::Trade::price_t price( depth.Price() );
      ou::tf::Trade::volume_t volume( depth.Volume() );

      if ( 0 != ix ) {
        //m_FeatureSet.IntegrityCheck();
        m_FeatureSet.HandleBookChangesAsk( op, ix, depth );
        //m_FeatureSet.IntegrityCheck();
      }

      switch ( m_pOrderBased->State() ) {
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

      if ( ( 1 == ix ) || ( 2 == ix ) ) { // may need to recalculate at any level change instead
        Imbalance( depth );
        if ( 1 == ix ) {
#if FVS
          m_ceFVS_Ask_Lvl1RelLmt.Append( dt, +m_FeatureSet.FVS()[ 1 ].ask.v8.relativeLimit );
          m_ceFVS_Ask_Lvl1RelMkt.Append( dt, +m_FeatureSet.FVS()[ 1 ].ask.v8.relativeMarket );
          m_ceFVS_Ask_Lvl1RelCncl.Append( dt, +m_FeatureSet.FVS()[ 1 ].ask.v8.relativeCancel );
#endif
        }
      }

      bool bChanged( false );
      m_FeatureSet.Changed( bChanged );

      if ( bChanged ) {
        m_nEmitted++;
        m_pTorch->Accumulate();

        if ( m_config.bEmitFVS ) {
          if ( m_streamFVS.is_open() ) {
              m_streamFVS
                << boost::posix_time::to_iso_string( depth.DateTime() )
                << ',' << m_FeatureSet
                << std::endl;
          }
        }
      }
      else {
        m_nEmitSuppressed++;
      }

    }
  );

  pWatch_t pWatch = m_pPosition->GetWatch();
  pWatch->OnDepthByOrder.Add( MakeDelegate( this, &Futures::HandleDepthByOrder ) );

}

void Futures::Imbalance( const ou::tf::Depth& depth ) {
  // from IndicatorTrading/FeedModel.cpp

  static const double w1( 19.0 / 20.0 );
  assert( 1.0 > w1 );
  static const double w2( 1.0 - w1 );

  ou::tf::linear::Stats stats;
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

  //m_pInteractiveChart->UpdateImbalance( depth.DateTime(), stats.meanY, m_dblImbalanceMean );

  boost::posix_time::ptime dt( depth.DateTime() );

  m_ceImbalanceRawMean.Append( dt, stats.meanY );
  //m_ceImbalanceRawB1.Append( depth.DateTime(), stats.b1 );

  m_ceImbalanceSmoothMean.Append( dt, m_dblImbalanceMean );
  //m_ceImbalanceSmoothB1.Append( depth.DateTime(), m_dblImbalanceSlope );

}

#if RDAF
void Strategy::LoadHistory( TClass* tc ) {
  BOOST_LOG_TRIVIAL(info) << "  load: " << tc->GetName();

  if ( 0 == strcmp( ( m_config.sSymbol_IQFeed + "_quotes" ).c_str(), tc->GetName() ) ) {
    TTree* pQuotes = dynamic_cast<TTree*>( tc );
  }

  if ( 0 == strcmp( ( m_config.sSymbol_IQFeed + "_trades" ).c_str(), tc->GetName() ) ) {
    TTree* pTrades = dynamic_cast<TTree*>( tc );
  }

  if ( 0 == strcmp( ( m_config.sSymbol_IQFeed + "_h1" ).c_str(), tc->GetName() ) ) {
    TH2D* pH1 = dynamic_cast<TH2D*>( tc );
  }
}
#endif

void Futures::Clear() {

  m_vStochastic.clear();

  using pProvider_t = ou::tf::Watch::pProvider_t;
  if  ( m_pPosition ) {
    pWatch_t pWatch = m_pPosition->GetWatch();
    pProvider_t pDataProvider = pWatch->GetProvider();
    switch ( m_config.eFeed ) {
      case ou::tf::config::symbol_t::EFeed::L1:
        break;
      case ou::tf::config::symbol_t::EFeed::L2M:
        if ( ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderSimulator == pDataProvider->ID() ) {
          m_pMarketMaker.reset();
          pWatch->OnDepthByMM.Remove( MakeDelegate( this, &Futures::HandleDepthByMM ) );
        }
        break;
      case ou::tf::config::symbol_t::EFeed::L2O:
        break;
    }

    pWatch->OnQuote.Remove( MakeDelegate( this, &Futures::HandleQuote ) );
    pWatch->OnTrade.Remove( MakeDelegate( this, &Futures::HandleTrade ) );

    m_cdv.Clear();
    //m_pPosition.reset(); // need to fix relative to thread
  }
}

void Futures::InitRdaf() {
#if RDAF
  pWatch_t pWatch = m_pPosition->GetWatch();
  const std::string& sSymbol( pWatch->GetInstrumentName() );

  m_pTreeQuote = std::make_shared<TTree>(
    ( sSymbol + "_quotes" ).c_str(), ( sSymbol + " quotes" ).c_str()
  );
  m_pTreeQuote->Branch( "quote", &m_branchQuote, "time/D:ask/D:askvol/l:bid/D:bidvol/l" );
  if ( !m_pTreeQuote ) {
    BOOST_LOG_TRIVIAL(error) << "problems m_pTreeQuote";
  }
  m_pTreeQuote->SetDirectory( m_pFile.get() );

  m_pTreeTrade = std::make_shared<TTree>(
    ( sSymbol + "_trades" ).c_str(), ( sSymbol + " trades" ).c_str()
  );
  m_pTreeTrade->Branch( "trade", &m_branchTrade, "time/D:price/D:vol/l:direction/L" );
  if ( !m_pTreeTrade ) {
    BOOST_LOG_TRIVIAL(error) << "problems m_pTreeTrade";
  }
  m_pTreeTrade->SetDirectory( m_pFile.get() );

  m_pHistVolume = std::make_shared<TH2D>(
    ( sSymbol + "_h1" ).c_str(), ( sSymbol + " Volume Histogram" ).c_str(),
    m_config.nPriceBins, m_config.dblPriceLower, m_config.dblPriceUpper,
    m_config.nTimeBins, m_config.dblTimeLower, m_config.dblTimeUpper
  );
  if ( !m_pHistVolume ) {
    BOOST_LOG_TRIVIAL(error) << "problems history";
  }
  m_pHistVolume->SetDirectory( m_pFile.get() );

  m_pHistVolumeDemo = std::make_shared<TH2D>(
    ( sSymbol + "_h1_demo" ).c_str(), ( sSymbol + " Volume Histogram" ).c_str(),
    m_config.nPriceBins, m_config.dblPriceLower, m_config.dblPriceUpper,
    m_config.nTimeBins, m_config.dblTimeLower, m_config.dblTimeUpper
  );
  if ( !m_pHistVolumeDemo ) {
    BOOST_LOG_TRIVIAL(error) << "problems history";
  }
  m_pHistVolumeDemo->SetDirectory( m_pFileUtility.get() );
#endif
}

// TODO:
//   confirm if quote comes before or after the related l2 change
//   are there top of book l2 changes not reflected as l1 quote changes?
//   use top of book changes instead of using quote as event?

void Futures::HandleQuote( const ou::tf::Quote& quote ) {

  ptime dt( quote.DateTime() );

  m_ceQuoteAsk.Append( dt, quote.Ask() );
  m_ceQuoteBid.Append( dt, quote.Bid() );

  m_quote = quote;
  m_quotes.Append( quote );

  TimeTick( quote );

  m_bfQuotes01Sec.Add( dt, quote.Midpoint(), 1 ); // provides a 1 sec pulse for checking the algorithm

}

void Futures::HandleTrade( const ou::tf::Trade& trade ) {

  ptime dt( trade.DateTime() );

  m_ceTrade.Append( dt, trade.Price() );
  m_ceVolume.Append( dt, trade.Volume() );

  const double mid = m_quote.Midpoint();
  const ou::tf::Trade::price_t price = trade.Price();

  if ( price >= mid ) {
    m_nMarketOrdersAsk++;
  }
  else {
    m_nMarketOrdersBid++;
  }

#if RDAF
  if ( m_pTreeTrade ) { // wait for initialization in thread to start
    std::time_t nTime = boost::posix_time::to_time_t( trade.DateTime() );
    m_branchTrade.time = (double)nTime / 1000.0;
    m_branchTrade.price = price;
    m_branchTrade.vol = volume;
    if ( mid == price ) {
      m_branchTrade.direction = 0;
    }
    else {
      m_branchTrade.direction = ( mid < price ) ? volume : -volume;
    }

    m_pTreeTrade->Fill();
  }

  if ( m_pHistVolume ) {
    m_pHistVolume->Fill( trade.Price(), m_branchTrade.time,  trade.Volume() );
  }

  if ( m_pHistVolumeDemo ) {
    m_pHistVolumeDemo->Fill( trade.Price(), m_branchTrade.time,  trade.Volume() );
  }
#endif

}

void Futures::HandleDepthByMM( const ou::tf::DepthByMM& depth ) {

  assert( m_pMarketMaker );
  m_pMarketMaker->MarketDepth( depth );

  if ( '4' == depth.MsgType() ) {
    switch ( depth.Side() ) {
      case 'A':
        m_cdMarketDepthAsk.Append( depth.DateTime(), depth.Price() );
        break;
      case 'B':
        m_cdMarketDepthBid.Append( depth.DateTime(), depth.Price() );
        break;
    }
  }

}

void Futures::HandleDepthByOrder( const ou::tf::DepthByOrder& depth ) {
  m_pOrderBased->MarketDepth( depth );
}

// l2 market maker only
void Futures::HandleUpdateL2Ask( price_t price, volume_t volume, bool bAdd ) {
}

// l2 market maker only
void Futures::HandleUpdateL2Bid( price_t price, volume_t volume, bool bAdd ) {
}

void Futures::HandleBarQuotes01Sec( const ou::tf::Bar& bar ) {

  double dblRealized, dblCommissionsPaid, dblTotal;

  m_pPosition->QueryStats( m_dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );

  m_ceProfitUnRealized.Append( bar.DateTime(), m_dblUnRealized );
  m_ceProfitRealized.Append( bar.DateTime(), dblRealized );
  m_ceCommissionsPaid.Append( bar.DateTime(), dblCommissionsPaid );
  m_ceProfit.Append( bar.DateTime(), dblTotal );

  if ( m_dblUnRealized > m_dblProfitMax ) m_dblProfitMax = m_dblUnRealized;
  if ( m_dblUnRealized < m_dblProfitMin ) m_dblProfitMin = m_dblUnRealized;

  TimeTick( bar );
}

/*
  // template to submit GTC limit order (for Interactive Brokers)
  // strip off fractional seconds
  boost::posix_time::ptime dtQuote
    = quote.DateTime()
    - boost::posix_time::time_duration( 0, 0, 0, quote.DateTime( ).time_of_day().fractional_seconds() );

  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, quantity, price );

  m_pOrder->SetGoodTillDate( dtQuote + boost::posix_time::seconds( 30 ) );
  m_pOrder->SetTimeInForce( ou::tf::TimeInForce::GoodTillDate );
*/

void Futures::EnterLong( const ou::tf::Quote& quote ) { // limit orders, in real, will need to be normalized
  double dblMidPoint( quote.Midpoint() );
  //assert( nullptr == m_pOrderPending.get() );
  m_dblProfitMax = m_dblUnRealized = m_dblProfitMin = 0.0;
  //pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
  pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 1, m_quote.Bid() );
  assert( pOrder );
  pOrder->SetSignalPrice( dblMidPoint );
  pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Futures::HandleOrderCancelled ) );
  pOrder->OnOrderFilled.Add( MakeDelegate( this, &Futures::HandleOrderFilled ) );
  m_ceLongEntry.AddLabel( quote.DateTime(), dblMidPoint, "LeS-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
  m_stateTrade = EStateTrade::LongSubmitted;
  m_pOrderPending = pOrder;
  m_pPosition->PlaceOrder( pOrder );
  //ShowOrder( pOrder );
}

void Futures::EnterShort( const ou::tf::Quote& quote ) { // limit orders, in real, will need to be normalized
  double dblMidPoint( quote.Midpoint() );
  //assert( nullptr == m_pOrderPending.get() );
  m_dblProfitMax = m_dblUnRealized = m_dblProfitMin = 0.0;
  //pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
  pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, 1, m_quote.Ask() );
  assert( pOrder );
  pOrder->SetSignalPrice( dblMidPoint );
  pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Futures::HandleOrderCancelled ) );
  pOrder->OnOrderFilled.Add( MakeDelegate( this, &Futures::HandleOrderFilled ) );
  m_ceShortEntry.AddLabel( quote.DateTime(), dblMidPoint, "SeS-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
  m_stateTrade = EStateTrade::ShortSubmitted;
  m_pOrderPending = pOrder;
  m_pPosition->PlaceOrder( pOrder );
  //ShowOrder( pOrder );
}

void Futures::ExitLong( const ou::tf::Quote& quote ) {
  double dblMidPoint( quote.Midpoint() );
  //assert( nullptr == m_pOrderPending.get() );
  pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
  //pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, 1, m_quote.Ask() );
  assert( pOrder );
  pOrder->SetSignalPrice( dblMidPoint );
  pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Futures::HandleOrderCancelled ) );
  pOrder->OnOrderFilled.Add( MakeDelegate( this, &Futures::HandleOrderFilled ) );
  m_ceLongExit.AddLabel( quote.DateTime(), dblMidPoint, "LxS1-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
  m_stateTrade = EStateTrade::LongExitSubmitted;
  m_pOrderPending = pOrder;
  m_pPosition->PlaceOrder( pOrder );
  //ShowOrder( pOrder );
}

void Futures::ExitShort( const ou::tf::Quote& quote ) {
  double dblMidPoint( quote.Midpoint() );
  //assert( nullptr == m_pOrderPending.get() );
  pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
  //pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 1, m_quote.Bid() );
  assert( pOrder );
  pOrder->SetSignalPrice( dblMidPoint );
  pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Futures::HandleOrderCancelled ) );
  pOrder->OnOrderFilled.Add( MakeDelegate( this, &Futures::HandleOrderFilled ) );
  m_ceShortExit.AddLabel( quote.DateTime(), dblMidPoint, "SxS1-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
  m_stateTrade = EStateTrade::ShortExitSubmitted;
  m_pOrderPending = pOrder;
  m_pPosition->PlaceOrder( pOrder );
  //ShowOrder( pOrder );
}

void Futures::ExitPosition( const ou::tf::Quote& quote ) {
  pOrder_t pOrder;
  double dblMidPoint( quote.Midpoint() );

  if ( m_pPosition->IsActive() ) {
    assert( 1 == m_pPosition->GetActiveSize() );
    switch ( m_pPosition->GetRow().eOrderSideActive ) {
      case ou::tf::OrderSide::EOrderSide::Buy:
        pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
        pOrder->SetSignalPrice( dblMidPoint );
        pOrder->SetDescription(
            m_sProfitDescription + ","
          + boost::lexical_cast<std::string>( m_dblProfitMin ) + ","
          + boost::lexical_cast<std::string>( m_dblUnRealized ) + ","
          + boost::lexical_cast<std::string>( m_dblProfitMax - m_dblUnRealized ) + ","
          + boost::lexical_cast<std::string>( m_dblProfitMax )
          );
        pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Futures::HandleExitOrderCancelled ) );
        pOrder->OnOrderFilled.Add( MakeDelegate( this, &Futures::HandleExitOrderFilled ) );
        m_ceLongExit.AddLabel( quote.DateTime(), dblMidPoint, "LxS2-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
        m_stateTrade = EStateTrade::LongExitSubmitted;
        m_pPosition->PlaceOrder( pOrder );
        ShowOrder( pOrder );
        break;
      case ou::tf::OrderSide::EOrderSide::Sell:
        pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
        pOrder->SetSignalPrice( dblMidPoint );
        pOrder->SetDescription(
            m_sProfitDescription + ","
          + boost::lexical_cast<std::string>( m_dblProfitMin ) + ","
          + boost::lexical_cast<std::string>( m_dblUnRealized ) + ","
          + boost::lexical_cast<std::string>( m_dblProfitMax - m_dblUnRealized ) + ","
          + boost::lexical_cast<std::string>( m_dblProfitMax )
          );
        pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Futures::HandleExitOrderCancelled ) );
        pOrder->OnOrderFilled.Add( MakeDelegate( this, &Futures::HandleExitOrderFilled ) );
        m_ceShortExit.AddLabel( quote.DateTime(), dblMidPoint, "SxS2-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
        m_stateTrade = EStateTrade::ShortExitSubmitted;
        m_pPosition->PlaceOrder( pOrder );
        ShowOrder( pOrder );
        break;
      default:
        assert( false ); // maybe check for unknown
    }
  }
  else {
    m_stateTrade = EStateTrade::Search;
  }
}

void Futures::ShowOrder( pOrder_t pOrder ) {
  //m_pTreeItemOrder = m_pTreeItemSymbol->AppendChild(
  //    "Order "
  //  + boost::lexical_cast<std::string>( m_pOrder->GetOrderId() )
  //  );
}

void Futures::HandleRHTrading( const ou::tf::Quote& quote ) {

  ptime dt( quote.DateTime() );

  // position has the quotes via the embedded watch
  // indicators could be attached to the embedded watch (but sometimes that watch has vector turned off)

#if RDAF
  if ( m_pTreeQuote ) { // wait for initialization in thread to start
    std::time_t nTime = boost::posix_time::to_time_t( quote.DateTime() );

    m_branchQuote.time = (double)nTime / 1000.0;
    m_branchQuote.ask = quote.Ask();
    m_branchQuote.askvol = quote.AskSize();
    m_branchQuote.bid = quote.Bid();
    m_branchQuote.bidvol = quote.BidSize();

    m_pTreeQuote->Fill();
  }
#endif

  // m_rHiPass[2].Update( dt, ma1 ); // not here, no regular interval

  // ehlers cybernetic analysis for stocks & futures page 7
  //double k_fisher = 0.5 * std::log( ( 1.0 + k_normalized ) / ( 1.0 - k_normalized ) );

  // Moving Average

  const double ma0( m_vMovingAverageSlope[0].EMA() ); // shortest
  const double ma1( m_vMovingAverageSlope[1].EMA() );
  const double ma2( m_vMovingAverageSlope[2].EMA() );
  const double ma3( m_vMovingAverageSlope[3].EMA() ); // longest

  m_ceRelativeMA1.Append( dt, ma0 - ma3 );
  m_ceRelativeMA2.Append( dt, ma1 - ma3 );
  m_ceRelativeMA3.Append( dt, ma2 - ma3 );

  double slope0 = m_vMovingAverageSlope[0].Slope();
  double slope1 = m_vMovingAverageSlope[1].Slope();
  double slope2 = m_vMovingAverageSlope[2].Slope();
  double slope3 = m_vMovingAverageSlope[3].Slope();

  // use a series of State objects to track changes,
  //   and forecast with a probability model
  //   for appropriate returns

  State state(
    quote.Bid(), quote.Ask()
  , ma0, ma1, ma2, ma3
  , slope0, slope1, slope2, slope3
  );

  EStateDesired stateDesired( EStateDesired::Continue );

  // TODO: perform a 'potential profit' test to determine if entering is desirable
  //    maybe a width of moving averages - which requires some data collection
  //    on successful vs unsuccessful

  // TODO: trail and decrement stop as price moves
  static const double stop_delta_min( 0.50 );

  if ( state.EnterLong() ) {
    double diff = state.Stop();
    assert( 0.0 < diff );
    if ( stop_delta_min <= diff ) {
      m_dblStopDeltaProposed = diff;
      stateDesired = EStateDesired::GoLong;
    }
  }
  else {
    if ( state.EnterShort() ) {
      double diff = state.Stop();
      assert( 0.0 < diff );
      if ( stop_delta_min <= diff ) {
        m_dblStopDeltaProposed = diff;
        stateDesired = EStateDesired::GoShort;
      }
    }
    else {
      // TODO: suggestion, four seconds perform a cancel, or from some other event
    }
  }

  // 1a) filter on moving average to reduce churn
  // 1b) lock it in by trailing a stop, based upon one of the ma
  // 2) run limit orders to reduce slippage
  // 3) apply risk or prediction factors, use markov chain to retry successful trades

  // how many seconds (or ratio) does it trend vs going sideways?

  static const boost::posix_time::seconds wait( 2 );

  switch ( m_stateTrade ) {
    case EStateTrade::Search:
      switch ( stateDesired ) {
        case EStateDesired::GoLong:

          m_signal( ESignal::rising );

          m_dblStopActiveDelta = m_dblStopDeltaProposed;
          m_dblStopActiveActual = quote.Ask() - m_dblStopActiveDelta;

          BOOST_LOG_TRIVIAL(info) << dt << " Search->GoLong," << m_dblStopActiveActual << "," << m_dblStopActiveDelta;
          m_sProfitDescription = "l,srch";
          EnterLong( quote );
          break;
        case EStateDesired::GoShort:

          m_signal( ESignal::falling );

          m_dblStopActiveDelta = m_dblStopDeltaProposed;
          m_dblStopActiveActual = quote.Bid() + m_dblStopActiveDelta;

          BOOST_LOG_TRIVIAL(info) << dt << " Search->GoShort," << m_dblStopActiveActual << "," << m_dblStopActiveDelta;
          m_sProfitDescription = "s,srch";
          EnterShort( quote );
          break;
        case EStateDesired::Exit:
        case EStateDesired::Cancel:
        case EStateDesired::Continue:
          break; // nothing to do
        default: // ensure 100% coverage
          assert( false );
      }
      break;
    case EStateTrade::LongSubmitted:
      // wait for order to execute
      if ( m_pOrderPending ) {
        if ( wait < ( dt - m_pOrderPending->GetDateTimeOrderSubmitted() ) ) {
          auto id = m_pOrderPending->GetOrderId();
          BOOST_LOG_TRIVIAL(info) << dt << " LongSubmitted->Cancel " << id;
          //m_stateTrade = EStateTrade::Cancelling; // can't do this as a fill may happen
          m_pOrderPending.reset();
          m_pPosition->CancelOrder( id );
        }
      }
      break;
    case EStateTrade::LongExitSignal:
      switch ( stateDesired ) {
        case EStateDesired::GoShort:
          {
            bool bGoShort( true );
            if ( bGoShort ) {
              BOOST_LOG_TRIVIAL(info) << dt << " LongExitSignal->GoShort";
              if ( EStateDesired::GoShort == stateDesired ) {
                m_sProfitDescription += ",x,short";
              }
              ExitPosition( quote );
              m_sProfitDescription = "s,go";
              EnterShort( quote );
            }
          }
          break;
        case EStateDesired::Exit:
          BOOST_LOG_TRIVIAL(info) << dt << " LongExitSignal->Exit";
          ExitLong( quote );
          break;
        case EStateDesired::GoLong:
          //BOOST_LOG_TRIVIAL(info)
          //  << dt << " LongExitSignal->GoLong staying: "
          //  << (int)stateDesired
          //  ; // already long
          break;
        case EStateDesired::Continue:
          {
            double bid = quote.Bid();

            if ( bid > ( m_dblStopActiveActual + m_dblStopActiveDelta ) ) {
              m_dblStopActiveActual = bid - m_dblStopActiveDelta;
            }

            if ( bid <= m_dblStopActiveActual ) {
              BOOST_LOG_TRIVIAL(info) << dt << " LongExitSignal->Continue(stop out)";
              m_sProfitDescription += ",x,cont.stop";
              ExitPosition( quote ); // provides completion summary
              //ExitLong( bar );
            }
          }
          break;
        case EStateDesired::Cancel:
          BOOST_LOG_TRIVIAL(info) << dt << " LongExitSignal->Cancel - nothing to cancel";
          break;
        default:
          assert( false );  // broken state machine
      }
      break;
    case EStateTrade::ShortSubmitted:
      // wait for order to execute
      if ( m_pOrderPending ) {
        if ( wait < ( dt - m_pOrderPending->GetDateTimeOrderSubmitted() ) ) {
          auto id = m_pOrderPending->GetOrderId();
          BOOST_LOG_TRIVIAL(info) << dt << " ShortSubmitted->Cancel " << id;
          //m_stateTrade = EStateTrade::Cancelling; // can't do this as a fill may happen
          m_pOrderPending.reset();
          m_pPosition->CancelOrder( id );
        }
      }
      break;
    case EStateTrade::ShortExitSignal:
      switch ( stateDesired ) {
        case EStateDesired::GoLong:
          {
            bool bGoLong( true );
            if ( bGoLong ) {
              BOOST_LOG_TRIVIAL(info) << dt << " ShortExitSignal->GoLong";
              if ( EStateDesired::GoLong == stateDesired ) {
                m_sProfitDescription += ",x,long";
              }
              ExitPosition( quote );
              m_sProfitDescription = "l,go";
              EnterLong( quote );
            }
          }
          break;
        case EStateDesired::Exit:
          BOOST_LOG_TRIVIAL(info) << dt << " ShortExitSignal->Exit";
          ExitShort( quote );
          break;
        case EStateDesired::GoShort:
          //BOOST_LOG_TRIVIAL(info)
          //  << dt << " ShortExitSignal->GoShort staying: "
          //  << (int)stateDesired
          //  ; // already short
          break;
        case EStateDesired::Continue:
          {
            double ask = quote.Ask();

            if ( ask < ( m_dblStopActiveActual - m_dblStopActiveDelta ) ) {
              m_dblStopActiveActual = ask + m_dblStopActiveDelta;
            }

            if ( ask >= m_dblStopActiveActual ) {
              BOOST_LOG_TRIVIAL(info) << dt << " ShortExitSignal->Continue(stop out)";
              m_sProfitDescription += ",x,cont.stop";
              ExitPosition( quote ); // provides completion summary
              //ExitShort( bar );
            }
          }
          break;
        case EStateDesired::Cancel:
          BOOST_LOG_TRIVIAL(info) << dt << " ShortExitSignal->Cancel - nothing to cancel";
          break;
        default:
          assert( false );  // broken state machine
      }
      break;
    case EStateTrade::LongExitSubmitted:
    case EStateTrade::ShortExitSubmitted:
      // wait for order to execute
      break;
    case EStateTrade::EndOfDayCancel:
    case EStateTrade::EndOfDayNeutral:
    case EStateTrade::Done:
      // perform exit?
      break;
    case EStateTrade::NoTrade:
      // do nothing based upon config file
      break;
    case EStateTrade::Init:
      if ( ( EStateDesired::GoLong == stateDesired ) || ( EStateDesired::GoShort == stateDesired ) ) {
        m_stateTrade = EStateTrade::Search;
      }
      break;
  }

  //m_stateMovingAverage = currentMovingAverage; // not stateMovingAverage

}

// not called at the moment
void Futures::HandleRHTrading( const ou::tf::Bar& bar ) { // once a second

  //const std::chrono::time_point<std::chrono::system_clock> begin
  //  = std::chrono::system_clock::now();

  ptime dt( bar.DateTime() );

  // Hi Pass - at one second intervals - continuous won't work with irregular intervals (ie quote/trade)
  // needs better integration

  //const double ma1( m_vMovingAverageSlope[1].EMA() );

  //m_rHiPass[0].Update( dt, ma0 );
  //m_rHiPass[1].Update( dt, ma1 );
  //m_rHiPass[2].Update( dt, ma1 );
  //m_rHiPass[3].Update( dt, ma1 );

  //HiPass& hp( m_rHiPass[2] );

  // ==

  //const bool bHP0Hi( 0.0 < hp.m_dblHPF0 );
  //const bool bHP0Lo( 0.0 > hp.m_dblHPF0 );

  //const bool bHP1Hi( 0.0 < hp.m_dblHPF1 );
  //const bool bHP1Lo( 0.0 > hp.m_dblHPF1 );

  //const bool bHPUpCross( bHP1Lo && bHP0Hi );
  //const bool bHPDnCross( bHP1Hi && bHP0Lo );

  // ==

  //const bool bHpSlope0Hi( 0.0 < hp.m_dblHPF_Slope0 );
  //const bool bHPSlope0Lo( 0.0 > hp.m_dblHPF_Slope0 );

  //const bool bHPSlope1Hi( 0.0 < hp.m_dblHPF_Slope1 );
  //const bool bHPSlope1Lo( 0.0 > hp.m_dblHPF_Slope1 );

  //const bool bHPSlopeUpCross( bHPSlope1Lo && bHpSlope0Hi );
  //const bool bHPSlopeDnCross( bHPSlope1Hi && bHPSlope0Lo );

  // ==

  //if ( !bHPUpCross && !bHPDnCross ) {
    //stateDesired = EStateDesired::Cancel; // implement on limit orders
  //}
  //else {
    //if ( EMovingAverage::Flat == currentMovingAverage ) {
    //  stateDesired = EStateDesired::Exit;
    //}
    //else {
      //if ( bMaRising && bHPSlopeUpCross ) {
      //if ( bHPSlopeUpCross ) {
//        stateDesired = EStateDesired::GoLong;
      //}
      //else {
        //if ( bMaFalling && bHPSlopeDnCross ) {
        //if ( bHPSlopeDnCross ) {
//          stateDesired = EStateDesired::GoShort;
        //}
        //else {
          //if ( bHPUpCross || bHPDnCross ) {
//            stateDesired = EStateDesired::Cancel;
          //}
        //}
      //}
    //}
  //}

  //const std::chrono::time_point<std::chrono::system_clock> end
  //  = std::chrono::system_clock::now();

   //auto delta = std::chrono::duration_cast<std::chrono::microseconds>( end - begin).count();
   //auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( end - begin).count();
   //m_ceExecutionTime.Append( bar.DateTime(), delta );

   Torch::Op op = m_pTorch->StepModel( bar.DateTime() );

   switch ( op ) {
    case Torch::Op::Long:
      break;
    case Torch::Op::Hold:
      break;
    case Torch::Op::Short:
      break;
    case Torch::Op::Neutral:
      break;
   }

}

void Futures::HandleOrderCancelled( const ou::tf::Order& order ) {
  ou::tf::Order& order_( const_cast<ou::tf::Order&>( order ) );
  order_.OnOrderCancelled.Remove( MakeDelegate( this, &Futures::HandleOrderCancelled ) );
  order_.OnOrderFilled.Remove( MakeDelegate( this, &Futures::HandleOrderFilled ) );

  switch ( m_stateTrade ) {
    case EStateTrade::EndOfDayCancel:
    case EStateTrade::EndOfDayNeutral:
      BOOST_LOG_TRIVIAL(info) << "order " << order.GetOrderId() << " cancelled - end of day";
      break;
    case EStateTrade::LongExitSubmitted:
    case EStateTrade::ShortExitSubmitted:
      //assert( false );  // TODO: need to figure out a plan to retry exit
      BOOST_LOG_TRIVIAL(error) << "order " << order.GetOrderId() << " cancelled";
      m_stateTrade = EStateTrade::Done;
      break;
    default:
      m_stateTrade = EStateTrade::Search;
  }
  m_pOrderPending.reset();
}

void Futures::HandleOrderFilled( const ou::tf::Order& order ) {
  ou::tf::Order& order_( const_cast<ou::tf::Order&>( order ) );
  order_.OnOrderCancelled.Remove( MakeDelegate( this, &Futures::HandleOrderCancelled ) );
  order_.OnOrderFilled.Remove( MakeDelegate( this, &Futures::HandleOrderFilled ) );

  //m_dblStopActiveActual = order.GetAverageFillPrice(); // needs adjustment with delta

  switch ( m_stateTrade ) {
    case EStateTrade::LongSubmitted:
      m_ceLongEntry.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "LeF-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      m_stateTrade = EStateTrade::LongExitSignal;
      break;
    case EStateTrade::ShortSubmitted:
      m_ceShortEntry.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "SeF-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      m_stateTrade = EStateTrade::ShortExitSignal;
      break;
    case EStateTrade::LongExitSubmitted:
      m_ceLongExit.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "LxF-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      m_stateTrade = EStateTrade::Search;
      break;
    case EStateTrade::ShortExitSubmitted:
      m_ceShortExit.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "SxF-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      m_stateTrade = EStateTrade::Search;
      break;
    case EStateTrade::EndOfDayCancel:
    case EStateTrade::EndOfDayNeutral:
      // figure out what labels to apply
      break;
    case EStateTrade::Done:
      break;
    default:
       assert( false ); // TODO: unravel the state mess if we get here
  }
  m_pOrderPending.reset();
}

void Futures::HandleExitOrderCancelled( const ou::tf::Order& order ) {
  ou::tf::Order& order_( const_cast<ou::tf::Order&>( order ) );
  order_.OnOrderCancelled.Remove( MakeDelegate( this, &Futures::HandleExitOrderCancelled ) );
  order_.OnOrderFilled.Remove( MakeDelegate( this, &Futures::HandleExitOrderFilled ) );

  switch ( order.GetOrderSide() ) {
    case ou::tf::OrderSide::EOrderSide::Buy: // is dt filled at 'internal' time?
      //m_ceLongExit.AddLabel( order.GetDateTimeOrderFilled(), order.GetSignalPrice(), "LxC-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      m_stateTrade = EStateTrade::Cancelled;  // or use cancelled for custom processing
      break;
    case ou::tf::OrderSide::EOrderSide::Sell: // is dt filled at 'internal' time?
      //m_ceShortExit.AddLabel( order.GetDateTimeOrderFilled(), order.GetSignalPrice(), "SxC-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      m_stateTrade = EStateTrade::Cancelled;  // or use cancelled for custom processing
      break;
    default:
      assert( false );
  }
}

void Futures::HandleExitOrderFilled( const ou::tf::Order& order ) {
  ou::tf::Order& order_( const_cast<ou::tf::Order&>( order ) );
  order_.OnOrderCancelled.Remove( MakeDelegate( this, &Futures::HandleExitOrderCancelled ) );
  order_.OnOrderFilled.Remove( MakeDelegate( this, &Futures::HandleExitOrderFilled ) );

  switch ( order.GetOrderSide() ) {
    case ou::tf::OrderSide::EOrderSide::Buy:
      //m_ceLongExit.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "LxF-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      switch( m_stateTrade ) {
        case EStateTrade::ShortExitSubmitted:
          m_stateTrade = EStateTrade::Search;
          break;
      }
      break;
    case ou::tf::OrderSide::EOrderSide::Sell:
      //m_ceShortExit.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "SxF-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      switch( m_stateTrade ) {
        case EStateTrade::LongExitSubmitted:
          m_stateTrade = EStateTrade::Search;
          break;
      }
      break;
    default:
      assert( false );
  }
}

void Futures::HandleCancel( boost::gregorian::date, boost::posix_time::time_duration ) { // one shot
  m_stateTrade = EStateTrade::EndOfDayCancel;
  if ( m_pPosition ) {
    m_pPosition->CancelOrders();
  }
}

void Futures::HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration ) { // one shot
  switch ( m_stateTrade ) {
    case EStateTrade::NoTrade:
      // do nothing
      break;
    default:
      m_stateTrade = EStateTrade::EndOfDayNeutral;
      if ( m_pPosition ) {
        m_pPosition->ClosePosition();
      }
      break;
  }
}

void Futures::SaveWatch( const std::string& sPrefix ) {
  // RecordSeries has been set to false
  Base::SaveWatch( sPrefix );
  //if ( m_pPosition ) {
    //m_pPosition->GetWatch()->SaveSeries( sPrefix );
    //if (m_pFile){ // don't do this, as the file is save on exit,
    //  this will create another version, which will cause problems during reload
    //  m_pFile->Write();
    //}
  //}
}

void Futures::CloseAndDone() {
  std::cout << "Sending Close & Done" << std::endl;
  switch ( m_stateTrade ) {
    case EStateTrade::NoTrade:
      // do nothing
      break;
    default:
      if ( m_pPosition ) {
        m_pPosition->ClosePosition();
      }
      m_stateTrade = EStateTrade::Done;
      break;
  }
}

} // namespace Strategy
