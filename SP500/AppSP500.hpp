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

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTrading/DBWrapper.h>

#include <TFVuTrading/TreeItem.hpp>
#include <TFVuTrading/WinChartView.h>

#include <TFBitsNPieces/FrameWork02.hpp>

#include "Config.hpp"
#include "StrategyManager.hpp"

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
  //ou::tf::PanelFinancialChart* m_pPanelFinancialChart; // refactor/inherit once functionality is known

  //std::unique_ptr<ou::tf::db> m_pdb;

  ou::tf::TreeItem* m_ptiRoot;

  ou::tf::WinChartView* m_pwcv; // handles drawing the chart

  using pStrategyManager_t = std::unique_ptr<StrategyManager>;
  pStrategyManager_t m_pStrategyManager;

  void OnFrameMainAutoMove( wxMoveEvent& );

  void HandleMenuActionSimStart();
  void HandleMenuActionSimStop();
  void HandleMenuActionSimEmitStats();

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    //ar & *m_pPanelFinancialChart;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    if ( 2 <= version ) {
      //ar & *m_pPanelFinancialChart;
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppSP500, 3)

DECLARE_APP(AppSP500)
