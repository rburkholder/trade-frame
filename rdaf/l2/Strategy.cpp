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
 * File:    Strategy.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: December 9, 2022  16:18:19
 */

// https://indico.cern.ch/event/697389/contributions/3062036/attachments/1712790/2761904/Support_for_SIMD_Vectorization_in_ROOT_ROOT_Workshop_2018.pdf
// https://www.intel.com/content/www/us/en/develop/documentation/advisor-user-guide/top/analyze-cpu-roofline.html
// https://stackoverflow.com/questions/52653025/why-is-march-native-used-so-rarely

#include <chrono>

// no longer use iostream, std::cout has a multithread contention problem
//   due to it being captured to the gui, and is not thread safe
#include <boost/log/trivial.hpp>

#include <boost/lexical_cast.hpp>

#include <rdaf/TH2.h>
#include <rdaf/TTree.h>
#include <rdaf/TFile.h>

#include <OUCharting/ChartDataView.h>

#include <TFVuTrading/TreeItem.hpp>

#include "Strategy.hpp"

using pWatch_t = ou::tf::Watch::pWatch_t;

namespace {
  static const unsigned int max_ix = 10; // TODO need to obtain from elsewhere & sync with Symbols
  static const int k_up = 80;
  static const int k_lo = 20;
  static const boost::posix_time::time_duration filter_stoch( 0, 0, 2 );
}

Strategy::Strategy(
  const ou::tf::config::symbol_t& config
, TreeItem* pTreeItem
//, pFile_t pFile
//, pFile_t pFileUtility
)
: ou::tf::DailyTradeTimeFrame<Strategy>()
, m_pTreeItemSymbol( pTreeItem )
//, m_pFile( pFile )
//, m_pFileUtility( pFileUtility )
, m_stateTrade( EStateTrade::Init )
, m_stableStochastic( EStateStochastic::Init )
, m_stateStochastic( EStateStochastic::Init )
, m_emaStochastic( m_pricesStochastic, filter_stoch )
, m_stateMovingAverage( EMovingAverage::Flat )
, m_bUseMARising( false ), m_bUseMAFalling( false )
, m_config( config )
, m_ceLongEntry( ou::ChartEntryShape::EShape::Long, ou::Colour::Blue )
//, m_ceLongFill( ou::ChartEntryShape::EShape::FillLong, ou::Colour::Blue )
, m_ceLongExit( ou::ChartEntryShape::EShape::LongStop, ou::Colour::Blue )
, m_ceShortEntry( ou::ChartEntryShape::EShape::Short, ou::Colour::Red )
//, m_ceShortFill( ou::ChartEntryShape::EShape::FillShort, ou::Colour::Red )
, m_ceShortExit( ou::ChartEntryShape::EShape::ShortStop, ou::Colour::Red )
, m_bfQuotes01Sec( 1 )
{
//  assert( m_pFile );
//  assert( m_pFileUtility );

  m_ceQuoteAsk.SetColour( ou::Colour::Red );
  m_ceQuoteBid.SetColour( ou::Colour::Blue );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );

  m_cemZero.AddMark( 0, ou::Colour::Black,  "0" );

  // need to present the marks prior to presenting the data
  m_cemStochastic.AddMark( 100, ou::Colour::Black,    "" ); // hidden by legend
  m_cemStochastic.AddMark(  k_up, ou::Colour::Red,   boost::lexical_cast<std::string>( k_up ) + "%" );
  m_cemStochastic.AddMark(  50, ou::Colour::Green, "50%" );
  m_cemStochastic.AddMark(  k_lo, ou::Colour::Blue,  boost::lexical_cast<std::string>( k_lo ) + "%" );
  m_cemStochastic.AddMark(   0, ou::Colour::Black,  "0%" );

  m_ceProfitUnRealized.SetColour( ou::Colour::Purple );
  m_ceProfitRealized.SetColour( ou::Colour::DeepSkyBlue );
  m_ceCommissionsPaid.SetColour( ou::Colour::DarkGreen );
  m_ceProfit.SetColour( ou::Colour::Green );

  m_ceQuoteAsk.SetName( "Ask" );
  m_ceTrade.SetName( "Tick" );
  m_ceQuoteBid.SetName( "Bid" );

  m_ceVolume.SetName( "Volume" );

  //m_ceSkewness.SetName( "Skew" );

  m_ceStochastic.SetName( "Stoch Prob" );
  m_ceStochastic.SetColour( ou::Colour::DarkGreen );

  m_ceProfitUnRealized.SetName( "UnRealized" );
  m_ceProfitRealized.SetName( "Realized" );
  m_ceCommissionsPaid.SetName( "Commissions" );
  m_ceProfit.SetName( "Profit" );

  //m_ceExecutionTime.SetName( "Execution Time" );

  m_bfQuotes01Sec.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarQuotes01Sec ) );

  m_cdMarketDepthAsk.SetName( "MarketDepth Ask" );
  m_cdMarketDepthAsk.SetColour( ou::Colour::Red );

  m_cdMarketDepthBid.SetName( "MarketDepth Bid" );
  m_cdMarketDepthBid.SetColour( ou::Colour::Blue );

  //m_ceFVS_Var1_Ask.SetName( "Depth Ask Var1" );
  //m_ceFVS_Var1_Bid.SetName( "Depth Bid Var1" );

  //m_ceFVS_Var1_Ask.SetColour( ou::Colour::Red );
  //m_ceFVS_Var1_Diff.SetColour( ou::Colour::Green );
  //m_ceFVS_Var1_Bid.SetColour( ou::Colour::Blue );

}

Strategy::~Strategy() {
  Clear();
}

void Strategy::SetupChart() {

  m_cdv.Add( EChartSlot::Price, &m_ceQuoteAsk );
  m_cdv.Add( EChartSlot::Price, &m_ceTrade );
  m_cdv.Add( EChartSlot::Price, &m_ceQuoteBid );

  m_cdv.Add( EChartSlot::Price, &m_ceLongEntry );
  //m_cdv.Add( EChartSlot::Price, &m_ceLongFill );
  m_cdv.Add( EChartSlot::Price, &m_ceLongExit );
  m_cdv.Add( EChartSlot::Price, &m_ceShortEntry );
  //m_cdv.Add( EChartSlot::Price, &m_ceShortFill );
  m_cdv.Add( EChartSlot::Price, &m_ceShortExit );

  for ( vMovingAverage_t::value_type& ma: m_vMovingAverage ) {
    ma.AddToView( m_cdv, EChartSlot::Price );
  }

  m_cdv.Add( EChartSlot::Volume, &m_ceVolume );

  m_cdv.Add( EChartSlot::Stoch, &m_cemStochastic );

  for ( vStochastic_t::value_type& vt: m_vStochastic ) {
    vt->AddToView( m_cdv, EChartSlot::Price, EChartSlot::Stoch );
  }
  m_cdv.Add( EChartSlot::Stoch, &m_ceStochastic );

  //m_cdv.Add( EChartSlot::ImbalanceMean, &m_cemZero );

  //m_cdv.Add( EChartSlot::ImbalanceMean, &m_ceImbalanceRawMean );
  //m_ceImbalanceRawMean.SetName( "imbalance mean" );
  //m_ceImbalanceRawMean.SetColour( ou::Colour::LightGreen );

  //m_cdv.Add( EChartSlot::ImbalanceMean, &m_ceImbalanceSmoothMean );
  //m_ceImbalanceSmoothMean.SetColour( ou::Colour::DarkGreen );

  //m_cdv.Add( EChartSlot::Skew, &m_ceSkewness );

  m_cdv.Add( EChartSlot::PL1, &m_ceProfitUnRealized );
  m_cdv.Add( EChartSlot::PL2, &m_ceProfitRealized );
  m_cdv.Add( EChartSlot::PL1, &m_ceCommissionsPaid );
  m_cdv.Add( EChartSlot::PL2, &m_ceProfit );

  //m_cdv.Add( EChartSlot::FVS_Var1, &m_cemZero );

  //m_cdv.Add( EChartSlot::FVS_Var1, & m_ceFVS_Var1_Ask );
  //m_cdv.Add( EChartSlot::FVS_Var1, & m_ceFVS_Var1_Diff );
  //m_cdv.Add( EChartSlot::FVS_Var1, & m_ceFVS_Var1_Bid );

  //m_cdv.Add( EChartSlot::ET, &m_ceExecutionTime );

  //m_cdv.Add( EChartSlot::MarketDepth, &m_cdMarketDepthAsk );
  //m_cdv.Add( EChartSlot::MarketDepth, &m_cdMarketDepthBid );

}

void Strategy::SetPosition( pPosition_t pPosition ) {

  using pProvider_t = ou::tf::Watch::pProvider_t;

  assert( pPosition );

  Clear();

  m_pPosition = pPosition;
  pWatch_t pWatch = m_pPosition->GetWatch();
  pProvider_t pDataProvider = pWatch->GetProvider();

  m_cdv.SetNames( "AutoTrade", pWatch->GetInstrument()->GetInstrumentName() );

  assert( 0 < m_config.nPeriodWidth );
  time_duration td = time_duration( 0, 0, m_config.nPeriodWidth );

  // stochastic

  m_vStochastic.emplace_back( std::make_unique<Stochastic>( "1", m_quotes, m_config.nStochastic1Periods, td, ou::Colour::DeepSkyBlue ) );
  m_vStochastic.emplace_back( std::make_unique<Stochastic>( "2", m_quotes, m_config.nStochastic2Periods, td, ou::Colour::DodgerBlue ) );  // is dark: MediumSlateBlue; MediumAquamarine is greenish; MediumPurple is dark; Purple is dark
  m_vStochastic.emplace_back( std::make_unique<Stochastic>( "3", m_quotes, m_config.nStochastic3Periods, td, ou::Colour::MediumSlateBlue ) ); // no MediumTurquoise, maybe Indigo

  double total = 0.0;
  total += m_config.nStochastic1Periods;
  total += m_config.nStochastic2Periods;
  total += m_config.nStochastic3Periods;

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

  m_vMovingAverage.emplace_back( MovingAverage( m_quotes, vMAPeriods[0], td, ou::Colour::Brown, "ma1" ) );
  m_vMovingAverage.emplace_back( MovingAverage( m_quotes, vMAPeriods[1], td, ou::Colour::Coral, "ma2" ) );
  m_vMovingAverage.emplace_back( MovingAverage( m_quotes, vMAPeriods[2], td, ou::Colour::Gold,  "ma3" ) );

  SetupChart(); // comes after stochastic initialization

  //InitRdaf();

  pWatch->RecordSeries( false ); // use Collector for keeping data
  pWatch->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuote ) );
  pWatch->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTrade ) );

  switch ( m_config.eFeed ) {
    case ou::tf::config::symbol_t::EFeed::L1:
      break;
    case ou::tf::config::symbol_t::EFeed::L2M:  // L2 via MarketMaker (nasdaq equities)
      pWatch->OnDepthByMM.Add( MakeDelegate( this, &Strategy::HandleDepthByMM ) );

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

  BOOST_LOG_TRIVIAL(info)
    << "Strategy::SetPosition " << m_config.sSymbol_IQFeed
    << ": algorithm='" << m_config.sAlgorithm
    << "' signal_from='" <<m_config.sSignalFrom
    << "'"
    ;

}

void Strategy::StartDepthByOrder() {
  // from IndicatorTrading/FeedModel.cpp

  using EState = ou::tf::iqfeed::l2::OrderBased::EState;

  m_FeatureSet.Set( 10 );

  m_pOrderBased = ou::tf::iqfeed::l2::OrderBased::Factory();

  m_pOrderBased->Set(
    [this]( ou::tf::iqfeed::l2::EOp op, unsigned int ix, const ou::tf::Depth& depth ){ // fBookChanges_t&& fBid_

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
        //Imbalance( depth );
        if ( 1 == ix ) {
          //double var1 = m_FeatureSet.FVS()[1].bid.v9.accelLimit;
          //double var1B = m_FeatureSet.FVS()[1].bid.v8.relativeLimit;
          //m_ceFVS_Var1_Bid.Append( depth.DateTime(), -var1B );
          //double var1A = m_FeatureSet.FVS()[1].ask.v8.relativeLimit;
          //m_ceFVS_Var1_Diff.Append( depth.DateTime(), var1A - var1B );
        }
      }
    },
    [this]( ou::tf::iqfeed::l2::EOp op, unsigned int ix, const ou::tf::Depth& depth ){ // fBookChanges_t&& fAsk_

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
        //Imbalance( depth );
        if ( 1 == ix ) {
          //double var1 = m_FeatureSet.FVS()[1].ask.v9.accelLimit;
          //double var1A = m_FeatureSet.FVS()[1].ask.v8.relativeLimit;
          //m_ceFVS_Var1_Ask.Append( depth.DateTime(), var1A );
          //double var1B = m_FeatureSet.FVS()[1].bid.v8.relativeLimit;
          //m_ceFVS_Var1_Diff.Append( depth.DateTime(), var1A - var1B );
        }
      }
    }
  );

  pWatch_t pWatch = m_pPosition->GetWatch();
  pWatch->OnDepthByOrder.Add( MakeDelegate( this, &Strategy::HandleDepthByOrder ) );

}

void Strategy::Imbalance( const ou::tf::Depth& depth ) {
  // from IndicatorTrading/FeedModel.cpp

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

  //m_pInteractiveChart->UpdateImbalance( depth.DateTime(), stats.meanY, m_dblImbalanceMean );

  boost::posix_time::ptime dt( depth.DateTime() );

  m_ceImbalanceRawMean.Append( dt, stats.meanY );
  //m_ceImbalanceRawB1.Append( depth.DateTime(), stats.b1 );

  m_ceImbalanceSmoothMean.Append( dt, m_dblImbalanceMean );
  //m_ceImbalanceSmoothB1.Append( depth.DateTime(), m_dblImbalanceSlope );

}

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

void Strategy::Clear() {

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
          pWatch->OnDepthByMM.Remove( MakeDelegate( this, &Strategy::HandleDepthByMM ) );
        }
        break;
      case ou::tf::config::symbol_t::EFeed::L2O:
        break;
    }

    pWatch->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleQuote ) );
    pWatch->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleTrade ) );

    m_cdv.Clear();
    //m_pPosition.reset(); // need to fix relative to thread
  }
}

void Strategy::InitRdaf() {
/*
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
*/
}

void Strategy::HandleQuote( const ou::tf::Quote& quote ) {
  // position has the quotes via the embedded watch
  // indicators are also attached to the embedded watch

  //if ( !quote.IsValid() ) { // empty function
  //  return;
  //}

  ptime dt( quote.DateTime() );

  m_ceQuoteAsk.Append( dt, quote.Ask() );
  m_ceQuoteBid.Append( dt, quote.Bid() );

  m_quote = quote;
  m_quotes.Append( quote );

//  if ( m_pTreeQuote ) { // wait for initialization in thread to start
//    std::time_t nTime = boost::posix_time::to_time_t( quote.DateTime() );

//    m_branchQuote.time = (double)nTime / 1000.0;
//    m_branchQuote.ask = quote.Ask();
//    m_branchQuote.askvol = quote.AskSize();
//    m_branchQuote.bid = quote.Bid();
//    m_branchQuote.bidvol = quote.BidSize();

//    m_pTreeQuote->Fill();
//  }

  for ( vMovingAverage_t::value_type& ma: m_vMovingAverage ) {
    ma.Update( dt );
  }

  double mix {};
  mix += 0.33 * m_vStochastic[0]->Latest();
  mix += 0.33 * m_vStochastic[1]->Latest();
  mix += 0.33 * m_vStochastic[2]->Latest();

  //double value = m_vStochastic[1]->Latest();
  double value = mix;
  m_pricesStochastic.Append( ou::tf::Price( dt, value ) ); // updates m_emaStochastic

  double k = m_emaStochastic.GetEMA();
  m_ceStochastic.Append( dt, k );

  EStateStochastic stochasticStablizing( m_stableStochastic ); // sticky until changed

  if ( k >= 50.0 ) {
    if ( k > (double)k_up ) {
      stochasticStablizing = EStateStochastic::AboveHi;
    }
    else {
      stochasticStablizing = EStateStochastic::AboveMid;
    }
  }
  else {
    if ( k < (double)k_lo ) {
      stochasticStablizing = EStateStochastic::BelowLo;
    }
    else {
      stochasticStablizing = EStateStochastic::BelowMid;
    }
  }

  m_stableStochastic = stochasticStablizing;

  m_bfQuotes01Sec.Add( dt, m_quote.Midpoint(), 1 ); // provides a 1 sec pulse for checking the algorithm

}

void Strategy::HandleTrade( const ou::tf::Trade& trade ) {

  ptime dt( trade.DateTime() );

  m_ceTrade.Append( dt, trade.Price() );
  m_ceVolume.Append( dt, trade.Volume() );

  const double mid = m_quote.Midpoint();
  const ou::tf::Trade::price_t price = trade.Price();
  const uint64_t volume = trade.Volume();

  if ( price >= mid ) {
    m_nMarketOrdersAsk++;
  }
  else {
    m_nMarketOrdersBid++;
  }

/*
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
*/
}

void Strategy::HandleDepthByMM( const ou::tf::DepthByMM& depth ) {

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

void Strategy::HandleDepthByOrder( const ou::tf::DepthByOrder& depth ) {
  m_pOrderBased->MarketDepth( depth );
}


void Strategy::HandleUpdateL2Ask( price_t price, volume_t volume, bool bAdd ) {
}

void Strategy::HandleUpdateL2Bid( price_t price, volume_t volume, bool bAdd ) {
}

void Strategy::HandleBarQuotes01Sec( const ou::tf::Bar& bar ) {

  double dblRealized, dblCommissionsPaid, dblTotal;

  m_pPosition->QueryStats( m_dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );

  m_ceProfitUnRealized.Append( bar.DateTime(), m_dblUnRealized );
  m_ceProfitRealized.Append( bar.DateTime(), dblRealized );
  m_ceCommissionsPaid.Append( bar.DateTime(), dblCommissionsPaid );
  m_ceProfit.Append( bar.DateTime(), dblTotal );

  if ( m_dblUnRealized > m_dblProfitMax )m_dblProfitMax = m_dblUnRealized;
  if ( m_dblUnRealized < m_dblProfitMin )m_dblProfitMin = m_dblUnRealized;

  TimeTick( bar );
}

/*
  // template to submit GTC limit order
  // strip off fractional seconds
  boost::posix_time::ptime dtQuote
    = quote.DateTime()
    - boost::posix_time::time_duration( 0, 0, 0, quote.DateTime( ).time_of_day().fractional_seconds() );

  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, quantity, price );

  m_pOrder->SetGoodTillDate( dtQuote + boost::posix_time::seconds( 30 ) );
  m_pOrder->SetTimeInForce( ou::tf::TimeInForce::GoodTillDate );
*/

void Strategy::EnterLong( const ou::tf::Bar& bar ) {
  m_dblProfitMax = m_dblUnRealized = m_dblProfitMin = 0.0;
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
  m_pOrder->SetSignalPrice( bar.Close() );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceLongEntry.AddLabel( bar.DateTime(), bar.Close(), "LeS-" + boost::lexical_cast<std::string>( m_pOrder->GetOrderId() ) );
  m_stateTrade = EStateTrade::LongSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
  ShowOrder( m_pOrder );
}

void Strategy::EnterShort( const ou::tf::Bar& bar ) {
  m_dblProfitMax = m_dblUnRealized = m_dblProfitMin = 0.0;
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
  m_pOrder->SetSignalPrice( bar.Close() );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceShortEntry.AddLabel( bar.DateTime(), bar.Close(), "SeS-" + boost::lexical_cast<std::string>( m_pOrder->GetOrderId() ) );
  m_stateTrade = EStateTrade::ShortSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
  ShowOrder( m_pOrder );
}

void Strategy::ExitLong( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
  m_pOrder->SetSignalPrice( bar.Close() );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceLongExit.AddLabel( bar.DateTime(), bar.Close(), "LxS1-" + boost::lexical_cast<std::string>( m_pOrder->GetOrderId() ) );
  m_stateTrade = EStateTrade::LongExitSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
  ShowOrder( m_pOrder );
}

void Strategy::ExitShort( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
  m_pOrder->SetSignalPrice( bar.Close() );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceShortExit.AddLabel( bar.DateTime(), bar.Close(), "SxS1-" + boost::lexical_cast<std::string>( m_pOrder->GetOrderId() ) );
  m_stateTrade = EStateTrade::ShortExitSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
  ShowOrder( m_pOrder );
}

void Strategy::ExitPosition( const ou::tf::Bar& bar ) {
  if ( m_pPosition->IsActive() ) {
    assert( 1 == m_pPosition->GetActiveSize() );
    switch ( m_pPosition->GetRow().eOrderSideActive ) {
      case ou::tf::OrderSide::EOrderSide::Buy:
        m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
        m_pOrder->SetSignalPrice( bar.Close() );
        m_pOrder->SetDescription(
            m_sProfitDescription + ","
          + boost::lexical_cast<std::string>( m_dblProfitMin ) + ","
          + boost::lexical_cast<std::string>( m_dblUnRealized ) + ","
          + boost::lexical_cast<std::string>( m_dblProfitMax - m_dblUnRealized ) + ","
          + boost::lexical_cast<std::string>( m_dblProfitMax )
          );
        m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleExitOrderCancelled ) );
        m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleExitOrderFilled ) );
        m_ceLongExit.AddLabel( bar.DateTime(), bar.Close(), "LxS2-" + boost::lexical_cast<std::string>( m_pOrder->GetOrderId() ) );
        m_stateTrade = EStateTrade::LongExitSubmitted;
        m_pPosition->PlaceOrder( m_pOrder );
        ShowOrder( m_pOrder );
        break;
      case ou::tf::OrderSide::EOrderSide::Sell:
        m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
        m_pOrder->SetSignalPrice( bar.Close() );
        m_pOrder->SetDescription(
            m_sProfitDescription + ","
          + boost::lexical_cast<std::string>( m_dblProfitMin ) + ","
          + boost::lexical_cast<std::string>( m_dblUnRealized ) + ","
          + boost::lexical_cast<std::string>( m_dblProfitMax - m_dblUnRealized ) + ","
          + boost::lexical_cast<std::string>( m_dblProfitMax )
          );
        m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleExitOrderCancelled ) );
        m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleExitOrderFilled ) );
        m_ceShortExit.AddLabel( bar.DateTime(), bar.Close(), "SxS2-" + boost::lexical_cast<std::string>( m_pOrder->GetOrderId() ) );
        m_stateTrade = EStateTrade::ShortExitSubmitted;
        m_pPosition->PlaceOrder( m_pOrder );
        ShowOrder( m_pOrder );
        break;
      default:
        assert( false ); // maybe check for unknown
    }
  }
  else {
    m_stateTrade = EStateTrade::Search;
  }
}

void Strategy::ShowOrder( pOrder_t pOrder ) {
  //m_pTreeItemOrder = m_pTreeItemSymbol->AppendChild(
  //    "Order "
  //  + boost::lexical_cast<std::string>( m_pOrder->GetOrderId() )
  //  );
}

void Strategy::HandleRHTrading( const ou::tf::Bar& bar ) { // once a second

  //const std::chrono::time_point<std::chrono::system_clock> begin
  //  = std::chrono::system_clock::now();

  //m_FeatureSet.FVS(). // use imbalance, and others

  ptime dt( bar.DateTime() );

  EMovingAverage stateMovingAverage( m_stateMovingAverage );
  EMovingAverage currentMovingAverage( EMovingAverage::Flat );

  const double ma1( m_vMovingAverage[0].Latest() );
  const double ma2( m_vMovingAverage[1].Latest() );
  const double ma3( m_vMovingAverage[2].Latest() );

  if ( ( ma1 > ma2 ) && ( ma2 > ma3 ) ) {
    currentMovingAverage = EMovingAverage::Rising;
    stateMovingAverage = ( currentMovingAverage == stateMovingAverage ) ? EMovingAverage::Rising : EMovingAverage::ToRising;
    if ( !m_bUseMARising ) m_bUseMARising = true;
  }
  else {
    if ( ( ma1 < ma2 ) && ( ma2 < ma3 ) ) {
      currentMovingAverage = EMovingAverage::Falling;
      stateMovingAverage = ( currentMovingAverage == stateMovingAverage ) ? EMovingAverage::Falling : EMovingAverage::ToFalling;
      if ( !m_bUseMAFalling ) m_bUseMAFalling = true;
    }
    else {
      assert( EMovingAverage::Flat == currentMovingAverage );
      if ( EMovingAverage::Flat != stateMovingAverage ) {
        stateMovingAverage = EMovingAverage::ToFlat;
      }
    }
  }

  m_stateMovingAverage = currentMovingAverage; // not stateMovingAverage

  EStateDesired stateDesired( EStateDesired::Continue );

  switch ( m_stateStochastic) {
    case EStateStochastic::Init:
      // wait for another crossing
      break;
    case EStateStochastic::AboveHi:
      switch ( m_stableStochastic ) {
        case EStateStochastic::AboveHi:
          break;
        case EStateStochastic::AboveMid:
        case EStateStochastic::BelowMid:
        case EStateStochastic::BelowLo:
          // exit & go short
          stateDesired = EStateDesired::GoShortHi;
          break;
      }
      break;
    case EStateStochastic::AboveMid:
      switch ( m_stableStochastic ) {
        case EStateStochastic::AboveHi:
          // go/continue long
          stateDesired = EStateDesired::GoLongHi;  // many are not successful, try short instead? (some sort of stop)
          //stateDesired = EStateDesired::GoShortHi;  // 2022/12/31 from ehlers, and use time based stop (average width of stochastic edge)
          break;
        case EStateStochastic::BelowMid:
          // exit & go short - maybe
          //desired = EStateDesired::Exit;
          break;
      }
      break;
    case EStateStochastic::BelowMid:
      switch ( m_stableStochastic ) {
        case EStateStochastic::AboveMid:
          // exit & go long - maybe
          //desired = EStateDesired::Exit;
          break;
        case EStateStochastic::BelowLo:
          // go/continue short
          stateDesired = EStateDesired::GoShortLo;  // many are not successful, try long instead? (some sort of stop)
          //stateDesired = EStateDesired::GoLongLo;  // 2022/12/31 from ehlers, and use time based stop (average width of stochastic edge)
          break;
      }
      break;
    case EStateStochastic::BelowLo:
      switch ( m_stableStochastic ) {
        case EStateStochastic::AboveHi:
        case EStateStochastic::AboveMid:
        case EStateStochastic::BelowMid:
          // exit & go long
          stateDesired = EStateDesired::GoLongLo;
          break;
        case EStateStochastic::BelowLo:
          break;
      }
      break;
  }

  switch ( m_stateTrade ) {
    case EStateTrade::Search:
      switch ( stateDesired ) {
        case EStateDesired::GoLongHi:
        case EStateDesired::GoLongLo:
          BOOST_LOG_TRIVIAL(info) << dt << " Search->GoLong";
          m_bUseMARising = ( EMovingAverage::Rising == currentMovingAverage );
          m_sProfitDescription = "l,srch";
          EnterLong( bar );
          break;
        case EStateDesired::GoShortHi:
        case EStateDesired::GoShortLo:
          BOOST_LOG_TRIVIAL(info) << dt << " Search->GoShort";
          m_bUseMAFalling = ( EMovingAverage::Falling == currentMovingAverage );
          m_sProfitDescription = "s,srch";
          EnterShort( bar );
          break;
      }
      break;
    case EStateTrade::LongSubmitted:
      // wait for order to execute
      break;
    case EStateTrade::LongExitSignal:
      switch ( stateDesired ) {
        case EStateDesired::GoShortHi:
        case EStateDesired::GoShortLo:
          {
            bool bGoShort( true );
            if ( m_bUseMARising ) {
              if ( EMovingAverage::Rising == currentMovingAverage ) {
                BOOST_LOG_TRIVIAL(info) << dt << " LongExitSignal - still strong long";
                bGoShort = false;
              }
            }
            if ( bGoShort ) {
              BOOST_LOG_TRIVIAL(info) << dt << " LongExitSignal->GoShort";
              if ( EStateDesired::GoShortHi == stateDesired ) {
                m_sProfitDescription += ",x,shrthi";
              }
              if ( EStateDesired::GoShortLo == stateDesired ) {
                m_sProfitDescription += ",x,shrtlo";
              }
              ExitPosition( bar );
              m_bUseMAFalling = EMovingAverage::Falling == currentMovingAverage;
              m_sProfitDescription = "s,go";
              EnterShort( bar );
            }
          }
          break;
        case EStateDesired::Exit: // not currently reachable
          BOOST_LOG_TRIVIAL(info) << dt << " LongExitSignal->Exit";
          ExitLong( bar );
          break;
        case EStateDesired::GoLongHi:
        case EStateDesired::GoLongLo:
          BOOST_LOG_TRIVIAL(info)
            << dt << " LongExitSignal->GoLong:"
            << (int)stateDesired
            << "," << (int)m_stableStochastic << "," << (int)m_stateStochastic
            ; // already long
          break;
        case EStateDesired::Continue:
          if ( -1.0 >= m_dblUnRealized ) { // ehlers - use time instead?
            BOOST_LOG_TRIVIAL(info) << dt << " LongExitSignal->Continue(Stop)";
            m_sProfitDescription += ",x,stop";
            ExitPosition( bar );
          }
          else {
            if ( m_bUseMARising ) {
              if ( EMovingAverage::Rising == currentMovingAverage ) {
                //BOOST_LOG_TRIVIAL(info) << dt << " LongExitSignal->Continue";
              }
              else {
                BOOST_LOG_TRIVIAL(info) << dt << " LongExitSignal->Continue(Exit)";
                m_sProfitDescription += ",x,cont.exit";
                ExitPosition( bar );
                //m_bUseMAFalling = EMovingAverage::Falling == currentMovingAverage;
                //m_sProfitDescription = "s,cont";
                //EnterShort( bar );
              }
            }
          }
          break;
        default:
          assert( false );  // broken state machine
      }
      break;
    case EStateTrade::ShortSubmitted:
      // wait for order to execute
      break;
    case EStateTrade::ShortExitSignal:
      switch ( stateDesired ) {
        case EStateDesired::GoLongHi:
        case EStateDesired::GoLongLo:
          {
            bool bGoLong( true );
            if ( m_bUseMAFalling ) {
              if ( EMovingAverage::Falling == currentMovingAverage ) {
                BOOST_LOG_TRIVIAL(info) << dt << " ShortExitSignal - still strong short";
                bGoLong = false;
              }
            }
            if ( bGoLong ) {
              BOOST_LOG_TRIVIAL(info) << dt << " ShortExitSignal->GoLong";
              if ( EStateDesired::GoLongHi == stateDesired ) {
                m_sProfitDescription += ",x,longhi";
              }
              if ( EStateDesired::GoLongLo == stateDesired ) {
                m_sProfitDescription += ",x,longlo";
              }
              ExitPosition( bar );
              m_bUseMARising = EMovingAverage::Rising == currentMovingAverage;
              m_sProfitDescription = "l,go";
              EnterLong( bar );
            }
          }
          break;
        case EStateDesired::Exit: // not currently reachable
          BOOST_LOG_TRIVIAL(info) << dt << " ShortExitSignal->Exit";
          ExitShort( bar );
          break;
        case EStateDesired::GoShortHi:
        case EStateDesired::GoShortLo:
          BOOST_LOG_TRIVIAL(info)
            << dt << " ShortExitSignal->GoShort:"
            << (int)stateDesired
            << "," << (int)m_stableStochastic << "," << (int)m_stateStochastic
            ; // already short
          break;
        case EStateDesired::Continue:
          if ( -1.0 >= m_dblUnRealized ) { // ehlers - use time instead?
            BOOST_LOG_TRIVIAL(info) << dt << " ShortExitSignal->Continue(Stop)";
            m_sProfitDescription += ",x,stop";
            ExitPosition( bar );
          }
          else {
            if ( m_bUseMAFalling ) {
              if ( EMovingAverage::Falling == currentMovingAverage ) {
                //BOOST_LOG_TRIVIAL(info) << dt << " ShortExitSignal->Continue";
              }
              else {
                BOOST_LOG_TRIVIAL(info) << dt << " ShortExitSignal->Continue(Exit)";
                m_sProfitDescription += ",x,cont.exit";
                ExitPosition( bar );
                //m_bUseMARising = EMovingAverage::Rising == currentMovingAverage;
                //m_sProfitDescription = "l,cont";
                //EnterLong( bar );
              }
            }
          }
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
      // market open statistics management here
      if ( ( m_vStochastic[0]->Latest() > m_vStochastic[1]->Latest() )
        && ( m_vStochastic[1]->Latest() > m_vStochastic[2]->Latest() )
      ) {
        m_stateTrade = EStateTrade::Search;
        m_stateStochastic = EStateStochastic::Init;
      }
      else {
        if ( ( m_vStochastic[0]->Latest() < m_vStochastic[1]->Latest() )
          && ( m_vStochastic[1]->Latest() < m_vStochastic[2]->Latest() )
        ) {
          m_stateTrade = EStateTrade::Search;
          m_stateStochastic = EStateStochastic::Init;
        }
      }
      break;
  }

  m_stateStochastic = m_stableStochastic;

  //const std::chrono::time_point<std::chrono::system_clock> end
  //  = std::chrono::system_clock::now();

   //auto delta = std::chrono::duration_cast<std::chrono::microseconds>( end - begin).count();
   //auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( end - begin).count();
   //m_ceExecutionTime.Append( bar.DateTime(), delta );

}

void Strategy::HandleOrderCancelled( const ou::tf::Order& order ) {
  m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  switch ( m_stateTrade ) {
    case EStateTrade::EndOfDayCancel:
    case EStateTrade::EndOfDayNeutral:
      BOOST_LOG_TRIVIAL(info) << "order " << order.GetOrderId() << " cancelled - end of day";
      break;
    case EStateTrade::LongExitSubmitted:
    case EStateTrade::ShortExitSubmitted:
      //assert( false );  // TODO: need to figure out a plan to retry exit
      BOOST_LOG_TRIVIAL(error) << "order " << order.GetOrderId() << " cancelled - state machine needs fixes";
      m_stateTrade = EStateTrade::Done;
      break;
    default:
      m_stateTrade = EStateTrade::Search;
  }
  m_pOrder.reset();
}

void Strategy::HandleOrderFilled( const ou::tf::Order& order ) {
  m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
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
  m_pOrder.reset();
}

void Strategy::HandleExitOrderCancelled( const ou::tf::Order& order ) {
  ou::tf::Order& order_( const_cast<ou::tf::Order&>( order ) );
  order_.OnOrderCancelled.Remove( MakeDelegate( this, &Strategy::HandleExitOrderCancelled ) );
  order_.OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleExitOrderFilled ) );

  switch ( order.GetOrderSide() ) {
    case ou::tf::OrderSide::EOrderSide::Buy: // is dt filled a real time?
      //m_ceLongExit.AddLabel( order.GetDateTimeOrderFilled(), order.GetSignalPrice(), "LxC-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      m_stateTrade = EStateTrade::Cancelled;  // or use cancelled for custom processing
      break;
    case ou::tf::OrderSide::EOrderSide::Sell: // is dt filled a real time?
      //m_ceShortExit.AddLabel( order.GetDateTimeOrderFilled(), order.GetSignalPrice(), "SxC-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      m_stateTrade = EStateTrade::Cancelled;  // or use cancelled for custom processing
      break;
    default:
      assert( false );
  }
}

void Strategy::HandleExitOrderFilled( const ou::tf::Order& order ) {
  ou::tf::Order& order_( const_cast<ou::tf::Order&>( order ) );
  order_.OnOrderCancelled.Remove( MakeDelegate( this, &Strategy::HandleExitOrderCancelled ) );
  order_.OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleExitOrderFilled ) );

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

void Strategy::HandleCancel( boost::gregorian::date, boost::posix_time::time_duration ) { // one shot
  m_stateTrade = EStateTrade::EndOfDayCancel;
  if ( m_pPosition ) {
    m_pPosition->CancelOrders();
  }
}

void Strategy::HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration ) { // one shot
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

void Strategy::SaveWatch( const std::string& sPrefix ) {
  // RecordSeries has been set to false
  if ( m_pPosition ) {
    m_pPosition->GetWatch()->SaveSeries( sPrefix );
    //if (m_pFile){ // don't do this, as the file is save on exit,
    //  this will create another version, which will cause problems during reload
    //  m_pFile->Write();
    //}
  }
}

void Strategy::CloseAndDone() {
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
