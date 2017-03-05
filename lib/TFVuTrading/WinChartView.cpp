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
  
  m_bInDrawChart = false;
  m_bThreadDrawChartActive = false;

  m_tdViewPortWidth = boost::posix_time::time_duration( 0, 10, 0 );  // viewport width is 10 minutes, until we make it adjustable
  
  Bind( EVENT_DRAW_CHART, &WinChartView::HandleGuiDrawChart, this );

}

bool WinChartView::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

  wxWindow::Create( parent, id, pos, size, style );

  CreateControls();
//  if (GetSizer())     {
//      GetSizer()->SetSizeHints(this);
//  }
  return true;
}

void WinChartView::CreateControls() {    
  
  Bind( wxEVT_CLOSE_WINDOW, &WinChartView::OnClose, this );  // start close of windows and controls
  
  Bind( wxEVT_PAINT, &WinChartView::HandlePaint, this );
  Bind( wxEVT_SIZE, &WinChartView::HandleSize, this );
  
  Bind( wxEVT_MOUSEWHEEL, &WinChartView::HandleMouse, this );
  Bind( wxEVT_MOTION, &WinChartView::HandleMouse, this );
  Bind( wxEVT_LEAVE_WINDOW, &WinChartView::HandleMouse, this );
  Bind( wxEVT_ENTER_WINDOW, &WinChartView::HandleMouse, this );  

  // this GuiRefresh initialization should come after all else
  m_timerGuiRefresh.SetOwner( this );
  Bind( wxEVT_TIMER, &WinChartView::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  m_timerGuiRefresh.Start( 250 );

}

void WinChartView::StartThread( void ) {
  m_pThreadDrawChart = new boost::thread( &WinChartView::ThreadDrawChart1, this );
}

void WinChartView::SetChartDataView( ou::ChartDataView* pChartDataView ) {
  // need to make this thread save (drain the thread if this is assigning a new ChartDataView
  if ( m_bThreadDrawChartActive ) StopThread();
  m_pChartDataView = pChartDataView;
  if ( nullptr != m_pChartDataView ) StartThread();
}

void WinChartView::HandleMouse( wxMouseEvent& event ) { 
  if ( event.LeftIsDown() ) std::cout << "Left is down" << std::endl;
  if ( event.MiddleIsDown() ) std::cout << "Middle is down" << std::endl;
  if ( event.RightIsDown() ) std::cout << "Right is down" << std::endl;
  wxCoord x, y;
  event.GetPosition( &x, &y );
  std::cout << x << "," << y << std::endl;
//  std::cout << event.AltDown() << "," << event.ControlDown() << "," << event.ShiftDown() << std::endl;
//  std::cout << event.GetWheelAxis() << "," << event.GetWheelDelta() << "," << event.GetWheelRotation() << std::endl;
  // 0,120,-120
  //event.Skip();
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
  if ( 0 != m_pChartDataView ) {
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
void WinChartView::ThreadDrawChart1( void ) {
  m_bThreadDrawChartActive = true;
  boost::unique_lock<boost::mutex> lock(m_mutexThreadDrawChart);
  while ( m_bThreadDrawChartActive ) {
    m_cvThreadDrawChart.wait( lock );

    if ( m_bThreadDrawChartActive ) {  // exit thread if false without doing anything
      // need to deal with market closing time frame on expiry friday, no further calcs after market close on that day
      boost::posix_time::ptime now = ou::TimeSource::Instance().External();

      static boost::posix_time::time_duration::fractional_seconds_type fs( 1 );
      boost::posix_time::time_duration td( 0, 0, 0, fs - now.time_of_day().fractional_seconds() );
      boost::posix_time::ptime dtEnd = now + td; 

      boost::posix_time::ptime dtBegin = dtEnd - m_tdViewPortWidth;

      std::stringstream ss;
      ss << dtBegin << "," << m_tdViewPortWidth << "," << dtEnd;

      m_pChartDataView->SetViewPort( dtBegin, dtEnd );
      m_pChartDataView->SetThreadSafe( true );

      wxSize size = this->GetClientSize();  // may not be able to do this cross thread
      m_chartMaster.SetChartDimensions( size.GetWidth(), size.GetHeight() );
      m_chartMaster.SetChartDataView( m_pChartDataView );
      // could use lambda instead here
      m_chartMaster.SetOnDrawChart( MakeDelegate( this, &WinChartView::ThreadDrawChart2 ) );  // this line could be factored out?
      m_chartMaster.DrawChart( );
    }
  }
}

// background thread to draw composed chart into memory, and send to gui thread 
void WinChartView::ThreadDrawChart2( const MemBlock& m ) {
  wxMemoryInputStream in( m.data, m.len );  // need this
  pwxBitmap_t p( new wxBitmap( wxImage( in, wxBITMAP_TYPE_BMP) ) ); // and need this to keep the drawn bitmap, then memblock can be reclaimed
  QueueEvent( new EventDrawChart( EVENT_DRAW_CHART, -1, p ) ); // which will invoke HandleGuiDrawChart
}

// triggered by ThreadDrawChart2
// event in gui thread to draw on display from memory, bit map remains for use in HandlePaint
void WinChartView::HandleGuiDrawChart( EventDrawChart& event ) {
  if ( 0 != m_pChartBitmap.use_count() ) m_pChartBitmap.reset();
  m_pChartBitmap = event.GetBitmap();
  wxClientDC dc( this );
  dc.DrawBitmap( *m_pChartBitmap, 0, 0);
  m_bInDrawChart = false;
}

// this is superceded by ThreadDrawChart2/HandleGuiDrawChart when crossing threads
// http://www.chartdir.com/forum/download_thread.php?bn=chartdir_support&thread=1144757575#N1144760096
void WinChartView::HandleDrawChart( const MemBlock& m ) {
  wxMemoryInputStream in( m.data, m.len );
  wxBitmap bmp( wxImage( in, wxBITMAP_TYPE_BMP) );
  wxPaintDC cdc( this );  // can only be used inside of paint, use ClientDC otherwise
  cdc.DrawBitmap(bmp, 0, 0);
}

void WinChartView::StopThread( void ) {
  m_bThreadDrawChartActive = false;
  m_cvThreadDrawChart.notify_one();
  m_pThreadDrawChart->join();
  delete m_pThreadDrawChart;
  m_pThreadDrawChart = 0;
}

void WinChartView::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  
  m_timerGuiRefresh.Stop();
  this->Unbind( EVENT_DRAW_CHART, &WinChartView::HandleGuiDrawChart, this );
  
  StopThread();
  
  m_pChartBitmap.reset();
  
  event.Skip();  // auto followed by Destroy();
}

} // namespace tf
} // namespace ou
