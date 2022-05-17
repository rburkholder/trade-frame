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
 * File:    SessionChart.hpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: May 16, 2022 20:03
 */

#pragma once

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <OUCharting/ChartDataView.h>

#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryVolume.h>
//#include <OUCharting/ChartEntryIndicator.h>

#include <TFTimeSeries/TimeSeries.h>
#include <TFTimeSeries/BarFactory.h>

#include <TFIQFeed/BarHistory.h>

#include <TFTrading/Position.h>

#include <TFVuTrading/WinChartView.h>

class SessionChart:
  public ou::tf::WinChartView
{
  friend class boost::serialization::access;
public:

  SessionChart();
  SessionChart(
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

  virtual ~SessionChart();

  using pPosition_t = ou::tf::Position::pPosition_t;

  void SetPosition( pPosition_t );

protected:
private:

  enum EChartSlot { Price, Volume };

  bool m_bWatchStarted;

  pPosition_t m_pPosition;

  ou::ChartDataView m_dvChart; // the data

  ou::tf::BarFactory m_bfPrice1Minute;

  using pBarHistory_t = std::unique_ptr<ou::tf::iqfeed::BarHistory>;
  pBarHistory_t m_pBarHistory;
  
  //ou::tf::Bars m_barsSessionHistory;

  ou::ChartEntryBars m_cePriceBars;
  ou::ChartEntryVolume m_ceVolume;

  void Init();

  void BindEvents();
  void UnBindEvents();

  void OnDestroy( wxWindowDestroyEvent& );

  void HandleTrade( const ou::tf::Trade& );

  void HandleBarCompletionPrice( const ou::tf::Bar& );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(SessionChart, 1)