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
 * File:    Chart.h
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: February 8, 2022 13:38
 */

#pragma once

#include "TFTimeSeries/DatedDatum.h"
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <OUCharting/ChartDataView.h>

#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryMark.h>
#include <OUCharting/ChartEntryShape.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTimeSeries/TimeSeries.h>
#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/Position.h>

#include <TFVuTrading/WinChartView.h>

class InteractiveChart:
  public ou::tf::WinChartView
{
public:

  using pPosition_t = ou::tf::Position::pPosition_t;

  InteractiveChart();
  InteractiveChart(
    wxWindow* parent,
    wxWindowID id = SYMBOL_WIN_CHARTINTERACTIVE_IDNAME,
    const wxPoint& pos = SYMBOL_WIN_CHARTINTERACTIVE_POSITION,
    const wxSize& size = SYMBOL_WIN_CHARTINTERACTIVE_SIZE,
    long style = SYMBOL_WIN_CHARTINTERACTIVE_STYLE );

  bool Create(
    wxWindow* parent,
    wxWindowID id = SYMBOL_WIN_CHARTINTERACTIVE_IDNAME,
    const wxPoint& pos = SYMBOL_WIN_CHARTINTERACTIVE_POSITION,
    const wxSize& size = SYMBOL_WIN_CHARTINTERACTIVE_SIZE,
    long style = SYMBOL_WIN_CHARTINTERACTIVE_STYLE );

  virtual ~InteractiveChart();

  void SetPosition( pPosition_t );

  void Connect();
  void Disconnect();

protected:
private:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  bool m_bConnected;

  ou::ChartDataView m_dvChart; // the data

  pPosition_t m_pPosition;

  ou::tf::BarFactory m_bfPrice;
  ou::tf::BarFactory m_bfPriceUp;
  ou::tf::BarFactory m_bfPriceDn;

  ou::ChartEntryIndicator m_ceTrade;
  ou::ChartEntryBars m_cePriceBars;

  ou::ChartEntryVolume m_ceVolume;
  ou::ChartEntryVolume m_ceVolumeUp;
  ou::ChartEntryVolume m_ceVolumeDn;

  ou::ChartEntryIndicator m_ceQuoteAsk;
  ou::ChartEntryIndicator m_ceQuoteBid;
  ou::ChartEntryIndicator m_ceQuoteSpread;

  ou::ChartEntryShape m_ceShortEntries;
  ou::ChartEntryShape m_ceLongEntries;
  ou::ChartEntryShape m_ceShortFills;
  ou::ChartEntryShape m_ceLongFills;
  ou::ChartEntryShape m_ceShortExits;
  ou::ChartEntryShape m_ceLongExits;

  ou::tf::Quote m_quote;

  void Init();

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

  void HandleBarCompletionPrice( const ou::tf::Bar& );
  void HandleBarCompletionPriceUp( const ou::tf::Bar& );
  void HandleBarCompletionPriceDn( const ou::tf::Bar& );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(InteractiveChart, 1)