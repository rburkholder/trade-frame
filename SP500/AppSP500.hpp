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
 * File:    AppSP500.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: March 30, 2025 13:49:15
 */

#pragma once

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>

#include <TFBitsNPieces/FrameWork02.hpp>

#include <OUCharting/ChartDataView.h>

#include "Config.hpp"

namespace ou { // namespace oneunified
namespace tf { // namespace tradeframe
  class PanelFinancialChart;
}
}
class FrameMain;

class AppSP500:
  public wxApp
, public ou::tf::FrameWork02<AppSP500>
{
  friend class boost::serialization::access;
  friend ou::tf::FrameWork02<AppSP500>;
public:
protected:
private:

  config::Choices m_choices;

  FrameMain* m_pFrameMain;
  ou::tf::PanelFinancialChart* m_pPanelFinancialChart;

  void OnFrameMainAutoMove( wxMoveEvent& );

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    ar & *m_pPanelFinancialChart;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    if ( 2 <= version ) {
      ar & *m_pPanelFinancialChart;
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppSP500, 2)

DECLARE_APP(AppSP500)
