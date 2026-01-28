/************************************************************************
 * Copyright(c) 2017, One Unified. All rights reserved.                 *
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
 * File:   WinChartView.h
 * Author: raymond@burkholder.net
 *
 * Created on October 16, 2016, 5:53 PM
 */

// Self contained Chart Viewer
// Handles viewing the user sourced data supplied in ou::ChartDataView

// includes own gui refresh function

#pragma once

#include <thread>

#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>

#include <wx/timer.h>
#include <wx/window.h>

#include <OUCharting/ChartMaster.h>
#include <OUCharting/ChartDataView.h>

#define SYMBOL_WIN_CHARTINTERACTIVE_STYLE wxTAB_TRAVERSAL
#define SYMBOL_WIN_CHARTINTERACTIVE_TITLE _("Window Interactive Chart")
#define SYMBOL_WIN_CHARTINTERACTIVE_IDNAME ID_WINDOW_CHARTINTERACTIVE
#define SYMBOL_WIN_CHARTINTERACTIVE_SIZE wxSize(400, 300)
#define SYMBOL_WIN_CHARTINTERACTIVE_POSITION wxDefaultPosition

namespace ou { // One Unified
namespace tf { // TradeFrame

class WinChartView: public wxWindow {
public:

  WinChartView();
  WinChartView( wxWindow* parent, wxWindowID id = SYMBOL_WIN_CHARTINTERACTIVE_IDNAME,
    const wxPoint& pos = SYMBOL_WIN_CHARTINTERACTIVE_POSITION,
    const wxSize& size = SYMBOL_WIN_CHARTINTERACTIVE_SIZE,
    long style = SYMBOL_WIN_CHARTINTERACTIVE_STYLE );

  bool Create( wxWindow* parent,
    wxWindowID id = SYMBOL_WIN_CHARTINTERACTIVE_IDNAME,
    const wxPoint& pos = SYMBOL_WIN_CHARTINTERACTIVE_POSITION,
    const wxSize& size = SYMBOL_WIN_CHARTINTERACTIVE_SIZE,
    long style = SYMBOL_WIN_CHARTINTERACTIVE_STYLE );
  virtual ~WinChartView();

  void SetChartDataView( ou::ChartDataView* pChartDataView );
  ou::ChartDataView* GetChartDataView() const { return m_pChartDataView; }

  enum class EView {
    live_trail // original 'follow along' (live mode)
  , live_review
  , sim_trail
  , sim_review
  };

  void Set( EView );
  void SetLive_trail();
  void SetLive_review();
  void SetSim_review();
  void SetSim_trail();

  using fDebug_t = std::function<void(const std::string&,const std::string&)>;
  void SetDebug( fDebug_t&& f ) { m_fDebug = std::move( f ); }

protected:

  enum {
    ID_Null=wxID_HIGHEST, ID_WINDOW_CHARTINTERACTIVE
  };

  void DrawChart();

  virtual void LeftClick( int nChart, double value ) {}
  virtual void RightClick( int nChart, double value ) {}
  //virtual void LiveY( int nChart, double value ) {}

private:

  enum class EMouse {
    Drag
  , Down
  , NothingSpecial
  } m_stateMouse;

  using pwxBitmap_t = boost::shared_ptr<wxBitmap>;
  using ViewPort_t = ChartEntryTime::range_t;

  fDebug_t m_fDebug;

  bool m_bSim; // default to false
  EView m_stateView; // default to live_trail

  wxCoord m_coordXStart;

  boost::posix_time::time_duration m_tdViewPortWidth; // width of currently displayed window

  ou::ChartMaster m_chartMaster;
  ou::ChartDataView* m_pChartDataView;

  ViewPort_t m_vpDataViewExtents;
  ViewPort_t m_vpDataViewVisual;

  wxTimer m_timerGuiRefresh;
  bool m_bInDrawChart;

  pwxBitmap_t m_pChartBitmap;

  std::mutex m_mutexChartDataView; // when updating m_pChartDataView
  std::thread m_threadDrawChart;
  boost::asio::io_context m_context;
  std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type> > m_pWork;

  void ThreadDrawChart();  // thread starts here

  void UpdateChartMaster();

  void HandlePaint( wxPaintEvent& );
  void HandleSize( wxSizeEvent& );

  void HandleMouseMotion( wxMouseEvent& );
  void HandleMouseWheel( wxMouseEvent& );

  void HandleMouseEnter( wxMouseEvent& );
  void HandleMouseLeave( wxMouseEvent& );

  void HandleMouseLeftDown( wxMouseEvent& );
  void HandleMouseLeftUp( wxMouseEvent& );

  void HandleMouseRightDown( wxMouseEvent& );
  void HandleMouseRightUp( wxMouseEvent& );

  void HandleMouseLeftClick(); // secondary event
  void HandleMouseRightClick(); // secondary event

  void OnDestroy( wxWindowDestroyEvent& );

  void HandleGuiRefresh( wxTimerEvent& );

  void Init();
  void CreateControls();

  void BindEvents();
  void UnbindEvents();

};

} // namespace tf
} // namespace ou
