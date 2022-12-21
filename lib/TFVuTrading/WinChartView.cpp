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

#include <boost/asio/post.hpp>

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
  assert( !m_threadDrawChart.joinable() ); // inheriting class needs a skip in the OnDestroy
}

void WinChartView::Init() {

  m_state = EState::trail;

  m_bInDrawChart = false;
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

void WinChartView::CreateControls() {

  using executor_type = boost::asio::io_context::executor_type;

  m_pWork
    = std::make_unique<boost::asio::executor_work_guard<executor_type> >( boost::asio::make_work_guard( m_context ) );
  //if ( m_threadDrawChart.joinable() ) m_threadDrawChart.join(); // need to finish off any previous thread - may not be required here (only if it was to be re-used)
  m_threadDrawChart = std::move( std::thread( &WinChartView::ThreadDrawChart, this ) );

  BindEvents();

}

void WinChartView::BindEvents() {

  Bind( wxEVT_DESTROY, &WinChartView::OnDestroy, this, this->GetId() );

  Bind( wxEVT_PAINT, &WinChartView::HandlePaint, this, this->GetId() );
  Bind( wxEVT_SIZE, &WinChartView::HandleSize, this, this->GetId() );

  Bind( wxEVT_MOTION, &WinChartView::HandleMouse, this, this->GetId() );
  Bind( wxEVT_MOUSEWHEEL, &WinChartView::HandleMouseWheel, this, this->GetId() );
  Bind( wxEVT_ENTER_WINDOW, &WinChartView::HandleMouseEnter, this, this->GetId() );
  Bind( wxEVT_LEAVE_WINDOW, &WinChartView::HandleMouseLeave, this, this->GetId() );

  Bind( wxEVT_LEFT_UP, &WinChartView::HandleMouseLeftClick, this, this->GetId() );
  Bind( wxEVT_RIGHT_UP, &WinChartView::HandleMouseRightClick, this, this->GetId() );

  // this GuiRefresh initialization should come after all else
  m_timerGuiRefresh.SetOwner( this );
  Bind( wxEVT_TIMER, &WinChartView::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  m_timerGuiRefresh.Start( 200 );

}

void WinChartView::ThreadDrawChart() { // thread for initiating chart, work is posted
  m_context.run(); // run while work is active
}

// called from PanelChartHdf5::LoadDataAndGenerateChart
// called from PanelCharts::HandleInstrumentLiveChart
// called from PanelFinancialChart::HandleTreeEventitemActivated
void WinChartView::SetChartDataView( ou::ChartDataView* pChartDataView, bool bReCalcViewPort ) {
  std::scoped_lock<std::mutex> lock( m_mutexChartDataView );
  // TODO: need to sync with the gui refresh thread
  m_pChartDataView = pChartDataView; // TODO: need some additional tender loving care with this for the mutex
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
  event.Skip();
}

void WinChartView::HandleMouseLeftClick( wxMouseEvent& event ) {
  int nChart;
  double dblY;
  m_chartMaster.WorldCoord( nChart, dblY );
  LeftClick( nChart, dblY );
  event.Skip();
}

void WinChartView::HandleMouseRightClick( wxMouseEvent& event ) {
  int nChart;
  double dblY;
  m_chartMaster.WorldCoord( nChart, dblY );
  RightClick( nChart, dblY );
  event.Skip();
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

  DrawChart();

  event.Skip();
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
    dc.DrawBitmap( *m_pChartBitmap.get(), 0, 0 );
  }
}

void WinChartView::HandleSize( wxSizeEvent& event ) {
  // let the timer do the refresh instead?
  //this->RefreshRect( this->GetClientRect(), false );
  DrawChart();
}

void WinChartView::HandleGuiRefresh( wxTimerEvent& event ) {
  DrawChart();
}

// TODO: there may be an issue with cursor & no data, which locks up the gui
void WinChartView::DrawChart() {
  if ( m_threadDrawChart.joinable() ) {

    if ( m_pChartDataView ) {

      if ( !m_bInDrawChart ) {

        std::scoped_lock<std::mutex> lock( m_mutexChartDataView );
        if ( !m_bInDrawChart ) {
          m_bInDrawChart = true;

          boost::asio::post(
            m_context,
            [this](){
              std::scoped_lock<std::mutex> lock( m_mutexChartDataView );

              static const boost::posix_time::time_duration one_sec( 0, 0, 1 ); // provide a border

              switch ( m_state ) {
                case EState::trail:
                  {
                    const boost::posix_time::ptime dtEnd = ou::TimeSource::GlobalInstance().Internal() + one_sec; // works with real vs simulation time
                    const boost::posix_time::ptime dtBegin = dtEnd - m_tdViewPortWidth;

                    m_pChartDataView->SetViewPort( dtBegin, dtEnd );
                  }
                  break;
                case EState::review:
                  {
                    const ou::ChartDataView::ViewPort_t view = m_pChartDataView->GetExtents();
                    m_pChartDataView->SetViewPort( view.dtEnd - m_tdViewPortWidth, view.dtEnd + one_sec );
                  }
                  break;
              }

              UpdateChartMaster();

              m_bInDrawChart = false;
            });
        }
      }
    }
  }
}

void WinChartView::UpdateChartMaster() { // in worker thread with DrawChart started post

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

      });
    });

  m_chartMaster.DrawChart();
}

void WinChartView::UnbindEvents() {

  m_timerGuiRefresh.Stop();

  SetChartDataView( nullptr );

  assert( Unbind( wxEVT_TIMER, &WinChartView::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() ) );

  assert( Unbind( wxEVT_PAINT, &WinChartView::HandlePaint, this, this->GetId() ) );
  assert( Unbind( wxEVT_SIZE, &WinChartView::HandleSize, this, this->GetId() ) );

  assert( Unbind( wxEVT_MOTION, &WinChartView::HandleMouse, this, this->GetId() ) );
  assert( Unbind( wxEVT_MOUSEWHEEL, &WinChartView::HandleMouseWheel, this, this->GetId() ) );
  assert( Unbind( wxEVT_ENTER_WINDOW, &WinChartView::HandleMouseEnter, this, this->GetId() ) );
  assert( Unbind( wxEVT_LEAVE_WINDOW, &WinChartView::HandleMouseLeave, this, this->GetId() ) );

  assert( Unbind( wxEVT_LEFT_UP, &WinChartView::HandleMouseLeftClick, this, this->GetId() ) );
  assert( Unbind( wxEVT_RIGHT_UP, &WinChartView::HandleMouseRightClick, this, this->GetId() ) );

  assert( Unbind( wxEVT_DESTROY, &WinChartView::OnDestroy, this, this->GetId() ) );

}

void WinChartView::OnDestroy( wxWindowDestroyEvent& event ) {

  UnbindEvents();

  m_pWork.reset();
  if ( m_threadDrawChart.joinable() ) {
    m_threadDrawChart.join(); // to end
  }

  event.Skip();  // auto followed by Destroy();
}

// crossing threads
// http://www.chartdir.com/forum/download_thread.php?bn=chartdir_support&thread=1144757575#N1144760096

} // namespace tf
} // namespace ou
