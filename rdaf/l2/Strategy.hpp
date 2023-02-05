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
 * File:    Strategy.hpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: December 9, 2022  16:18:19
 */

#pragma once

#ifdef RDAF
#undef RDAF
#endif

#ifdef FVS
#undef FVS
#endif

#include <fstream>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryMark.h>
#include <OUCharting/ChartEntryShape.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <OUCharting/ChartDataView.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>
#include <TFTrading/DailyTradeTimeFrames.h>

#include <TFIQFeed/Level2/Symbols.hpp>
#include <TFIQFeed/Level2/FeatureSet.hpp>

#include <TFBitsNPieces/Stochastic.hpp>
#include <TFBitsNPieces/MovingAverage.hpp>

#include "ConfigParser.hpp"

#if RDAF
class TH2D;
class TFile;
class TTree;
class TClass;
#endif

namespace ou {
  class ChartDataView;
namespace tf {
  class TreeItem;
} // namespace tf
} // namespace ou

using pOrder_t = ou::tf::Order::pOrder_t;

// =========

class Cycle {
public:
  using fDone_t = std::function<void()>;
  Cycle( pOrder_t pEntry, pOrder_t pExit, fDone_t&& fDone )
  : m_pOrderEntry( std::move( pEntry ) )
  , m_pOrderExit( std::move( pExit ) )
  , m_fDone( std::move( fDone ) )
  {}
  void PlaceEntry() {}
  void PlaceExit() {}
protected:
private:
  pOrder_t m_pOrderEntry;
  pOrder_t m_pOrderExit;
  fDone_t m_fDone;
};

// =========

class Strategy:
  public ou::tf::DailyTradeTimeFrame<Strategy>
{
  friend class boost::serialization::access;
  friend ou::tf::DailyTradeTimeFrame<Strategy>;
public:

  using price_t = ou::tf::Trade::price_t;
  using volume_t = ou::tf::Trade::volume_t;

  using TreeItem = ou::tf::TreeItem;
  using pPosition_t = ou::tf::Position::pPosition_t;
  //using pFile_t = std::shared_ptr<TFile>;

  Strategy(
    const ou::tf::config::symbol_t&
  , TreeItem*
#if RDAF
  , pFile_t
  , pFile_t
#endif
  );
  virtual ~Strategy();

  const ou::tf::config::symbol_t& Choices() const { return m_config; }

  void SetPosition( pPosition_t );

  void FVSStreamStart( const std::string& sPath );
  void FVSStreamStop( int );

  ou::tf::config::symbol_t::EFeed Feed() const { return m_config.eFeed; }

#if RDAF
  void LoadHistory( TClass* );
#endif

  void HandleUpdateL2Ask( price_t price, volume_t volume, bool bAdd );
  void HandleUpdateL2Bid( price_t price, volume_t volume, bool bAdd );

  void SaveWatch( const std::string& );

  ou::ChartDataView& GetChartDataView() { return m_cdv; }

  void CloseAndDone();

protected:
private:

  enum EChartSlot { Price, Volume, Cycle, CycleSlope, SD, MASlope, MA, ImbalanceMean, Stoch, FVS_v8_rel, PL1, PL2, ET, MarketDepth };

  enum class EStateTrade {
    Init,  // initiaize state in current market
    Search,  // looking for long or short enter
    LongSubmitted, // order has been submitted, waiting for confirmation
    LongExitSignal,  // position exists, looking for exit
    ShortSubmitted,  // order has been submitted, waiting for confirmtaion
    ShortExitSignal,  // position exists, looking for exit
    LongExitSubmitted, // wait for exit to complete
    ShortExitSubmitted, // wait for exit to complete
    Cancelling,
    Cancelled,
    NoTrade, // from the config file, no trading, might be a future
    EndOfDayCancel,
    EndOfDayNeutral,
    Done // no more action
    };

  enum class EStateDesired {
    Continue
  , GoLong
  , GoShort
  , Cancel
  , Exit
  };

  EStateTrade m_stateTrade;

  TreeItem* m_pTreeItemSymbol;
  TreeItem* m_pTreeItemOrder;

  ou::tf::Quote m_quote;
  ou::tf::Quotes m_quotes; // used for stochastics, as storage for watch is off

  const ou::tf::config::symbol_t& m_config;

#if RDAF
  pFile_t m_pFile;
  pFile_t m_pFileUtility;
#endif

  pPosition_t m_pPosition;

  pOrder_t m_pOrderPending;
  //pOrder_t m_pOrderDone;

  ou::ChartDataView m_cdv;

  ou::ChartEntryIndicator m_ceQuoteAsk;
  ou::ChartEntryIndicator m_ceQuoteBid;

  ou::ChartEntryIndicator m_ceTrade;
  ou::ChartEntryVolume m_ceVolume;

  ou::ChartEntryShape m_ceLongEntry;
  //ou::ChartEntryShape m_ceLongFill;
  ou::ChartEntryShape m_ceLongExit;
  ou::ChartEntryShape m_ceShortEntry;
  //ou::ChartEntryShape m_ceShortFill;
  ou::ChartEntryShape m_ceShortExit;

  ou::ChartEntryIndicator m_ceProfitUnRealized;
  ou::ChartEntryIndicator m_ceProfitRealized;
  ou::ChartEntryIndicator m_ceCommissionsPaid;
  ou::ChartEntryIndicator m_ceProfit;

  ou::ChartEntryIndicator m_cdMarketDepthAsk;
  ou::ChartEntryIndicator m_cdMarketDepthBid;

  ou::ChartEntryIndicator m_ceSkewness;
  ou::ChartEntryIndicator m_ceExecutionTime;

  ou::ChartEntryIndicator m_ceImbalanceRawMean;
  ou::ChartEntryIndicator m_ceImbalanceSmoothMean;

  ou::ChartEntryIndicator m_ceRelativeMA1;
  ou::ChartEntryIndicator m_ceRelativeMA2;
  ou::ChartEntryIndicator m_ceRelativeMA3;
#if FVS
  ou::ChartEntryIndicator m_ceFVS_Ask_Lvl1RelLmt;
  ou::ChartEntryIndicator m_ceFVS_Ask_Lvl1RelMkt;
  ou::ChartEntryIndicator m_ceFVS_Ask_Lvl1RelCncl;
  ou::ChartEntryIndicator m_ceFVS_Bid_Lvl1RelLmt;
  ou::ChartEntryIndicator m_ceFVS_Bid_Lvl1RelMkt;
  ou::ChartEntryIndicator m_ceFVS_Bid_Lvl1RelCncl;
#endif
  ou::ChartEntryMark m_cemZero;

  ou::tf::BarFactory m_bfQuotes01Sec;

  using pMarketMaker_t = ou::tf::iqfeed::l2::MarketMaker::pMarketMaker_t;
  pMarketMaker_t m_pMarketMaker;

  using pOrderBased_t = ou::tf::iqfeed::l2::OrderBased::pOrderBased_t;
  pOrderBased_t m_pOrderBased;

  std::atomic_uint32_t m_nMarketOrdersAsk; // pull from InteractiveChart
  std::atomic_uint32_t m_nMarketOrdersBid; // pull from InteractiveChart

  double m_dblImbalanceMean, m_dblImbalanceSlope;

  using vMovingAverageSlope_t = std::vector<ou::tf::MovingAverageSlope>;
  vMovingAverageSlope_t m_vMovingAverageSlope;

  // https://github.com/rburkholder/tf2/commit/8a9ed856d16b744df6becbe7ec6a18eb5df52644
  using pStochastic_t = std::unique_ptr<Stochastic>;
  using vStochastic_t = std::vector<pStochastic_t>;
  vStochastic_t m_vStochastic;

  //ou::tf::Prices m_pricesStochastic;
  //ou::tf::hf::TSEMA<ou::tf::Price> m_emaStochastic;

  ou::ChartEntryMark m_cemStochastic;
  //ou::ChartEntryIndicator m_ceStochastic;

  struct HiPass {

    ou::ChartEntryIndicator m_ceEhlersHiPassFilter;
    ou::ChartEntryIndicator m_ceEhlersHiPassFilterSlope;
    //ou::ChartEntryIndicator m_ceEhlersLoPassFilter;

    // ehlers, cybernetic analsys, eqn 2.7, eqn 2.9
    double m_alpha;
    double m_one_minus_alpha;
    double m_alpha_by_two;
    double m_one_minus_alpha_by_two;
    double m_alpha_squared;

    double m_dblPrice0, m_dblPrice1, m_dblPrice2;
    double m_dblHPF0, m_dblHPF1, m_dblHPF2;
    //double m_dblLPF0, m_dblLPF1, m_dblLPF2;

    double m_dblHPF_Slope0, m_dblHPF_Slope1;

    HiPass()
    : m_alpha( 0.1 )
    , m_dblPrice0 {}, m_dblPrice1 {}, m_dblPrice2 {}
    , m_dblHPF0 {}, m_dblHPF1 {}, m_dblHPF2 {}
    //, m_dblLPF0 {}, m_dblLPF1 {}, m_dblLPF2 {}
    , m_dblHPF_Slope0 {}, m_dblHPF_Slope1 {}
    {}

    void Init( int nPeriods, ou::Colour::EColour colour, const std::string& sName ) {

      m_alpha = 1.0 / (double) nPeriods;
      m_one_minus_alpha = 1.0 - m_alpha;
      m_alpha_by_two = m_alpha / 2.0;
      m_one_minus_alpha_by_two = 1.0 - m_alpha_by_two;
      m_alpha_squared = m_alpha * m_alpha;

      m_ceEhlersHiPassFilter.SetColour( colour );
      m_ceEhlersHiPassFilterSlope.SetColour( colour );
      //m_ceEhlersLoPassFilter.SetColour( colour );

      m_ceEhlersHiPassFilter.SetName( sName );
      m_ceEhlersHiPassFilterSlope.SetName( sName + "Slope" );
      //m_ceEhlersLoPassFilter.SetName( "LoPass" );

    }

    void Update( boost::posix_time::ptime dt, double price ) {
      // ehlers page 15, eqn 2.7, high pass filter
      // ehlers page 16, eqn 2.9, low pass filter

      if ( 0.0 == m_dblPrice0 ) {
        m_dblPrice0 = m_dblPrice1 = m_dblPrice2 = price;
        //m_dblHPF0 = m_dblHPF1 = m_dblHPF2 = 0.0;
        //m_dblLPF0 = m_dblLPF1 = m_dblLPF2 = ma0;
      }
      else {
        m_dblPrice2 = m_dblPrice1; m_dblPrice1 = m_dblPrice0; // archive older values
        m_dblPrice0 = price;
        const double weighted = m_dblPrice0 - ( m_dblPrice1 + m_dblPrice1 ) + m_dblPrice2;

        m_dblHPF2 = m_dblHPF1; m_dblHPF1 = m_dblHPF0; // archive older values

        m_dblHPF0 = m_one_minus_alpha_by_two * m_one_minus_alpha_by_two * weighted
                  + 2.0 * m_one_minus_alpha * m_dblHPF1
                  - m_one_minus_alpha * m_one_minus_alpha * m_dblHPF2
                  ;
        m_ceEhlersHiPassFilter.Append( dt, m_dblHPF0 );

        m_dblHPF_Slope1 = m_dblHPF_Slope0; // keep previous value
        m_dblHPF_Slope0 = m_dblHPF0 - m_dblHPF1; // diff is slope
        m_ceEhlersHiPassFilterSlope.Append( dt, m_dblHPF_Slope0 );

        //if ( 10 > m_ceEhlersHiPassFilter.Size() ) {
        //  BOOST_LOG_TRIVIAL(info) << "hpf=" << m_dblPrice0 << "," << m_dblHPF0 << "," << m_dblHPF1 << "," << m_dblHPF2 << std::endl;
        //}

        // only marginally better than a simple ema
        //m_dblLPF2 = m_dblLPF1; m_dblLPF1 = m_dblLPF0;
        //m_dblLPF0 = ( m_alpha - m_alpha_by_two * m_alpha_by_two ) * m_dblPrice0
        //          + ( m_alpha_squared / 2 ) * m_dblPrice1
        //          - ( m_alpha - 3.0 * m_alpha_squared / 4.0 ) * m_dblPrice2
        //          + 2.0 * m_one_minus_alpha * m_dblLPF1
        //          - m_one_minus_alpha * m_one_minus_alpha * m_dblLPF2
        //          ;
        //m_ceEhlersLoPassFilter.Append( dt, m_dblLPF0 );
      }

    }

  };

  HiPass m_rHiPass[4];

  struct State {

    enum class EValue { bid, ask, ma0, ma1, ma2, ma3 };

    using vRelative_t = std::vector<EValue>;
    using mapRelative_t = std::map<double,vRelative_t>;

    using rEValue_t = std::array<EValue,6>;
    rEValue_t rEValue;

    double slope0 {};
    double slope1 {};
    double slope2 {};
    double slope3 {};

    double stop {};

    void Insert( mapRelative_t& map, EValue e, const double value ) {
      mapRelative_t::iterator iter = map.find( value );
      if ( map.end() == iter ) {
        auto pair =  map.emplace( mapRelative_t::value_type( value, std::move( vRelative_t() ) ));
        assert( pair.second );
        iter = pair.first;
      }
      iter->second.push_back( e );
    }

    State(
      const double bid, const double ask
    , const double ma0, const double ma1, const double ma2, const double ma3
    , const double slope0_, const double slope1_, const double slope2_, const double slope3_
    )
    : slope0( slope0_ ), slope1( slope1_ ), slope2( slope2_ ), slope3( slope3_ )
    {
      mapRelative_t mapRelative;

      Insert( mapRelative, EValue::bid, bid );
      Insert( mapRelative, EValue::ask, ask );
      Insert( mapRelative, EValue::ma0, ma0 );
      Insert( mapRelative, EValue::ma1, ma1 );
      Insert( mapRelative, EValue::ma2, ma2 );
      Insert( mapRelative, EValue::ma3, ma3 );

      stop = mapRelative.rbegin()->first - mapRelative.begin()->first;

      // TODO:
      //   ratio of mid two related to min/max
      //   slope > 0, slope < 0

      //rEValue_t::iterator iterEValue( rEValue.begin() );
      //for ( const mapRelative_t::value_type& vt_map: mapRelative ) {
      //  for ( const vRelative_t::value_type vt_vector: vt_map.second ) {
      //    *iterEValue = vt_vector;
      //    iterEValue++;
      //  }
      //}

    }

    bool operator==( const State& rhs ) {
      bool bResult( true );
      rEValue_t::const_iterator iterLhs = rEValue.begin();
      rEValue_t::const_iterator iterRhs = rhs.rEValue.begin();
      for (
        ;
        iterLhs != rEValue.end();
        iterLhs++, iterRhs++
      ) {
        bResult &= ( *iterLhs == *iterRhs );
      }
      return bResult;
    }

    double Stop() const { return stop; }

    bool EnterLong() const {
      //return (
        //   ( EValue::bid == rEValue[ 4 ] )
      //     ( EValue::ma0 == rEValue[ 3 ] )
      //  && ( EValue::ma1 == rEValue[ 2 ] )
      //  && ( EValue::ma2 == rEValue[ 1 ] )
      //  && ( EValue::ma3 == rEValue[ 0 ] )
      //);
      return (
        ( 0.0 < slope0 ) && ( 0.0 < slope1 ) && ( 0.0 < slope2 ) && ( 0.0 < slope3 )
      );
    }

    bool EnterShort() const {
      //return (
         //   ( EValue::ask == rEValue[ 1 ] )
      //      ( EValue::ma0 == rEValue[ 0 ] )
      //   && ( EValue::ma1 == rEValue[ 1 ] )
      //   && ( EValue::ma2 == rEValue[ 2 ] )
      //   && ( EValue::ma3 == rEValue[ 3 ] )
      //);
      return (
        ( 0.0 > slope0 ) && ( 0.0 > slope1 ) && ( 0.0 > slope2 ) && ( 0.0 > slope3 )
      );
    }
  };

  double m_dblStopDeltaProposed;
  double m_dblStopActiveDelta;
  double m_dblStopActiveActual;

  std::string m_sProfitDescription;
  double m_dblProfitMax;
  double m_dblUnRealized;
  double m_dblProfitMin;

#if RDAF
  // ==
  struct QuoteForBranch {
    double time;
    double ask;
    uint64_t askvol;
    double bid;
    uint64_t bidvol;
  } m_branchQuote;

  struct TradeForBranch {
    double time;
    double price;
    uint64_t vol;
    int64_t direction;
  } m_branchTrade;

  // https://root.cern/doc/master/classTTree.html
  using pTTree_t = std::shared_ptr<TTree>;
  pTTree_t m_pTreeQuote;
  pTTree_t m_pTreeTrade;

  using pTH2D_t = std::shared_ptr<TH2D>;
  pTH2D_t m_pHistVolume;
  pTH2D_t m_pHistVolumeDemo;
#endif

  ou::tf::iqfeed::l2::FeatureSet m_FeatureSet;
  std::string m_sFVSPath;
  std::ofstream m_streamFVS;

  void InitRdaf();

  void StartDepthByOrder();
  void Imbalance( const ou::tf::Depth& );

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );
  void HandleDepthByMM( const ou::tf::DepthByMM& );
  void HandleDepthByOrder( const ou::tf::DepthByOrder& );

  void HandleBarQuotes01Sec( const ou::tf::Bar& );

  void HandleRHTrading( const ou::tf::Quote& );

  void HandleRHTrading( const ou::tf::Bar& );
  void HandleCancel( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration );

  void EnterLong( const ou::tf::Quote& );
  void EnterShort( const ou::tf::Quote& );

  void ExitLong( const ou::tf::Quote& );
  void ExitShort( const ou::tf::Quote& );

  void ExitPosition( const ou::tf::Quote& );

  void ShowOrder( pOrder_t );

  void HandleOrderCancelled( const ou::tf::Order& );
  void HandleOrderFilled( const ou::tf::Order& );

  void HandleExitOrderCancelled( const ou::tf::Order& );
  void HandleExitOrderFilled( const ou::tf::Order& );

  void Clear();
  void SetupChart();

};
