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
 * File:    PivotModel.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 26, 2025 15:44:33
 */

// Review to add: lib/TFBitsNPieces/DailyHistory.cpp

#pragma once

#include <OUCharting/ChartEntryMark.h>

#include <TFTimeSeries/DatedDatum.h>

class PivotModel
{
public:

  PivotModel();
  ~PivotModel();

  void OnHistoryBar( const ou::tf::Bar& );
  void OnHistoryDone();

  ou::ChartEntryMark& Pivots() { return m_cePivot; }

protected:
private:

  bool m_bFirstBarProcessed;

  boost::posix_time::ptime m_dtFirstBarUTC, m_dtLastBarUTC;
  ou::tf::Price::price_t m_open, m_high, m_low, m_close;
  ou::tf::Trade::volume_t m_volume;

  ou::ChartEntryMark m_cePivot;

};
