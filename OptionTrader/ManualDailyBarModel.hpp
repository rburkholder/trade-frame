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
 * File:    ManualDailyBarModel.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 28, 2025 09:00:23
 */

// Review to add: lib/TFBitsNPieces/DailyHistory.cpp

#pragma once

#include <OUCharting/ChartDataView.h>

#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryMark.h>
#include <OUCharting/ChartEntryVolume.h>

#include <TFTimeSeries//TimeSeries.h>

#include <TFTrading/TradingEnumerations.h>
#include <boost/date_time/posix_time/ptime.hpp>

class ManualDailyBarModel
{
public:

  ManualDailyBarModel();
  ~ManualDailyBarModel();

  void Set( ou::tf::InstrumentType::EInstrumentType );

  void OnHistoryIntraBar( const ou::tf::Bar& );
  void OnHistoryDone();

  ou::ChartDataView* GetChartDataView() { return &m_dvChart; }

  ou::ChartEntryMark& Statistics() { return m_ceStatistics; }

protected:
private:

  // *** testing
  ou::tf::InstrumentType::EInstrumentType m_type;
  enum EState { wait_for_start, in_session, out_of_session } m_state;

  boost::posix_time::ptime m_dtStart;
  boost::posix_time::ptime m_dtEnd;
  boost::posix_time::ptime m_dtDead;

  ou::tf::Bars m_barsIntraDay;
  ou::tf::Bars m_barsInterDay;
  //boost::posix_time::ptime m_dtBar;
  boost::gregorian::date m_dateBarTracking;
  ou::tf::Bar m_barAccumulate;
  // *** testing

  enum EChartSlot { Price, Volume };

  ou::ChartDataView m_dvChart; // the data

  ou::ChartEntryBars m_cePriceBars;
  ou::ChartEntryVolume m_ceVolume;

  ou::ChartEntryMark m_ceStatistics;

  void SetTimeFrame_future( boost::posix_time::ptime );
  void SetTimeFrame_equity( boost::posix_time::ptime );
  void CloseBar();

  void HandleBarCompletionPrice( const ou::tf::Bar& );
  void OnHistoryDailyBar( const ou::tf::Bar& );
  void OnHistoryDoneStatistics();

};
