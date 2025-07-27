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
 * File:    DailyBarModel.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 26, 2025 11:40:04
 */

// Review to add: lib/TFBitsNPieces/DailyHistory.cpp

#pragma once

#include <OUCharting/ChartDataView.h>

#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryMark.h>
#include <OUCharting/ChartEntryVolume.h>

#include <TFTimeSeries//TimeSeries.h>

class DailyBarModel
{
public:

  DailyBarModel();
  ~DailyBarModel();

  //void Set( pWatch_t&, ou::ChartEntryMark& );

  void OnHistoryBar( const ou::tf::Bar& );
  void OnHistoryDone();

  ou::ChartDataView* GetChartDataView() { return &m_dvChart; }

  ou::ChartEntryMark& Statistics() { return m_ceStatistics; }

protected:
private:

  enum EChartSlot { Price, Volume };

  ou::ChartDataView m_dvChart; // the data

  ou::tf::Bars m_barsHistory;

  ou::ChartEntryBars m_cePriceBars;
  ou::ChartEntryVolume m_ceVolume;

  ou::ChartEntryMark m_ceStatistics;

  void HandleBarCompletionPrice( const ou::tf::Bar& );

};
