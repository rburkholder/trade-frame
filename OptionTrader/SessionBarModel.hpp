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
 * File:    SessionBarModel.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 25, 2025 11:17:40
 */

// Inspiration: IndicatorTrading/SessionChart.hpp

#pragma once

#include <OUCharting/ChartDataView.h>

#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryMark.h>
#include <OUCharting/ChartEntryVolume.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/Watch.h>

class SessionBarModel
{
public:

  SessionBarModel();
  ~SessionBarModel();

  using pWatch_t = ou::tf::Watch::pWatch_t;

  bool IsWatching() const { return m_bWatchStarted; };

  //void Set( pWatch_t&, ou::ChartEntryMark& );
  void Set( pWatch_t& );

  void OnHistoryBar( const ou::tf::Bar& );
  void OnHistoryDone();

  ou::ChartDataView* GetChartDataView() { return &m_dvChart; }

protected:
private:

  enum EChartSlot { Price, Volume };

  bool m_bWatchStarted;

  pWatch_t m_pWatch;

  ou::ChartDataView m_dvChart; // the data

  ou::tf::BarFactory m_bfPrice1Minute;

  ou::ChartEntryBars m_cePriceBars;
  ou::ChartEntryVolume m_ceVolume;

  void StartWatch();
  void StopWatch();

  void HandleTrade( const ou::tf::Trade& );

  void HandleBarCompletionPrice( const ou::tf::Bar& );

};
