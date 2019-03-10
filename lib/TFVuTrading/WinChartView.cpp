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

#include <boost/date_time/posix_time/posix_time.hpp>

#include <wx/mstream.h>
#include <wx/bitmap.h>
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

void WinChartView::Init( void ) {

  m_bBound = false;
  m_bInDrawChart = false;
  m_bThreadDrawChartActive = false;
  m_pThreadDrawChart = nullptr;
  m_pChartDataView = nullptr;

  m_tdViewPortWidth = boost::posix_time::time_duration( 0, 10, 0 );  // viewport width is 10 minutes, until we make it adjustable

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

    Bind( wxEVT_PAINT, &WinChartView::HandlePaint, this );
    Bind( wxEVT_SIZE, &WinChartView::HandleSize, this );

    Bind( wxEVT_MOTION, &WinChartView::HandleMouse, this );
    Bind( wxEVT_MOUSEWHEEL, &WinChartView::HandleMouseWheel, this );
    Bind( wxEVT_ENTER_WINDOW, &WinChartView::HandleMouseEnter, this );
    Bind( wxEVT_LEAVE_WINDOW, &WinChartView::HandleMouseLeave, this );

    // this GuiRefresh initialization should come after all else
    m_timerGuiRefresh.SetOwner( this );
    Bind( wxEVT_TIMER, &WinChartView::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
    m_timerGuiRefresh.Start( 250 );

    m_bBound = true;
  }

}

void WinChartView::CreateControls() {

  //Bind( wxEVT_CLOSE_WINDOW, &WinChartView::OnClose, this );  // not called for child windows
  Bind( wxEVT_DESTROY, &WinChartView::OnDestroy, this );

  BindEvents();

}

void WinChartView::StartThread( void ) {
  m_pThreadDrawChart = new boost::thread( &WinChartView::ThreadDrawChart, this );
}

void WinChartView::StopThread( void ) {
  m_bThreadDrawChartActive = false;
  m_cvThreadDrawChart.notify_one();
  m_pThreadDrawChart->join();
  delete m_pThreadDrawChart;
  m_pThreadDrawChart = nullptr;
}

// called from PanelChartHdf5::LoadDataAndGenerateChart
// called from PanelCharts::HandleInstrumentLiveChart
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
  //event.Skip();
}

void WinChartView::HandleMouseWheel( wxMouseEvent& event ) {
  int delta = event.GetWheelDelta();
  int rotation = event.GetWheelRotation(); // has positive, negative, use delta to normalize
  bool bShift = event.ShiftDown();
  bool bControl = event.ControlDown();
  bool bAlt = event.AltDown();
  //std::cout
  //      << "Wheel: " << delta << "," << rotation << ",sca:"
  //    << bShift << bControl << bAlt
  //    << std::endl;

  // TODO: will need to keep to within a min/max
  if ( 0 > rotation ) {
    m_tdViewPortWidth *= 12;
    m_tdViewPortWidth /= 10;
  }
  else {
    m_tdViewPortWidth *= 10;
    m_tdViewPortWidth /= 12;
  }

  DrawChart();
  //event.Skip();
}

void WinChartView::HandleMouseEnter( wxMouseEvent& event ) {
  //std::cout << "mouse enter" << std::endl;
  event.Skip();
}

void WinChartView::HandleMouseLeave( wxMouseEvent& event ) {
  //std::cout << "mouse leave" << std::endl;
  event.Skip();
}

void WinChartView::HandlePaint( wxPaintEvent& event ) {
  if ( 0 != m_pChartBitmap.use_count() ) {
    wxPaintDC dc( this );
    dc.DrawBitmap( *m_pChartBitmap.get(), 0, 0);
  }
}

// placeholder for unused code
void WinChartView::ManualDraw( void ) {

// ====
  if ( nullptr != m_pChartDataView ) {
    try {
      //m_bPaintingChart = true;
      wxSize size = this->GetClientSize();
      m_chartMaster.SetChartDimensions( size.GetWidth(), size.GetHeight() );
      // turn this into a lambda instead
      m_chartMaster.SetOnDrawChart( MakeDelegate( this, &WinChartView::HandleDrawChart ) );
      m_chartMaster.DrawChart( );
    }
    catch (...) {
    }
  }
  //m_bPaintingChart = false;
}


void WinChartView::HandleSize( wxSizeEvent& event ) {
  // let the timer do the refresh instead?
  //this->RefreshRect( this->GetClientRect(), false );
  DrawChart();
}

void WinChartView::HandleGuiRefresh( wxTimerEvent& event ) {
  DrawChart();
}

void WinChartView::DrawChart( void ) {
  if ( m_bThreadDrawChartActive ) {
    m_bInDrawChart = true;
    m_cvThreadDrawChart.notify_one();
  }
}

// could change this into a worker future/promise solution, or use asio to submit jobs or packages
void WinChartView::ThreadDrawChart( void ) {
  m_bThreadDrawChartActive = true;
  boost::unique_lock<boost::mutex> lock(m_mutexThreadDrawChart);
  while ( m_bThreadDrawChartActive ) {
    m_cvThreadDrawChart.wait( lock );

    assert( nullptr != m_pChartDataView );

    if ( m_bThreadDrawChartActive ) {  // exit thread if false without doing anything
      // need to deal with market closing time frame on expiry friday, no further calcs after market close on that day
      if ( m_bReCalcViewPort ) {
        boost::posix_time::ptime now = ou::TimeSource::Instance().External();

        // chart moves at 1s step - not sure if this is trader friendly though
        static boost::posix_time::time_duration::fractional_seconds_type fs( 1 );
        boost::posix_time::time_duration td( 0, 0, 0, fs - now.time_of_day().fractional_seconds() );
        boost::posix_time::ptime dtEnd = now + td;

        boost::posix_time::ptime dtBegin = dtEnd - m_tdViewPortWidth;

        std::stringstream ss;
        ss << dtBegin << "," << m_tdViewPortWidth << "," << dtEnd;

        m_pChartDataView->SetViewPort( dtBegin, dtEnd );
      }

      m_pChartDataView->SetThreadSafe( true );  // not sure what this is, does it belong in side the conditional?

      UpdateChartMaster();
    }
  }
}

void WinChartView::UpdateChartMaster() {
  wxSize size = this->GetClientSize();  // may not be able to do this cross thread
  m_chartMaster.SetChartDataView( m_pChartDataView );
  m_chartMaster.SetChartDimensions( size.GetWidth(), size.GetHeight() );
  // could use lambda instead here
  m_chartMaster.SetOnDrawChart( MakeDelegate( this, &WinChartView::CallBackDrawChart ) );  // this line could be factored out?
  m_chartMaster.DrawChart( );
}

// background thread to draw composed chart into memory, and send to gui thread
void WinChartView::CallBackDrawChart( const MemBlock& m ) {
  wxMemoryInputStream in( m.data, m.len );  // need this
  pwxBitmap_t p( new wxBitmap( wxImage( in, wxBITMAP_TYPE_BMP) ) ); // and need this to keep the drawn bitmap, then memblock can be reclaimed
  //QueueEvent( new EventDrawChart( EVENT_DRAW_CHART, -1, p ) ); // which will invoke HandleGuiDrawChart
  CallAfter([this,p](){
    if ( 0 != m_pChartBitmap.use_count() ) m_pChartBitmap.reset();
    m_pChartBitmap = p;  //  bit map remains for use in HandlePaint
    wxClientDC dc( this );
    dc.DrawBitmap( *m_pChartBitmap, 0, 0);
    m_bInDrawChart = false;
  });
}

// this is superceded by ThreadDrawChart2/HandleGuiDrawChart when crossing threads
// http://www.chartdir.com/forum/download_thread.php?bn=chartdir_support&thread=1144757575#N1144760096
// placeholder from ManualDraw
void WinChartView::HandleDrawChart( const MemBlock& m ) {
  wxMemoryInputStream in( m.data, m.len );
  wxBitmap bmp( wxImage( in, wxBITMAP_TYPE_BMP) );
  wxPaintDC cdc( this );  // can only be used inside of paint, use ClientDC otherwise
  cdc.DrawBitmap(bmp, 0, 0);
}

void WinChartView::UnbindEvents( void ) {

  if ( m_bBound ) {

    SetChartDataView( nullptr );

    m_timerGuiRefresh.Stop();
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


} // namespace tf
} // namespace ou
