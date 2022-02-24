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
 * File:    AppIndicatorTrading.h
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: February 8, 2022 00:12
 */

#pragma once

#include <string>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>
#include <wx/splitter.h>

#include <TFTrading/DBWrapper.h>

#include <TFBitsNPieces/FrameWork01.h>

#include "Config.h"

class wxBoxSizer;
class wxTreeCtrl;

class FrameMain;
class InteractiveChart;

namespace ou {
namespace tf {
  class PanelLogging;
  class FrameControls;
  class BuildInstrument;
  class PanelOrderButtons;
}
}

class AppIndicatorTrading:
  public wxApp,
  public ou::tf::FrameWork01<AppIndicatorTrading>
{
  friend ou::tf::FrameWork01<AppIndicatorTrading>;
  friend class boost::serialization::access;
public:
protected:
private:

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;
  ou::tf::FrameControls* m_pFrameControls;
  ou::tf::PanelOrderButtons* m_pPanelOrderButtons;

  std::string m_sTSDataStreamStarted;

  wxTreeCtrl* m_ptreeTradables;  // http://docs.wxwidgets.org/trunk/classwx_tree_ctrl.html

  wxBoxSizer* m_sizerFrame;
  wxSplitterWindow* m_splitterRow;

  InteractiveChart* m_pInteractiveChart;

  std::unique_ptr<ou::tf::BuildInstrument> m_pBuildInstrument;

  std::unique_ptr<ou::tf::db> m_pdb;

  config::Options m_config;

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void OnData1Connected( int );
  void OnData2Connected( int );
  void OnExecConnected( int );
  void OnData1Disconnected( int );
  void OnData2Disconnected( int );
  void OnExecDisconnected( int );

  void HandleMenuActionStartWatch();
  void HandleMenuActionStopWatch();
  void HandleMenuActionSaveValues();
  void HandleMenuActionStartChart();
  void HandleMenuActionStopChart();

  void ConstructInstrument();

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    ar & m_splitterRow->GetSashPosition();
    ar & *m_pFrameControls;
    //ar & *m_pWinChartView;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    int x;
    ar & x;
    m_splitterRow->SetSashPosition( x );
    if ( 2 <= version ) {
      //ar & *m_pWinChartView;
    }
    if ( 3 <= version ) {
      ar & *m_pFrameControls;
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppIndicatorTrading, 3)

DECLARE_APP(AppIndicatorTrading)

