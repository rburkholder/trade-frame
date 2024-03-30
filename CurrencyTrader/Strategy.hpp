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

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryMark.h>
#include <OUCharting/ChartEntryShape.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>
#include <TFTrading/DailyTradeTimeFrames.h>

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

  Strategy();
  ~Strategy();

  void SetPosition( pPosition_t );
  void SaveWatch( const std::string& sPrefix );

  ou::ChartDataView& GetChartDataView() { return m_cdv; }

protected:
private:

  enum EChartSlot { Price, Volume, ATR, SD, MASlope, MA, Stoch };
  enum class ETradeState {
    Init,  // initiaize state in current market
    Search,  // looking for long or short enter
    LongSubmitted, // order has been submitted, waiting for confirmation
    LongExit,  // position exists, looking for exit
    ShortSubmitted,  // order has been submitted, waiting for confirmtaion
    ShortExit,  // position exists, looking for exit
    LongExitSubmitted, // wait for exit to complete
    ShortExitSubmitted, // wait for exit to complete
    NoTrade, // from the config file, no trading, might be a future
    EndOfDayCancel,
    EndOfDayNeutral,
    Done // no more action
    };

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

  ou::ChartEntryBars m_ceBarsTradeRise;
  ou::ChartEntryBars m_ceBarsTradeFall;

  ou::ChartEntryIndicator m_ceTrade;
  ou::ChartEntryVolume m_ceVolume;

  ou::ChartEntryShape m_ceLongEntry;
  ou::ChartEntryShape m_ceLongFill;
  ou::ChartEntryShape m_ceLongExit;
  ou::ChartEntryShape m_ceShortEntry;
  ou::ChartEntryShape m_ceShortFill;
  ou::ChartEntryShape m_ceShortExit;

  ou::ChartEntryIndicator m_ceProfitLoss;

  pOrder_t m_pOrder;
  pWatch_t m_pWatch;
  pPosition_t m_pPosition;

  ETradeState m_stateTrade;

  struct EMA { // Exponential Moving Average

    bool bBootStrapped;

    double dblCoef1; // smaller - used on arriving value
    double dblCoef2; // 1 - dblCoef1 (larger), used on prior ema
    double dblEmaLatest;
    unsigned int ixSlot;

    ou::ChartEntryIndicator m_ceEma;
    ou::ChartDataView& m_cdv;

    EMA( unsigned int nIntervals, ou::ChartDataView& cdv, unsigned int ixSlot_ )
    : bBootStrapped( false ), dblEmaLatest {}, m_cdv( cdv ), ixSlot( ixSlot_ )
    {
      dblCoef1 = 2.0 / ( nIntervals + 1 );
      dblCoef2 = 1.0 - dblCoef1;
      m_cdv.Add( ixSlot, &m_ceEma );
    }

    ~EMA() {
      m_cdv.Remove( ixSlot, &m_ceEma );
    }

    void Set( ou::Colour::EColour colour, const std::string& sName ) {
      m_ceEma.SetName( sName );
      m_ceEma.SetColour( colour );
    }

    double Update( boost::posix_time::ptime dt, double value ) {

      if ( bBootStrapped ) {
        dblEmaLatest = ( dblCoef1 * value ) + ( dblCoef2 * dblEmaLatest );
      }
      else {
        bBootStrapped = true;
        dblEmaLatest = value;
      }

      m_ceEma.Append( dt, dblEmaLatest );
      return dblEmaLatest;
    }
  };

  using pEMA_t = std::unique_ptr<EMA>;
  pEMA_t m_pEmaCurrency;

  struct TR { // True Range

    double close;

    TR(): close {} {}

    double Update( const ou::tf::Bar& bar ) {

      const double hi( bar.High() );
      const double lo( bar.Low() );

      const double upper( hi >= close ? hi - close : close - hi );
      const double lower( lo >= close ? lo - close : close - lo );

      double range( hi - lo );
      if ( upper > range ) range = upper;
      if ( lower > range ) range = lower;

      close = bar.Close();

      return range;
    }
  };

  TR m_TRFast;
  pEMA_t m_pATRFast;

  TR m_TRSlow;
  pEMA_t m_pATRSlow;

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

  void HandleRHTrading( const ou::tf::Bar& bar );
  void HandleCancel( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration );

  void HandleOrderCancelled( const ou::tf::Order& );
  void HandleOrderFilled( const ou::tf::Order& );

  void HandleBarQuotes01Sec( const ou::tf::Bar& );
  void HandleMinuteBar( const ou::tf::Bar& );

};