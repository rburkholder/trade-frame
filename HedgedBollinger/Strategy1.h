/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#pragma once

// Started 2013/09/23

// started after MultiExpiryBundle has been populated

#include <TFTimeSeries/TimeSeries.h>
#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/DailyTradeTimeFrames.h>

#include <TFOptions/Bundle.h>

#include <TFIndicators/TSSWStats.h>
#include <TFIndicators/TSEMA.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>
#include <OUCharting/ChartEntryShape.h>

class Strategy: public ou::tf::DailyTradeTimeFrame<Strategy> {
  friend ou::tf::DailyTradeTimeFrame<Strategy>; 
public:
  Strategy( ou::tf::option::MultiExpiryBundle* meb );
  ~Strategy(void);
  ou::ChartDataView& GetChartDataView( void ) {return m_dvChart; };
protected:
private:

  ou::tf::option::MultiExpiryBundle* m_pBundle;  // keep towards top of variable section

  ou::ChartDataView m_dvChart;

  ou::ChartEntryBars m_ceBars;

  ou::tf::BarFactory m_bfTrades;
  ou::tf::BarFactory m_bfBuys;
  ou::tf::BarFactory m_bfSells;

  ou::ChartEntryIndicator m_ceEma1;
  ou::ChartEntryIndicator m_ceEma2;
  ou::ChartEntryIndicator m_ceEma3;

  ou::tf::hf::TSEMA<ou::tf::Quote> m_ema1;
  ou::tf::hf::TSEMA<ou::tf::Quote> m_ema2;
  ou::tf::hf::TSEMA<ou::tf::Quote> m_ema3;

  ou::tf::TSSWStatsMidQuote* m_pIndStats1;
  ou::tf::TSSWStatsMidQuote* m_pIndStats2;
  ou::tf::TSSWStatsMidQuote* m_pIndStats3;

  void HandleQuoteUnderlying( const ou::tf::Quote& quote );
  void HandleTradeUnderlying( const ou::tf::Trade& trade );

  void HandleCommon( const ou::tf::Quote& quote );
  void HandleRHTrading( const ou::tf::Quote& quote );

  void HandleCommon( const ou::tf::Trade& trade );
  void HandleRHTrading( const ou::tf::Trade& trade ) {};

  void HandleBarCompletionTrades( const ou::tf::Bar& );
  void HandleBarCompletionBuys( const ou::tf::Bar& );
  void HandleBarCompletionSells( const ou::tf::Bar& );


};

