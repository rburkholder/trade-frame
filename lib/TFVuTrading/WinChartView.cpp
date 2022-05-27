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
 * File:   WinChartView.cpp
 * Author: raymond@burkholder.net
 *
 * Created on October 16, 2016, 5:53 PM
 */

#include <wx/bitmap.h>
#include <wx/cursor.h>
#include <wx/mstream.h>
#include <wx/dcclient.h>

#include <OUCommon/TimeSource.h>

#include "WinChartView.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

WinChartView::WinChartView(): wxWindow() {
  Init();
}

WinChartView::WinChartView(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos,
  const wxSize& size,
  long style )
{

  Init(); // binds and variables needed before
  Create(parent, id, pos, size, style);

}

WinChartView::~WinChartView() {
}

void WinChartView::Init() {

  m_bBound = false;
  m_bInDrawChart = false;
  m_bThreadDrawChartActive = false;
  m_pThreadDrawChart = nullptr;
  m_pChartDataView = nullptr;

  m_dblViewPortRatio = 1.0;
  m_bBeginExtentFound = false;

  m_tdViewPortWidth = boost::posix_time::time_duration( 0, 10, 0 );  // default viewport width to 10 minutes

}

bool WinChartView::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

  wxWindow::Create( parent, id, pos, size, style );

  CreateControls();
//  if (GetSizer())     {
//      GetSizer()->SetSizeHints(this);
//  }
  return true;
}

void WinChartView::BindEvents() {

  if ( !m_bBound ) {

    m_bBound = true;

    Bind( wxEVT_PAINT, &WinChartView::HandlePaint, this );
    Bind( wxEVT_SIZE, &WinChartView::HandleSize, this );

    Bind( wxEVT_MOTION, &WinChartView::HandleMouse, this );
    Bind( wxEVT_MOUSEWHEEL, &WinChartView::HandleMouseWheel, this );
    Bind( wxEVT_ENTER_WINDOW, &WinChartView::HandleMouseEnter, this );
    Bind( wxEVT_LEAVE_WINDOW, &WinChartView::HandleMouseLeave, this );

    // this GuiRefresh initialization should come after all else
    m_timerGuiRefresh.SetOwner( this );
    Bind( wxEVT_TIMER, &WinChartView::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
    m_timerGuiRefresh.Start( 200 );

  }
}

void WinChartView::CreateControls() {

  //Bind( wxEVT_CLOSE_WINDOW, &WinChartView::OnClose, this );  // not called for child windows
  Bind( wxEVT_DESTROY, &WinChartView::OnDestroy, this );

  BindEvents();

}

void WinChartView::StartThread() {
  m_pThreadDrawChart = new boost::thread( &WinChartView::ThreadDrawChart, this );
}

void WinChartView::StopThread() {
  m_bThreadDrawChartActive = false;
  m_cvThreadDrawChart.notify_one();
  m_pThreadDrawChart->join();
  delete m_pThreadDrawChart;
  m_pThreadDrawChart = nullptr;
}

// called from PanelChartHdf5::LoadDataAndGenerateChart
// called from PanelCharts::HandleInstrumentLiveChart
// called from PanelFinancialChart::HandleTreeEventitemActivated
void WinChartView::SetChartDataView( ou::ChartDataView* pChartDataView, bool bReCalcViewPort ) {
  // TODO: need to sync with the gui refresh thread
  m_bReCalcViewPort = bReCalcViewPort;
  if ( m_bThreadDrawChartActive ) {
    StopThread();
  }
  m_pChartDataView = pChartDataView;
  if ( nullptr != m_pChartDataView ) {
    StartThread();
  }
}

void WinChartView::HandleMouse( wxMouseEvent& event ) {
  //if ( event.LeftIsDown() ) std::cout << "Left is down" << std::endl;
  //if ( event.MiddleIsDown() ) std::cout << "Middle is down" << std::endl;
  //if ( event.RightIsDown() ) std::cout << "Right is down" << std::endl;
  wxCoord x, y;
  event.GetPosition( &x, &y );
  //std::cout << x << "," << y << std::endl;
//  std::cout << event.AltDown() << "," << event.ControlDown() << "," << event.ShiftDown() << std::endl;
//  std::cout << event.GetWheelAxis() << "," << event.GetWheelDelta() << "," << event.GetWheelRotation() << std::endl;
  // 0,120,-120
  m_chartMaster.CrossHairPosition( x, y );
  // TODO: translate into a price
  //event.Skip();
}

void WinChartView::HandleMouseWheel( wxMouseEvent& event ) {

  //int delta = event.GetWheelDelta();
  int rotation = event.GetWheelRotation(); // has positive, negative, use delta to normalize
  //bool bShift = event.ShiftDown();
  //bool bControl = event.ControlDown();
  //bool bAlt = event.AltDown();

  wxPoint pos = event.GetPosition();
  wxSize size = GetClientSize();

  if ( 0 == size.GetWidth() ) m_dblViewPortRatio = 1.0;
  else {
    m_dblViewPortRatio = (double) pos.x / (double) size.GetWidth();
  }

  //std::cout
  //      << "Wheel: " << delta << "," << rotation << ",sca:"
  //    << bShift << bControl << bAlt
  //    << std::endl;

  static const boost::posix_time::time_duration tdTenSeconds( 0, 0, 10 );
  boost::posix_time::time_duration tdCurrent( m_tdViewPortWidth );

  if ( 0 > rotation ) {
    m_tdViewPortWidth *= 12;
    m_tdViewPortWidth /= 10;
  }
  else {
    m_tdViewPortWidth *= 10;
    m_tdViewPortWidth /= 12;
  }

  if ( m_tdViewPortWidth < tdTenSeconds ) m_tdViewPortWidth = tdCurrent;

  //std::cout << "m_tdViewPortWidth=" << m_tdViewPortWidth << std::endl;

  m_bReCalcViewPort = true;

  DrawChart();
  //event.Skip();
}

void WinChartView::RescaleViewPort() {
  ViewPort_t vp;
  if ( !m_bBeginExtentFound ) {
    vp = m_pChartDataView->GetExtents();
    if ( vp.HasBegin() ) m_bBeginExtentFound = true;
  }
  else {
    // assume 'begin' extent never changes - time moves forward
    vp.dtBegin = m_vpPrior.dtBegin; // may or may not need this line
    vp.dtEnd = m_pChartDataView->GetExtentEnd();
  }
}

void WinChartView::HandleMouseEnter( wxMouseEvent& event ) {
  //std::cout << "mouse enter" << std::endl;
  m_chartMaster.CrossHairDraw( true );
  event.Skip();
}

void WinChartView::HandleMouseLeave( wxMouseEvent& event ) {
  //std::cout << "mouse leave" << std::endl;
  m_chartMaster.CrossHairDraw( false );
  event.Skip();
}

void WinChartView::HandlePaint( wxPaintEvent& event ) {
  if ( 0 != m_pChartBitmap.use_count() ) {
    wxPaintDC dc( this );
    dc.DrawBitmap( *m_pChartBitmap.get(), 0, 0);
  }
}

void WinChartView::HandleSize( wxSizeEvent& event ) {
  // let the timer do the refresh instead?
  //this->RefreshRect( this->GetClientRect(), false );
  DrawChart();
}

void WinChartView::HandleGuiRefresh( wxTimerEvent& event ) {
  if ( m_fRefreshData ) m_fRefreshData();
  DrawChart();
}

void WinChartView::DrawChart() {
  if ( m_bThreadDrawChartActive ) {
    m_bInDrawChart = true;
    m_cvThreadDrawChart.notify_one();
  }
}

// could change this into a worker future/promise solution, or use asio to submit jobs or packages
void WinChartView::ThreadDrawChart() {
  m_bThreadDrawChartActive = true; // TODO: examine if this a problem for locking (happened when multiple changes submitted in rapid succession)
  boost::unique_lock<boost::mutex> lock(m_mutexThreadDrawChart);
  while ( m_bThreadDrawChartActive ) {
    m_cvThreadDrawChart.wait( lock );

    assert( nullptr != m_pChartDataView );

    if ( m_bThreadDrawChartActive ) {  // exit thread if false without doing anything
      //if ( m_bReCalcViewPort ) {
        boost::posix_time::ptime now = ou::TimeSource::Instance().Internal(); // works with real vs simulation time

        static boost::posix_time::time_duration::fractional_seconds_type fs( 1 );
        auto now_ = now;
        boost::posix_time::ptime dtEnd = now_;
        if ( false ) {
          // chart moves at 1s step - not sure if this is trader friendly though
          boost::posix_time::time_duration td( 0, 0, 0, fs - now_.time_of_day().fractional_seconds() );
          dtEnd = now_ + td;
        }

        boost::posix_time::ptime dtBegin = dtEnd - m_tdViewPortWidth;

        if ( false ) {
          std::stringstream ss;
          ss << "vport=" << dtBegin << "," << m_tdViewPortWidth << "," << dtEnd;
          std::string s( ss.str() );
          std::cout << s << std::endl;
        }

        m_pChartDataView->SetViewPort( dtBegin, dtEnd );

        m_bReCalcViewPort = false;
      //}

      UpdateChartMaster();
    }
  }
}

void WinChartView::UpdateChartMaster() {

  wxSize size = this->GetClientSize();  // may not be able to do this cross thread
  m_chartMaster.SetChartDataView( m_pChartDataView );
  m_chartMaster.SetChartDimensions( size.GetWidth(), size.GetHeight() );

  m_chartMaster.SetOnDrawChart(
    [this]( bool bCursor, const MemBlock& m ){ // in background thread to draw composed chart into memory, and send to gui thread
      wxMemoryInputStream in( m.data, m.len );  // need this
      pwxBitmap_t p( new wxBitmap( wxImage( in, wxBITMAP_TYPE_BMP) ) ); // and need this to keep the drawn bitmap, then memblock can be reclaimed

      CallAfter([this,p,bCursor](){ // perform draw in gui thread
        if ( 0 != m_pChartBitmap.use_count() ) m_pChartBitmap.reset();
        m_pChartBitmap = p;  //  bit map remains for use in HandlePaint
        wxClientDC dc( this );
        dc.DrawBitmap( *m_pChartBitmap, 0, 0);

        if ( bCursor ) {
          SetCursor( wxStockCursor( wxCURSOR_ARROW ) );
        }
        else {
          SetCursor( wxStockCursor( wxCURSOR_BLANK ) );
        }

        m_bInDrawChart = false;
      });
    });

  m_chartMaster.DrawChart();
}

void WinChartView::UnbindEvents() {

  if ( m_bBound ) {

    m_timerGuiRefresh.Stop();

    SetChartDataView( nullptr );

    assert( Unbind( wxEVT_TIMER, &WinChartView::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() ) );

    assert( Unbind( wxEVT_PAINT, &WinChartView::HandlePaint, this ) );
    assert( Unbind( wxEVT_SIZE, &WinChartView::HandleSize, this ) );

    assert( Unbind( wxEVT_MOTION, &WinChartView::HandleMouse, this ) );
    assert( Unbind( wxEVT_MOUSEWHEEL, &WinChartView::HandleMouseWheel, this ) );
    assert( Unbind( wxEVT_ENTER_WINDOW, &WinChartView::HandleMouseEnter, this ) );
    assert( Unbind( wxEVT_LEAVE_WINDOW, &WinChartView::HandleMouseLeave, this ) );

    m_bBound = false;
  }

}

void WinChartView::OnDestroy( wxWindowDestroyEvent& event ) {

  UnbindEvents();

  assert( Unbind( wxEVT_DESTROY, &WinChartView::OnDestroy, this ) );

  event.Skip();  // auto followed by Destroy();
}

// crossing threads
// http://www.chartdir.com/forum/download_thread.php?bn=chartdir_support&thread=1144757575#N1144760096

} // namespace tf
} // namespace ou
