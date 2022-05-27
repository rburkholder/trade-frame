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
 * File:    200DayChart.hpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: May 26, 2022 21:55
 */

#pragma once

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <OUCharting/ChartDataView.h>

#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryVolume.h>

#include <TFVuTrading/WinChartView.h>

#define PANEL_200DAYCHART_STYLE wxTAB_TRAVERSAL
#define PANEL_200DAYCHART_TITLE _("Panel 200 Day Chart")
#define PANEL_200DAYCHART_IDNAME ID_PANEL_200DAYCHART
#define PANEL_200DAYCHART_SIZE wxDefaultSize
#define PANEL_200DAYCHART_POSITION wxDefaultPosition

class Chart200Day:
  public ou::tf::WinChartView
{
  friend class boost::serialization::access;
public:

  Chart200Day();
  Chart200Day(
    wxWindow* parent,
    wxWindowID id = PANEL_200DAYCHART_IDNAME,
    const wxPoint& pos = PANEL_200DAYCHART_POSITION,
    const wxSize& size = PANEL_200DAYCHART_SIZE,
    long style = PANEL_200DAYCHART_STYLE );

  bool Create(
    wxWindow* parent,
    wxWindowID id = PANEL_200DAYCHART_IDNAME,
    const wxPoint& pos = PANEL_200DAYCHART_POSITION,
    const wxSize& size = PANEL_200DAYCHART_SIZE,
    long style = PANEL_200DAYCHART_STYLE );

  virtual ~Chart200Day();

  void Add( const ou::tf::Bars& );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST, ID_PANEL_200DAYCHART
  };

  enum EChartSlot { Price, Volume };

  ou::ChartDataView m_dvChart; // the data

  ou::ChartEntryBars m_cePriceBars;
  ou::ChartEntryVolume m_ceVolume;

  void Init();

  void BindEvents();
  void UnBindEvents();

  void HandleOnResize( wxSizeEvent& );

  void OnDestroy( wxWindowDestroyEvent& );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(Chart200Day, 1)