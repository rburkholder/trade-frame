/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * Project: SP500
 * Created: April 14, 2025 20:32:29
 */

#pragma once

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryMark.h>
#include <OUCharting/ChartEntryShape.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <OUCharting/ChartDataView.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFIndicators/TSSWStats.h>
#include <TFIndicators/TSSWMinMax.hpp>

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>
#include <TFTrading/DailyTradeTimeFrames.h>

#include <TFBitsNPieces/TrackOrder.hpp>

#include "Flags.hpp"
#include "Features.hpp"

class Strategy
: public ou::tf::DailyTradeTimeFrame<Strategy>
{
  friend ou::tf::DailyTradeTimeFrame<Strategy>;
  friend class boost::serialization::access;
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  using fConstructedWatch_t = std::function<void( pWatch_t )>;
  using fConstructWatch_t = std::function<void( const std::string sIQFeedSymbolName, fConstructedWatch_t&& )>;

  using fConstructedPosition_t = std::function<void( pPosition_t )>;
  using fConstructPosition_t = std::function<void( const std::string sIQFeedSymbolName, fConstructedPosition_t&& )>;

  using fStart_t = std::function<void()>;
  using fStop_t = std::function<void()>;

  using fForward_t = std::function<ou::tf::Price( const Features_raw&, Features_scaled& )>;

  Strategy(
    ou::ChartDataView&
  , const Flags& // needs to be non-transient
  , fConstructWatch_t&&
  , fConstructPosition_t&&
  , fStart_t&&
  , fStop_t&&
  , fForward_t&&
  );
  ~Strategy();

  void Start();

  void PredictionVector( const size_t distance, const size_t size, const float* );

protected:
private:

  enum EChartSlot { Price, TickVolume, QuoteVolume, PriceBB, rtnPrice, rtnPriceMean, rtnPriceSlope, rtnPriceSDa, rtnPriceSDo, TickRegime, AdvDec, TickStat, Imbalance, Ratio, Predict, PredVec, PL };

  const Flags& m_flags;

  using pOrder_t = ou::tf::Order::pOrder_t;

  fConstructWatch_t m_fConstructWatch;
  fConstructPosition_t m_fConstructPosition;
  fStart_t m_fStart;
  fStop_t m_fStop;

  using pTrackOrder_t = std::unique_ptr<ou::tf::TrackOrder>;
  pTrackOrder_t m_pTrackOrder;

  pPosition_t m_pPosition;
  pWatch_t m_pTickJ;
  pWatch_t m_pTickL;
  pWatch_t m_pTrin;
  pWatch_t m_pAdv;
  pWatch_t m_pDec;

  double m_dblMid;
  //double m_dblLastTrin;

  struct VolumeWeightedPrice {

    double m_dblSumVolume;
    double m_dblSumVolumePrice;

    VolumeWeightedPrice()
    : m_dblSumVolumePrice {}, m_dblSumVolume {} {}

    void Add( double price, ou::tf::Price::volume_t volume ) {
      const double volume_( volume );
      m_dblSumVolume += volume_;
      m_dblSumVolumePrice += volume_ * price;
    }

    double operator()() {
      if ( 0.0 == m_dblSumVolume ) return 0.0;
      else {
        const double price( m_dblSumVolumePrice / m_dblSumVolume );
        m_dblSumVolumePrice = m_dblSumVolume = 0.0;
        return price;
      }
    }
  };

  VolumeWeightedPrice m_vwp;

  Features_raw m_features;

  uint64_t m_cntQuotePriceChanged;
  uint64_t m_cntQuotePriceUnchanged;

  ou::tf::Quote m_quote;
  ou::tf::Trade m_trade;

  fForward_t m_fForward;

  size_t m_nEnterLong;
  size_t m_nEnterShort;

  double m_stopInitial;
  double m_stopDelta;
  double m_stopTrail;

  double m_dblQuoteImbalance;

  ou::ChartDataView& m_cdv;

  ou::ChartEntryMark m_cemPosOne;
  ou::ChartEntryMark m_cemZero;
  ou::ChartEntryMark m_cemNegOne;
  ou::ChartEntryMark m_cemRegimMin;

  ou::ChartEntryIndicator m_ceTradePrice;
  ou::ChartEntryVolume    m_ceTradeVolume;
  ou::ChartEntryVolume    m_ceTradeVolumeUp;
  ou::ChartEntryVolume    m_ceTradeVolumeDn;

  ou::ChartEntryIndicator m_ceTradeBBU;
  ou::ChartEntryIndicator m_ceTradeBBL;
  ou::ChartEntryVolume    m_ceTradeBBDiff_vol;
  ou::ChartEntryIndicator m_ceTradeBBDiff_val;

  ou::ChartEntryIndicator m_ceTradeZigZag;

  ou::ChartEntryIndicator m_ceTradePrice_bb_ratio;
  ou::ChartEntryIndicator m_ceTradePrice_ema_bb_ratio;

  ou::ChartEntryIndicator m_ceAskPrice;
  ou::ChartEntryIndicator m_ceBidPrice;

  ou::ChartEntryVolume m_ceAskVolume;
  ou::ChartEntryVolume m_ceBidVolume;

  ou::ChartEntryIndicator m_ceEma013;
  ou::ChartEntryIndicator m_ceEma029;
  ou::ChartEntryIndicator m_ceEma050;
  ou::ChartEntryIndicator m_ceEma200;

  ou::ChartEntryMark m_cemImbalanceMarker;
  ou::ChartEntryIndicator m_ceImbalance;

  ou::ChartEntryIndicator m_ceEma013_ratio;
  ou::ChartEntryIndicator m_ceEma029_ratio;
  ou::ChartEntryIndicator m_ceTrade_ratio;
  ou::ChartEntryIndicator m_cePrediction_scaled;
  ou::ChartEntryIndicator m_cePrediction_descaled;
  ou::ChartEntryIndicator m_cePrediction_vector;

  ou::ChartEntryIndicator m_ceTickJ_sigmoid;
  ou::ChartEntryIndicator m_ceTickL_sigmoid;
  ou::ChartEntryIndicator m_ceAdvDec_ratio;

  ou::ChartEntryShape m_ceLongEntry;
  ou::ChartEntryShape m_ceLongFill;
  ou::ChartEntryShape m_ceLongExit;
  ou::ChartEntryShape m_ceShortEntry;
  ou::ChartEntryShape m_ceShortFill;
  ou::ChartEntryShape m_ceShortExit;

  ou::ChartEntryIndicator m_ceTickJ;
  ou::ChartEntryIndicator m_ceTickL;
  //ou::ChartEntryIndicator m_ceTrin;

  ou::ChartEntryVolume m_ceAdvDec;

  ou::tf::Prices m_returns;
  ou::tf::TSSWStatsPrice m_statsReturns;

  ou::tf::Prices m_prices; // might use the underlying directly?
  ou::tf::TSSWStatsPrice m_statsPrices;

  double m_dblEma013;
  double m_dblEma029;
  double m_dblEma050;
  double m_dblEma200;

  size_t m_cntOffsetUp;
  size_t m_cntOffsetDn;

  double m_dblPrvPrice; // calculating returns
  double m_dblPrvSD;

  ou::ChartEntryIndicator m_ceRtnPrice_mean;
  ou::ChartEntryIndicator m_ceRtnPrice_slope;

  ou::tf::Prices m_returns_mean;
  ou::tf::TSSWStatsPrice m_statsReturns_mean;

  ou::tf::Prices m_returns_slope;
  ou::tf::TSSWStatsPrice m_statsReturns_slope;

  //double m_dblPrvAdvDec;
  //ou::ChartEntryIndicator m_ceRtnAdvDec;

  ou::ChartEntryIndicator m_ceProfitLoss;

  ou::tf::BarFactory m_bfQuotes01Sec;

  // use a sequence of ECross to maintain history and imply direction
  // use matrix with probabilities to suggest next state?
  // how much history is required? start with prv & cur
  enum class ECross { lowerlo, lowermk, lowerhi, zero, upperlo, uppermk, upperhi }; // fuzzy touch? (how to measure fuzzy?)
  enum EIndicator { // used as index into rCross_t[n][ix]
    rtn_mean
  , rtn_slope
  , rtn_sd
  , prc_norm // normalized price
  , _count
  };

  struct CrossState {
    ECross cross;
    double value;
    CrossState(): cross( ECross::zero ), value {} {}
  };

  using rCross_t = std::array<CrossState, (size_t)EIndicator::_count>;
  rCross_t m_crossing[ 2 ]; // current switches  0 -> 1 -> 0 via mod ( % 1 )
  rCross_t::size_type m_ixprvCrossing;
  rCross_t::size_type m_ixcurCrossing;

  ECross m_ECross_imbalance;

  double m_dblPrice_hi;
  double m_dblPrice_start;
  double m_dblPrice_lo;

  double m_dblPrice_sum_max_profit; // max possible profit during trade
  double m_dblPrice_sum_win; // actual profit, if any
  double m_dblPrice_sum_loss; // actual loss, if any
  double m_dblPrice_sum_max_loss; // max possible loss during trade

  unsigned int m_nMaxProfit;  // based on max, then do histogram of maxes found
  unsigned int m_nWin;
  unsigned int m_nLoss; // based upon stop, not min/max
  unsigned int m_nMaxLoss;

  enum class EZigZag { init, tracklower, trackupper } m_eZigZag;
  boost::posix_time::ptime m_dtZigZag;
  double m_dblZigHi;
  double m_dblZigLo;
  unsigned int m_nZigZags;
  double m_dblSumZigZags;

  void UpdateECross( ECross&, const double mark, const double value ) const;

  template<unsigned int n>
  void UpdateEma( const ou::tf::Price& price_, double& ema, ou::ChartEntryIndicator& cei ) const {
    constexpr double seconds( n );
    constexpr double cur( 1.0 / seconds );
    constexpr double prv( 1.0 - cur );
    const double price( price_.Value() );
    const double ema_ = prv * ema + cur * price; // pre-compute
    ema = ( 0.0 == ema ) ? price : ema_; // uses conditional move instruction?
    cei.Append( price_.DateTime(), ema );
    };

  void UpdatePriceReturn( ou::tf::Price::dt_t, ou::tf::Price::price_t );

  void UpdatePositionProgressUp( const ou::tf::Trade& );
  void UpdatePositionProgressUp_order( ou::tf::OrderArgs&, bool reenter );
  void UpdatePositionProgressDn( const ou::tf::Trade& );
  void UpdatePositionProgressDn_order( ou::tf::OrderArgs&, bool reenter );

  void UpdatePositionProgressUp( const ou::tf::Quote& );
  void UpdatePositionProgressDn( const ou::tf::Quote& );

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

  void HandleTickJ( const ou::tf::Trade& );
  void HandleTickL( const ou::tf::Trade& );
  void HandleTrin( const ou::tf::Trade& );

  void HandleAdv( const ou::tf::Trade& );
  void HandleDec( const ou::tf::Trade& );

  void CalcAdvDec( boost::posix_time::ptime );

  void HandleBarQuotes01Sec( const ou::tf::Bar& );
  void Calc01SecIndicators( const ou::tf::Bar& );

  void HandleRHTrading( const ou::tf::Quote& );
  void HandleRHTrading( const ou::tf::Trade& );

  void HandleRHTrading( const ou::tf::Bar& );
  void HandleCancel( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration );

  void HandleAtRHClose( boost::gregorian::date, boost::posix_time::time_duration );

  enum class ESearchResult { none, sell, buy };
  ESearchResult Search( const ou::tf::Trade& ) const;
  ESearchResult Search( const ou::tf::Quote& ) const;

  void EnterLong(  const ou::tf::Trade& );
  void EnterShort( const ou::tf::Trade& );

  void SetupChart();
  void ValidateAndStart();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(Strategy, 1)

