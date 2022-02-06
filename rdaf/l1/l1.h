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

#pragma once

// Started 2022/02/06

#include <string>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartMaster.h>

#include <TFBitsNPieces/FrameWork01.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/WinChartView.h>

class wxTimer;
class wxTreeCtrl;

class AppRdafL1:
  public wxApp,
  public ou::tf::FrameWork01<AppRdafL1> {
  friend ou::tf::FrameWork01<AppRdafL1>;
  friend class boost::serialization::access;
public:
protected:
private:

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;

  std::string m_sStateFileName;

  std::string m_sSymbol;

  ou::ChartMaster m_chart;
  ou::tf::WinChartView* m_pWinChartView;

  wxTimer m_timerGuiRefresh;

  wxTreeCtrl* m_ptreeChartables;  // http://docs.wxwidgets.org/trunk/classwx_tree_ctrl.html

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void HandleGuiRefresh( wxTimerEvent& event );

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

  void HandleSaveValues( void );

//  void HandleDrawChart( const MemBlock& );
  void HandlePaint( wxPaintEvent& event );
  void HandleSize( wxSizeEvent& event );
  void HandleMouse( wxMouseEvent& event );

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    //ar & m_splitPanels->GetSashPosition();
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    //m_splitPanels->SetSashPosition( x );
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppRdafL1, 1)

DECLARE_APP(AppRdafL1)

