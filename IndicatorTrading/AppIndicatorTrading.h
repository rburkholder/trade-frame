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

#include <TFBitsNPieces/FrameWork01.h>

class wxBoxSizer;
class wxTreeCtrl;

class InteractiveChart;
class FrameMain;

namespace ou {
namespace tf {
  class PanelLogging;
}
}

class AppIndicatorTrading:
  public wxApp,
  public ou::tf::FrameWork01<AppIndicatorTrading> {
  friend ou::tf::FrameWork01<AppIndicatorTrading>;
  friend class boost::serialization::access;
public:
protected:
private:

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;

  std::string m_sSymbol;

  //ChartData* m_pChartData;
  //ou::tf::WinChartView* m_pWinChartView;

  wxTreeCtrl* m_ptreeTradables;  // http://docs.wxwidgets.org/trunk/classwx_tree_ctrl.html

  wxBoxSizer* m_sizerFrame;
  wxSplitterWindow* m_splitterRow;

  InteractiveChart* m_pInteractiveChart;

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
    ar & m_splitterRow->GetSashPosition();
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
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppIndicatorTrading, 2)

DECLARE_APP(AppIndicatorTrading)

