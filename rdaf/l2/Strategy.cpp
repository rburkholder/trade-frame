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
, m_stateDesired( EStateDesired::Continue )
, m_stateTrade( EStateTrade::Init )
, m_stateStochastic( EStateStochastic::Init )
, m_config( config )
, m_ceLongEntry( ou::ChartEntryShape::EShape::Long, ou::Colour::Blue )
, m_ceLongFill( ou::ChartEntryShape::EShape::FillLong, ou::Colour::Blue )
, m_ceLongExit( ou::ChartEntryShape::EShape::LongStop, ou::Colour::Blue )
, m_ceShortEntry( ou::ChartEntryShape::EShape::Short, ou::Colour::Red )
, m_ceShortFill( ou::ChartEntryShape::EShape::FillShort, ou::Colour::Red )
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
  m_cemStochastic.AddMark(  80, ou::Colour::Red,   "80%" );
  m_cemStochastic.AddMark(  50, ou::Colour::Green, "50%" );
  m_cemStochastic.AddMark(  20, ou::Colour::Blue,  "20%" );
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

}

Strategy::~Strategy() {
  Clear();
}

void Strategy::SetupChart() {

  m_cdv.Add( EChartSlot::Price, &m_ceQuoteAsk );
  m_cdv.Add( EChartSlot::Price, &m_ceTrade );
  m_cdv.Add( EChartSlot::Price, &m_ceQuoteBid );

  m_cdv.Add( EChartSlot::Price, &m_ceLongEntry );
  m_cdv.Add( EChartSlot::Price, &m_ceLongFill );
  m_cdv.Add( EChartSlot::Price, &m_ceLongExit );
  m_cdv.Add( EChartSlot::Price, &m_ceShortEntry );
  m_cdv.Add( EChartSlot::Price, &m_ceShortFill );
  m_cdv.Add( EChartSlot::Price, &m_ceShortExit );

  m_cdv.Add( EChartSlot::Volume, &m_ceVolume );

  m_cdv.Add( EChartSlot::Stoch, &m_cemStochastic );

  for ( vStochastic_t::value_type& vt: m_vStochastic ) {
    vt->AddToView( m_cdv, EChartSlot::Price, EChartSlot::Stoch );
  }

  m_cdv.Add( EChartSlot::ImbalanceMean, &m_cemZero );

  m_cdv.Add( EChartSlot::ImbalanceMean, &m_ceImbalanceRawMean );
  m_ceImbalanceRawMean.SetName( "imbalance mean" );
  m_ceImbalanceRawMean.SetColour( ou::Colour::LightGreen );

  m_cdv.Add( EChartSlot::ImbalanceMean, &m_ceImbalanceSmoothMean );
  m_ceImbalanceSmoothMean.SetColour( ou::Colour::DarkGreen );

  //m_cdv.Add( EChartSlot::Skew, &m_ceSkewness );

  m_cdv.Add( EChartSlot::PL, &m_ceProfitUnRealized );
  m_cdv.Add( EChartSlot::PL, &m_ceProfitRealized );
  m_cdv.Add( EChartSlot::PL, &m_ceCommissionsPaid );
  m_cdv.Add( EChartSlot::PL, &m_ceProfit );

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
        Imbalance( depth );
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
        Imbalance( depth );
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

  double dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal;

  m_pPosition->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );

  m_ceProfitUnRealized.Append( bar.DateTime(), dblUnRealized );
  m_ceProfitRealized.Append( bar.DateTime(), dblRealized );
  m_ceCommissionsPaid.Append( bar.DateTime(), dblCommissionsPaid );
  m_ceProfit.Append( bar.DateTime(), dblTotal );

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
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceLongEntry.AddLabel( bar.DateTime(), bar.Close(), "Long Submit" );
  m_stateTrade = EStateTrade::LongSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
  ShowOrder( m_pOrder );
}

void Strategy::EnterShort( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceShortEntry.AddLabel( bar.DateTime(), bar.Close(), "Short Submit" );
  m_stateTrade = EStateTrade::ShortSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
  ShowOrder( m_pOrder );
}

void Strategy::ExitLong( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceLongExit.AddLabel( bar.DateTime(), bar.Close(), "Long Exit Submit" );
  m_stateTrade = EStateTrade::LongExitSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
  ShowOrder( m_pOrder );
}

void Strategy::ExitShort( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceShortExit.AddLabel( bar.DateTime(), bar.Close(), "Short Exit Submit" );
  m_stateTrade = EStateTrade::ShortExitSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
  ShowOrder( m_pOrder );
}

void Strategy::ShowOrder( pOrder_t pOrder ) {
  m_pTreeItemOrder = m_pTreeItemSymbol->AppendChild(
      "Order "
    + boost::lexical_cast<std::string>( m_pOrder->GetOrderId() )
    );
}

void Strategy::HandleRHTrading( const ou::tf::Bar& bar ) { // once a second

  //const std::chrono::time_point<std::chrono::system_clock> begin
  //  = std::chrono::system_clock::now();

  //m_FeatureSet.FVS(). // use imbalance, and others

  ptime dt( bar.DateTime() );

  EStateStochastic stateStochastic( m_stateStochastic ); // sticky until changed

  double k = m_vStochastic[2]->Latest();
  if ( k >= 50.0 ) {
    if ( k > 80.0 ) {
      stateStochastic = EStateStochastic::Above80;
    }
    else {
      stateStochastic = EStateStochastic::Above50;
    }
  }
  else {
    if ( k < 20.0 ) {
      stateStochastic = EStateStochastic::Below20;
    }
    else {
      stateStochastic = EStateStochastic::Below50;
    }
  }

  EStateDesired stateDesired( m_stateDesired ); // sticky until reset to Continue (allows cycling through exit -> entry)

  switch ( m_stateStochastic) {
    case EStateStochastic::Init:
      // wait for another crossing
      break;
    case EStateStochastic::Above80:
      switch ( stateStochastic ) {
        case EStateStochastic::Above80:
          break;
        case EStateStochastic::Above50:
        case EStateStochastic::Below50:
        case EStateStochastic::Below20:
          // exit & go short
          stateDesired = EStateDesired::GoShort;
          break;
      }
      break;
    case EStateStochastic::Above50:
      switch ( stateStochastic ) {
        case EStateStochastic::Above80:
          // go/continue long
          stateDesired = EStateDesired::GoLong;
          break;
        case EStateStochastic::Below50:
          // exit & go short - maybe
          //desired = EStateDesired::Exit;
          break;
      }
      break;
    case EStateStochastic::Below50:
      switch ( stateStochastic ) {
        case EStateStochastic::Above50:
          // exit & go long - maybe
          //desired = EStateDesired::Exit;
          break;
        case EStateStochastic::Below20:
          // go/continue short
          stateDesired = EStateDesired::GoShort;
          break;
      }
      break;
    case EStateStochastic::Below20:
      switch ( stateStochastic ) {
        case EStateStochastic::Above80:
        case EStateStochastic::Above50:
        case EStateStochastic::Below50:
          // exit & go long
          stateDesired = EStateDesired::GoLong;
          break;
        case EStateStochastic::Below20:
          break;
      }
      break;
  }

  switch ( m_stateTrade ) {
    case EStateTrade::Search:
      switch ( stateDesired ) {
        case EStateDesired::GoLong:
          BOOST_LOG_TRIVIAL(info) << dt << " Search->GoLong";
          stateDesired = EStateDesired::Continue;
          EnterLong( bar );
          break;
        case EStateDesired::GoShort:
          BOOST_LOG_TRIVIAL(info) << dt << " Search->GoShort";
          stateDesired = EStateDesired::Continue;
          EnterShort( bar );
          break;
      }
      break;
    case EStateTrade::LongSubmitted:
      // wait for order to execute
      break;
    case EStateTrade::LongExit:
      switch ( stateDesired ) {
        case EStateDesired::GoShort:
          BOOST_LOG_TRIVIAL(info) << dt << " GoShort->ExitLong";
          // stateDesired = EStateDesired::Continue; leave desire as it is
          ExitLong( bar );
          break;
        case EStateDesired::Exit:
          BOOST_LOG_TRIVIAL(info) << dt << " Exit->ExitLong";
          stateDesired = EStateDesired::Continue;
          ExitLong( bar );
          break;
        case EStateDesired::GoLong:
          BOOST_LOG_TRIVIAL(info)
            << dt << " GoLong->Continue:"
            << (int)stateDesired << "," << (int)m_stateDesired
            << "," << (int)stateStochastic << "," << (int)m_stateStochastic
            ; // already long
          stateDesired = EStateDesired::Continue;
          break;
        case EStateDesired::Continue:
          BOOST_LOG_TRIVIAL(info) << dt << " LongExit->Continue";
          break;
        default:
          assert( false );  // broken state machine
      }
      break;
    case EStateTrade::ShortSubmitted:
      // wait for order to execute
      break;
    case EStateTrade::ShortExit:
      switch ( stateDesired ) {
        case EStateDesired::GoLong:
          BOOST_LOG_TRIVIAL(info) << dt << " GoLong->ExitShort";
          // stateDesired = EStateDesired::Continue; leave desire as it is
          ExitShort( bar );
          break;
        case EStateDesired::Exit:
          BOOST_LOG_TRIVIAL(info) << dt << " Exit->ExitShort";
          stateDesired = EStateDesired::Continue;
          ExitShort( bar );
          break;
        case EStateDesired::GoShort:
          BOOST_LOG_TRIVIAL(info)
            << dt << " GoShort->Continue:"
            << (int)stateDesired << "," << (int)m_stateDesired
            << "," << (int)stateStochastic << "," << (int)m_stateStochastic
            ; // already short
          stateDesired = EStateDesired::Continue;
          break;
        case EStateDesired::Continue:
          BOOST_LOG_TRIVIAL(info) << dt << " ShortExit->Continue";
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
        m_stateDesired = EStateDesired::Continue;
        m_stateStochastic = EStateStochastic::Init;
      }
      else {
        if ( ( m_vStochastic[0]->Latest() < m_vStochastic[1]->Latest() )
          && ( m_vStochastic[1]->Latest() < m_vStochastic[2]->Latest() )
        ) {
          m_stateTrade = EStateTrade::Search;
          m_stateDesired = EStateDesired::Continue;
          m_stateStochastic = EStateStochastic::Init;
        }
      }
      break;
  }

  m_stateDesired = stateDesired;
  m_stateStochastic = stateStochastic;

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
      m_ceLongFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Long Fill" );
      m_stateTrade = EStateTrade::LongExit;
      break;
    case EStateTrade::ShortSubmitted:
      m_ceShortFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Short Fill" );
      m_stateTrade = EStateTrade::ShortExit;
      break;
    case EStateTrade::LongExitSubmitted:
      m_ceShortFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Long Exit Fill" );
      m_stateTrade = EStateTrade::Search;
      break;
    case EStateTrade::ShortExitSubmitted:
      m_ceLongFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Short Exit Fill" );
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
