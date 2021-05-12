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

#include <boost/shared_ptr.hpp>

#include <boost/thread/thread_only.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

#include <wx/window.h>
#include <wx/timer.h>

#include <OUCharting/ChartMaster.h>
#include <OUCharting/ChartDataView.h>

#include <TFVuTrading/InterfaceBoundEvents.h>

#define SYMBOL_WIN_CHARTINTERACTIVE_STYLE wxTAB_TRAVERSAL
#define SYMBOL_WIN_CHARTINTERACTIVE_TITLE _("Window Interactive Chart")
#define SYMBOL_WIN_CHARTINTERACTIVE_IDNAME ID_WINDOW_CHARTINTERACTIVE
#define SYMBOL_WIN_CHARTINTERACTIVE_SIZE wxSize(400, 300)
#define SYMBOL_WIN_CHARTINTERACTIVE_POSITION wxDefaultPosition

namespace ou { // One Unified
namespace tf { // TradeFrame

class WinChartView: public wxWindow, public InterfaceBoundEvents {
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

  void SetChartDataView( ou::ChartDataView* pChartDataView, bool bReCalcViewPort = true );
  ou::ChartDataView* GetChartDataView( void ) const { return m_pChartDataView; }

  // really don't want these here, but necessary to deal with searchdynamiceventtable issues
  virtual void BindEvents();
  virtual void UnbindEvents();

protected:

  void Init();
  void CreateControls();

  void DrawChart( void );

private:

  using pwxBitmap_t = boost::shared_ptr<wxBitmap>;
  using ViewPort_t = ChartEntryTime::range_t;

  enum {
    ID_Null=wxID_HIGHEST, ID_WINDOW_CHARTINTERACTIVE
  };

  boost::posix_time::time_duration m_tdViewPortWidth;

  double m_dblViewPortRatio; // 0.0 ... 1.0 (expands around mouse)
  ViewPort_t m_vpPrior;
  bool m_bBeginExtentFound;

  ou::ChartMaster m_chartMaster;
  ou::ChartDataView* m_pChartDataView;

  wxTimer m_timerGuiRefresh;
  bool m_bInDrawChart;
  bool m_bReCalcViewPort;

  pwxBitmap_t m_pChartBitmap;

  bool m_bThreadDrawChartActive;
  boost::mutex m_mutexThreadDrawChart;
  boost::condition_variable m_cvThreadDrawChart;
  boost::thread* m_pThreadDrawChart;

  bool m_bBound;

  void ThreadDrawChart( void );  // thread starts here
  void CallBackDrawChart( const MemBlock& m );  // a callback here to perform bitmap
  //void HandleGuiDrawChart( EventDrawChart& event );
  void StartThread( void );
  void StopThread( void );

  void RescaleViewPort( void );

  void UpdateChartMaster();

  void HandleDrawChart( const MemBlock& );
  void HandlePaint( wxPaintEvent& event );
  void HandleSize( wxSizeEvent& event );

  void HandleMouse( wxMouseEvent& event );
  void HandleMouseEnter( wxMouseEvent& event );
  void HandleMouseLeave( wxMouseEvent& event );
  void HandleMouseWheel( wxMouseEvent& event );

  void OnDestroy( wxWindowDestroyEvent& event );

  void HandleGuiRefresh( wxTimerEvent& event );

  void ManualDraw( void ); // code placeholder for now

};

} // namespace tf
} // namespace ou
