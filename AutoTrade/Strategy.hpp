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
 * File:    Strategy.h
 * Author:  raymond@burkholder.net
 * Project: AutoTrade
 * Created: February 14, 2022 10:59
 */

#include <vector>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryMark.h>
#include <OUCharting/ChartEntryShape.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <OUCharting/ChartDataView.h>

#include <TFIndicators/TSEMA.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>
#include <TFTrading/DailyTradeTimeFrames.h>

namespace config {
  class Options;
}

namespace ou {
  class ChartDataView;
}

class Stochastic;
class Strategy_impl;

class Strategy:
  public ou::tf::DailyTradeTimeFrame<Strategy>
{
  friend class boost::serialization::access;
  friend ou::tf::DailyTradeTimeFrame<Strategy>;
public:

  using pOrder_t = ou::tf::Order::pOrder_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  Strategy( ou::ChartDataView&, const config::Options& );
  virtual ~Strategy();

  void SetPosition( pPosition_t );
  void SetTick( pWatch_t );
  void SetTrin( pWatch_t );

  void SaveWatch( const std::string& );

  void CloseAndDone();

protected:
private:

  enum EChartSlot { Price, Volume, Tick, Stoch, Trin, PL };

  enum class ETradeState {
    Init,  // initiaize state in current market
    Neutral, // netral state prior to active search
    Search,  // looking for long or short enter
    LongSubmitted, // order has been submitted, waiting for confirmation
    LongExit,  // position exists, looking for exit
    ShortSubmitted,  // order has been submitted, waiting for confirmtaion
    ShortExit,  // position exists, looking for exit
    LongExitSubmitted, // wait for exit to complete
    ShortExitSubmitted, // wait for exit to complete
    EndOfDayCancel,
    EndOfDayNeutral,
    Done // no more action
    };

  ETradeState m_stateTrade;

  enum class ETick {

  };

  pPosition_t m_pPosition;
  pWatch_t m_pTick;
  pWatch_t m_pTrin;

  int m_nPeriodWidth;

  using vMAPeriods_t = std::vector<int>;
  vMAPeriods_t m_vMAPeriods;

  struct MA {

    ou::tf::hf::TSEMA<ou::tf::Quote> m_ema;
    ou::ChartEntryIndicator m_ceMA;

    MA( ou::tf::Quotes& quotes, size_t nPeriods, time_duration tdPeriod, ou::Colour::EColour colour, const std::string& sName )
    : m_ema( quotes, nPeriods, tdPeriod )
    {
      m_ceMA.SetName( sName );
      m_ceMA.SetColour( colour );
    }

    MA( MA&& rhs )
    : m_ema(  std::move( rhs.m_ema ) )
    , m_ceMA( std::move( rhs.m_ceMA ) )
    {}

    void AddToView( ou::ChartDataView& cdv ) {
      cdv.Add( EChartSlot::Price, &m_ceMA );
    }

    void Update( ptime dt ) {
      m_ceMA.Append( dt, m_ema.GetEMA() );
    }

    double Latest() const { return m_ema.GetEMA(); }
  };

  using vMA_t = std::vector<MA>;
  vMA_t m_vMA;

  int m_nStochasticPeriods;
  using pStochastic_t = std::unique_ptr<Stochastic>;
  pStochastic_t m_pStochastic;

  ou::ChartEntryMark m_cemStochastic;

  double m_dblStochastic;
  enum class EZigZag { EndPoint1, EndPoint2, LowFound, HighFound } m_eZigZag;
  double m_dblEndPoint1;
  double m_dblEndPoint2;
  double m_dblZigZagDistance;
  size_t m_nZigZagLegs;

  using pStrategy_impl_t = std::unique_ptr<Strategy_impl>;
  pStrategy_impl_t m_pStrategy_impl;

  double m_dblMid;
  double m_dblLastTick;
  double m_dblLastTrin;

  ou::tf::Quote m_quote;

  pOrder_t m_pOrder;

  ou::ChartDataView& m_cdv;

  ou::ChartEntryMark m_cemZero;
  ou::ChartEntryMark m_cemOne;

  ou::ChartEntryIndicator m_ceQuoteAsk;
  ou::ChartEntryIndicator m_ceQuoteBid;

  ou::ChartEntryIndicator m_ceTrade;
  ou::ChartEntryVolume m_ceVolume;

  ou::ChartEntryShape m_ceLongEntry;
  ou::ChartEntryShape m_ceLongFill;
  ou::ChartEntryShape m_ceLongExit;
  ou::ChartEntryShape m_ceShortEntry;
  ou::ChartEntryShape m_ceShortFill;
  ou::ChartEntryShape m_ceShortExit;

  ou::ChartEntryIndicator m_ceTick;
  ou::ChartEntryIndicator m_ceTrin;

  boost::posix_time::ptime m_dtZigZag;
  ou::ChartEntryIndicator m_ceZigZag;

  ou::ChartEntryIndicator m_ceProfitLoss;

  ou::tf::BarFactory m_bfQuotes01Sec;

  double m_dblStopDelta;
  double m_dblStopAbsolute;

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

  void HandleTick( const ou::tf::Trade& );
  void HandleTrin( const ou::tf::Trade& );

  void HandleBarQuotes01Sec( const ou::tf::Bar& bar );

  void HandleRHTrading( const ou::tf::Bar& bar );
  void HandleRHTrading_01Sec( const ou::tf::Bar& bar );
  void HandleCancel( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration );

  void EnterLong( const ptime dt, const double tag );
  void EnterShort( const ptime dt, const double tag );

  void ExitLong( const ptime dt, const double tag );
  void ExitShort( const ptime dt, const double tag );

  void HandleOrderCancelled( const ou::tf::Order& );
  void HandleOrderFilled( const ou::tf::Order& );

  void Clear();
  void SetupChart();

};