/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * Project: CurrencyTrader
 * Created: March 14, 2024 21:32:07
 */

#pragma once

#include <array>
#include <cmath>
#include <cstdlib>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryMark.h>
#include <OUCharting/ChartEntryShape.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>
#include <TFTrading/Portfolio.h>
#include <TFTrading/DailyTradeTimeFrames.h>

#include <TFBitsNPieces/TrackOrder.hpp>

#include "Ind_Ema.hpp"
//#include "Ind_UltSmth.hpp"

#include "CubicRegression.hpp"

#include "Common.hpp"

namespace ou {
namespace tf {
  class TreeItem;
}
}

class Strategy:
  public ou::tf::DailyTradeTimeFrame<Strategy>
{
  friend class boost::serialization::access;
  friend ou::tf::DailyTradeTimeFrame<Strategy>;
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;

  using fResetSoftware_t = std::function<bool()>;
  using fConstructPosition_t = std::function<pPosition_t(pWatch_t,const std::string&)>;

  using fTransferFunds_t = TrackOrder::fTransferFunds_t;

  Strategy( const config::Strategy& );
  ~Strategy();

  void SetTransaction( ou::tf::Order::quantity_t, fTransferFunds_t&& );
  void SetWatch( EBase, pWatch_t, pPortfolio_t, fConstructPosition_t&& );

  void SaveWatch( const std::string& sPrefix );
  void CloseAndDone();
  void SetResetSoftware( fResetSoftware_t&& f ) { m_fResetSoftware = std::move( f ); }

  void EmitSwingTrack();
  void EmitCubicCoef();

  struct latest_t {
    const double bid;
    const double ask;
    const size_t count;
    const double commission;
    latest_t( double bid_, double ask_, size_t count_, double commission_ )
    : bid( bid_ ), ask( ask_ ), count( count_ ), commission( commission_ ) {}
  };

  latest_t Latest() const;

  ou::ChartDataView& GetChartDataView() { return m_cdv; }

protected:
private:

  enum EChartSlot { Price, Volume, ATR, TR_EMA, SD, MASlope, MA, Stoch, PL_To, PL_Ttl, Commission };

  using pOrder_t = ou::tf::Order::pOrder_t;
  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  ou::tf::TreeItem* m_pTreeItemSymbol;

  ou::tf::BarFactory m_bfQuotes01Sec;
  ou::tf::BarFactory m_bfTrading;  // default to 15 minutes

  ou::tf::Bars m_barsTrading;

  ou::ChartDataView m_cdv;

  ou::tf::Quote m_quote;

  ou::ChartEntryIndicator m_ceQuoteAsk;
  ou::ChartEntryIndicator m_ceQuoteBid;

  ou::ChartEntryBars m_ceBarsTrade;

  ou::ChartEntryIndicator m_ceTrade;
  ou::ChartEntryVolume m_ceVolume;

  ou::ChartEntryIndicator m_ceCubicSwingDn;
  ou::ChartEntryIndicator m_ceCubicSwingUp;

  ou::ChartEntryShape m_ceSwingHi;
  ou::ChartEntryShape m_ceSwingLo;

  // Should be able to detect a generally rising or falling market?
  ou::ChartEntryShape m_ceTradingRangeRising;
  ou::ChartEntryShape m_ceTradingRangeFalling;

  struct PL {

    ou::ChartEntryIndicator m_ceUnRealized;
    ou::ChartEntryIndicator m_ceRealized;
    ou::ChartEntryIndicator m_ceProfitLoss;
    ou::ChartEntryIndicator m_ceCommission;

    void Init( ou::ChartDataView& cdv, int slot ) {
      m_ceRealized.SetName( "Realized" );
      m_ceUnRealized.SetName( "Unrealized" );

      m_ceRealized.SetColour( ou::Colour::DarkCyan );
      m_ceUnRealized.SetColour( ou::Colour::Purple );

      cdv.Add( slot, &m_ceRealized );
      cdv.Add( slot, &m_ceUnRealized );
    }

    void Clear() {
      m_ceUnRealized.Clear();
      m_ceRealized.Clear();
      m_ceProfitLoss.Clear();
      m_ceCommission.Clear();
    }
  };

  PL m_plTo;
  PL m_plTtl;

  EBase m_eBaseCurrency;
  ou::tf::Order::quantity_t m_quantityToOrder;
  double m_tick;
  double m_base_currency_pip;

  pWatch_t m_pWatch;
  pPortfolio_t m_pPortfolio;

  TrackOrder m_to;

  using pSmoother_t = std::unique_ptr<ou::tf::indicator::Ema>;
  using vSmoother_t = std::vector<pSmoother_t>;
  vSmoother_t m_vSmootherCurrency;

  struct TR { // True Range

    double previous_close;
    double true_range;

    TR(): previous_close {} {}

    double Update( const ou::tf::Bar& bar ) { // calculate true range

      if ( 0.0 == previous_close ) {
        previous_close = bar.Open(); // prime the value one time
      }

      const double bar_hi( bar.High() );
      const double bar_lo( bar.Low() );

      const double upper_delta( bar_hi >= previous_close ? bar_hi - previous_close : previous_close - bar_hi );
      const double lower_delta( bar_lo >= previous_close ? bar_lo - previous_close : previous_close - bar_lo );

      true_range = bar_hi - bar_lo; // prime with bar height
      if ( upper_delta > true_range ) true_range = upper_delta;
      if ( lower_delta > true_range ) true_range = lower_delta;

      previous_close = bar.Close();

      return true_range;
    }
  };

  using pEma_t = std::unique_ptr<ou::tf::indicator::Ema>;

  TR m_TRFast;
  pEma_t m_pATRFast;

  TR m_TRSlow;
  pEma_t m_pATRSlow;

  struct Swing {

    double hi;
    double lo;
    boost::posix_time::ptime dt;

    enum class EBarState { Unknown, Above, Below, Straddle } eBarState;

    Swing(): hi {}, lo {}, eBarState( EBarState::Unknown ) {}

    void Update( const ou::tf::Bar& bar, double barrier ) {

      hi = bar.High();
      lo = bar.Low();
      assert( hi >= lo );

      dt = bar.DateTime(); // TODO: add bar width to set properly
      if ( hi < barrier ) {
        eBarState = EBarState::Below;
      }
      else {
        if ( lo > barrier ) {
          eBarState = EBarState::Above;
        }
        else {
          eBarState = EBarState::Straddle;
        }
      }
    }

  };

  using rSwing_t = std::array<Swing, 5>;
  rSwing_t m_rSwing;

  // TODO: need to track volume to enhance the signal change

  struct SwingTrack {

    boost::posix_time::ptime dtComputed;
    boost::posix_time::ptime dtPeak;
    double dblPeak;
    boost::posix_time::ptime dtSignal;
    double dblSignal;

    SwingTrack(
      boost::posix_time::ptime dtComputed_
    , boost::posix_time::ptime dtPeak_
    , double dblPeak_
    , boost::posix_time::ptime dtSignal_
    , double dblSignal_
    )
    : dtComputed( dtComputed_ )
    , dtPeak( dtPeak_ ), dblPeak( dblPeak_ )
    , dtSignal( dtSignal_ ), dblSignal( dblSignal_ )
    {}
  };

  using vSwingTrack_t = std::vector<SwingTrack>;
  vSwingTrack_t m_vSwingTrack;

  unsigned int m_nHi;
           int m_nNet;
  unsigned int m_nLo;

  struct State {
    enum class Swing { none, up, down } swing;
    enum class Cross { none, above, below } cross; // ema
    double last;
    double sum;
    State()
    : swing( Swing::none )
    , cross( Cross::none )
    , last {}
    , sum {}
    {}
  };
  State m_state;

  struct TradingRange {
    double dblStart; // starting point on Swing change
    double dblExtension; // current new limit in range
    double dblEma; // running ema of range
    TradingRange(): dblStart {}, dblExtension {}, dblEma {} {}
  };
  TradingRange m_trRising, m_trFalling;

  struct Stop {
    double start;
    double diff;
    double trail;
    Stop(): start {}, diff {}, trail {} {}
  };
  Stop m_stop;

  size_t m_nCount;
  double m_dblCommission;

  ou::tf::Regression::Cubic m_cubicSwingDn;
  ou::tf::Regression::Cubic m_cubicSwingUp;

  fResetSoftware_t m_fResetSoftware;

  void Init( const config::Strategy& );

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

  void HandleBellHeard( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleRHTrading( const ou::tf::Bar& );
  void HandleCancel( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleAtRHClose( boost::gregorian::date, boost::posix_time::time_duration );

  void HandleRHTrading( const ou::tf::DatedDatum& ) {};
  bool HandleSoftwareReset( boost::gregorian::date, boost::posix_time::time_duration );

  void HandleBarQuotes01Sec( const ou::tf::Bar& );
  void HandleMinuteBar( const ou::tf::Bar& );

  void RunState( TrackOrder& );

};