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

// Started 2013/09/26

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>
#include <OUCharting/ChartEntryShape.h>

#include <TFIndicators/TSEMA.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/ProviderManager.h>
#include <TFTrading/Watch.h>

class ChartTest {
public:
  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;
  ChartTest( pProvider_t );
  ~ChartTest(void);
  ou::ChartDataView& GetChartDataView( void ) {return m_dvChart; };
protected:
private:
  ou::tf::Watch* m_pWatch;

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;

  ou::tf::BarFactory m_bfTrades;

  ou::ChartDataView m_dvChart;
  ou::ChartEntryBars m_ceBars;

  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTrade( const ou::tf::Trade& trade );

  void HandleBarCompletionTrades( const ou::tf::Bar& bar );
};

