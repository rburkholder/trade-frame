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
 * File:    ActivityChart.h
 * Author:  raymond@burkholder.net
 * Project: AutoTrade
 * Created: February 14, 2022 11:01
 */

#pragma once

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>


namespace ou {
  class ChartDataView;
namespace tf {
  class TSSWStatsMidQuote;
}
}

class ActivityChart:
  public ou::tf::WinChartView
{
public:

  ActivityChart();
  ActivityChart(
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

  virtual ~ActivityChart();

protected:
private:

  //ou::ChartDataView m_dvChart; // the data, use shared_ptr

  void Init();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(ActivityChart, 1)