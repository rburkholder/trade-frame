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

  SetLive_trail();

  m_stateMouse = EMouse::NothingSpecial;

  m_bInDrawChart = false;
  m_pChartDataView = nullptr;

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

  Bind( wxEVT_MOTION, &WinChartView::HandleMouseMotion, this, this->GetId() );
  Bind( wxEVT_MOUSEWHEEL, &WinChartView::HandleMouseWheel, this, this->GetId() );

  Bind( wxEVT_ENTER_WINDOW, &WinChartView::HandleMouseEnter, this, this->GetId() );
  Bind( wxEVT_LEAVE_WINDOW, &WinChartView::HandleMouseLeave, this, this->GetId() );

  Bind( wxEVT_LEFT_DOWN, &WinChartView::HandleMouseLeftDown, this, this->GetId() );
  Bind( wxEVT_LEFT_UP,   &WinChartView::HandleMouseLeftUp,   this, this->GetId() );

  Bind( wxEVT_RIGHT_DOWN, &WinChartView::HandleMouseRightDown, this, this->GetId() );
  Bind( wxEVT_RIGHT_UP,   &WinChartView::HandleMouseRightUp,   this, this->GetId() );

  // this GuiRefresh initialization should come after all else
  m_timerGuiRefresh.SetOwner( this );
  Bind( wxEVT_TIMER, &WinChartView::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  m_timerGuiRefresh.Start( 250 );

}

void WinChartView::ThreadDrawChart() { // thread for initiating chart, work is posted
  m_context.run(); // run while work is active
}

// called from PanelChartHdf5::LoadDataAndGenerateChart
// called from PanelCharts::HandleInstrumentLiveChart
// called from PanelFinancialChart::HandleTreeEventitemActivated
void WinChartView::SetChartDataView( ou::ChartDataView* pChartDataView ) {
  std::scoped_lock<std::mutex> lock( m_mutexChartDataView );   // PROBLEM line 509 lock 487
  // TODO: need to sync with the gui refresh thread
  m_pChartDataView = pChartDataView; // TODO: need some additional tender loving care with this for the mutex
  if ( m_pChartDataView ) {
    m_vpDataViewVisual = m_vpDataViewExtents = m_pChartDataView->GetExtents(); // TODO: may not want this if to maintain continuity across charts
  }
  else { // nullptr
    m_vpDataViewVisual = m_vpDataViewExtents = ViewPort_t();
  }
}

void WinChartView::HandleMouseMotion( wxMouseEvent& event ) {
  //if ( event.LeftIsDown() ) std::cout << "Left is down" << std::endl;
  //if ( event.MiddleIsDown() ) std::cout << "Middle is down" << std::endl;
  //if ( event.RightIsDown() ) std::cout << "Right is down" << std::endl;
  //std::cout << x << "," << y << std::endl;
  //std::cout << event.AltDown() << "," << event.ControlDown() << "," << event.ShiftDown() << std::endl;
  //std::cout << event.GetWheelAxis() << "," << event.GetWheelDelta() << "," << event.GetWheelRotation() << std::endl;
  // 0,120,-120

  {
    wxCoord x, y;
    event.GetPosition( &x, &y );
    m_chartMaster.SetCrossHairPosition( x, y );
  }

  int xLeft, xX, xRight;
  m_chartMaster.GetX( xLeft, xX, xRight );

  if ( m_vpDataViewVisual.HasBoth() ) {

    assert ( m_vpDataViewVisual.dtBegin <= m_vpDataViewVisual.dtEnd );

    //assert( m_vpDataViewVisual.dtBegin >= m_vpDataViewExtents.dtBegin );
    //assert( m_vpDataViewVisual.dtEnd <= m_vpDataViewExtents.dtEnd );

    switch ( m_stateMouse ) {
      case EMouse::Down:
        if ( event.Dragging() ) {
          m_stateMouse = EMouse::Drag;
        }
        // fall into next state
      case EMouse::Drag:
        // TODO: update chart for intermediate positions? or just change cursor?
        if ( m_coordXStart != xX ) {
          int width = xRight - xLeft;
          if ( 10 <= ( width ) ) {

            int distance {};
            if ( xX > m_coordXStart ) {
              distance = xX - m_coordXStart;
            }
            else {
              distance = m_coordXStart - xX;
            }

            boost::posix_time::time_duration tdMovement;
            tdMovement  = m_tdViewPortWidth * distance;
            tdMovement /= width;

            if ( xX < m_coordXStart ) {
              m_vpDataViewVisual.dtBegin += tdMovement;
              m_vpDataViewVisual.dtEnd += tdMovement;

              if ( m_vpDataViewVisual.dtBegin < m_vpDataViewExtents.dtBegin ) {
                m_vpDataViewVisual.dtBegin = m_vpDataViewExtents.dtBegin;
                if ( m_vpDataViewVisual.dtBegin >= m_vpDataViewVisual.dtEnd ) {
                  m_vpDataViewVisual.dtEnd = m_vpDataViewVisual.dtBegin + m_tdViewPortWidth;
                }
              }

              if ( m_vpDataViewVisual.dtEnd >= m_vpDataViewExtents.dtEnd ) {
                m_vpDataViewVisual.dtEnd = m_vpDataViewExtents.dtEnd;
                m_tdViewPortWidth = m_vpDataViewVisual.dtEnd - m_vpDataViewVisual.dtBegin;
                //m_state = m_bSim ? EState::sim_trail : EState::live_trail;
              }

              // Still having problems here
              assert ( m_vpDataViewVisual.dtBegin <= m_vpDataViewVisual.dtEnd );

            }
            else {
              m_vpDataViewVisual.dtBegin -= tdMovement;
              m_vpDataViewVisual.dtEnd -= tdMovement;

              if ( m_vpDataViewVisual.dtBegin < m_vpDataViewExtents.dtBegin ) {
                m_vpDataViewVisual.dtBegin = m_vpDataViewExtents.dtBegin;
                if ( m_vpDataViewVisual.dtBegin >= m_vpDataViewVisual.dtEnd ) {
                  m_vpDataViewVisual.dtEnd = m_vpDataViewVisual.dtBegin + m_tdViewPortWidth;
                }
              }

              if ( m_vpDataViewVisual.dtEnd < m_vpDataViewExtents.dtEnd ) {
                //m_state = m_bSim ? EState::sim_review : EState::live_review;
                //m_vpDataViewVisual = ViewPort_t( m_vpDataViewExtents.dtEnd - tdNewWidth, m_vpDataViewExtents.dtEnd );
              }

              assert ( m_vpDataViewVisual.dtBegin <= m_vpDataViewVisual.dtEnd );

            }
          }

          m_coordXStart = xX;

        }
        break;
      case EMouse::NothingSpecial:
        break;
    }

    if ( xLeft < xRight ) { // check for non-zer as well?
      boost::posix_time::time_duration tdCursor;
      boost::posix_time::time_duration tdDelta;

      tdDelta = m_vpDataViewVisual.dtEnd - m_vpDataViewVisual.dtBegin;

      tdCursor = tdDelta * ( xX - xLeft );
      tdCursor /= ( xRight - xLeft );

      boost::posix_time::ptime dtCursor =  m_vpDataViewVisual.dtBegin + tdCursor;

      //boost::posix_time::time_duration td( dtCursor.time_of_day() );
      //std::string sTime = boost::posix_time::to_simple_string( td );
      std::string sDT = boost::posix_time::to_simple_string( dtCursor );
      m_chartMaster.SetCrossHairTime( sDT );
    }

  }
  else {
    assert( true );  // test point
  }

  DrawChart(); // after CrossHairPosition

  event.Skip();
}

void WinChartView::HandleMouseLeftDown( wxMouseEvent& event ) {
  //wxCoord x, y;
  //event.GetPosition( &x, &y );
  int xLeft, xX, xRight;
  m_chartMaster.GetX( xLeft, xX, xRight );
  m_coordXStart = xX;
  m_stateMouse = EMouse::Down;
  event.Skip();
}

void WinChartView::HandleMouseLeftUp( wxMouseEvent& event ) {
  switch ( m_stateMouse ) {
    case EMouse::NothingSpecial:
    case EMouse::Down:
      HandleMouseLeftClick(); // only on non-movement
      break;
    case EMouse::Drag:
      // update chart with MouseMove
      break;
  }

  m_stateMouse = EMouse::NothingSpecial;

  event.Skip();
}

void WinChartView::HandleMouseRightDown( wxMouseEvent& event ) {
  event.Skip();
}

void WinChartView::HandleMouseRightUp( wxMouseEvent& event ) {
  HandleMouseRightClick(); // only on non-movement
  event.Skip();
}

void WinChartView::HandleMouseLeftClick() {
  int nChart;
  double dblY;
  m_chartMaster.GetWorldCoordY( nChart, dblY );
  LeftClick( nChart, dblY );
}

void WinChartView::HandleMouseRightClick() {
  int nChart;
  double dblY;
  m_chartMaster.GetWorldCoordY( nChart, dblY );
  RightClick( nChart, dblY );
}

void WinChartView::HandleMouseWheel( wxMouseEvent& event ) {

  static const boost::posix_time::time_duration tdTenSeconds( 0, 0, 10 );

  //int delta = event.GetWheelDelta();
  int rotation = event.GetWheelRotation(); // has positive, negative, use delta to normalize

  int xLeft, xX, xRight;
  m_chartMaster.GetX( xLeft, xX, xRight );

  if ( xLeft < xRight ) { // needs inequality for tdCursorOld divisor
    // zoom in/out around cursor
    // TODO: use this to provide date/time on cursor

    boost::posix_time::time_duration tdDeltaOld;
    boost::posix_time::time_duration tdDeltaNew;
    boost::posix_time::time_duration tdCursorOld; // offset from left
    boost::posix_time::time_duration tdCursorNew; // offset from left
    boost::posix_time::ptime dtCursor;

    bool bBegin = m_vpDataViewVisual.HasBegin();
    bool bEnd = m_vpDataViewVisual.HasEnd();

    if ( bBegin && bEnd ) {

      //assert( m_vpDataViewVisual.dtBegin >= m_vpDataViewExtents.dtBegin );
      //assert( m_vpDataViewVisual.dtEnd <= m_vpDataViewExtents.dtEnd );

      assert ( m_vpDataViewVisual.dtBegin <= m_vpDataViewVisual.dtEnd );

      tdDeltaOld = m_vpDataViewVisual.dtEnd - m_vpDataViewVisual.dtBegin;

      tdCursorOld = tdDeltaOld * ( xX - xLeft );
      tdCursorOld /= ( xRight - xLeft );

      dtCursor =  m_vpDataViewVisual.dtBegin + tdCursorOld;

      if ( 0 > rotation ) { // expand width

        tdDeltaNew = tdDeltaOld * 12;
        tdDeltaNew /= 10;

        tdCursorNew = tdDeltaNew * ( xX - xLeft );
        tdCursorNew /= ( xRight - xLeft );

        m_vpDataViewVisual.dtBegin = dtCursor - tdCursorNew;
        m_vpDataViewVisual.dtEnd   = m_vpDataViewVisual.dtBegin + tdDeltaNew;

        if ( m_vpDataViewVisual.dtBegin < m_vpDataViewExtents.dtBegin ) {
          m_vpDataViewVisual.dtBegin = m_vpDataViewExtents.dtBegin;
          if ( m_vpDataViewVisual.dtBegin >= m_vpDataViewVisual.dtEnd ) {
            m_vpDataViewVisual.dtEnd = m_vpDataViewVisual.dtBegin + tdDeltaNew;
          }
        }

        if ( m_vpDataViewVisual.dtEnd >= m_vpDataViewExtents.dtEnd ) {
          //m_state = m_bSim ? EState::sim_trail : EState::live_trail;
          m_vpDataViewVisual = ViewPort_t( m_vpDataViewExtents.dtEnd - tdDeltaNew, m_vpDataViewExtents.dtEnd );
        }

        assert ( m_vpDataViewVisual.dtBegin <= m_vpDataViewVisual.dtEnd );

        tdDeltaNew = m_vpDataViewVisual.dtEnd - m_vpDataViewVisual.dtBegin;

      }
      else { // reduce width

        tdDeltaNew = tdDeltaOld * 10;
        tdDeltaNew /= 12;

        if ( tdDeltaNew < tdTenSeconds ) tdDeltaNew = tdTenSeconds;

        tdCursorNew = tdDeltaNew * ( xX - xLeft );
        tdCursorNew /= ( xRight - xLeft );

        m_vpDataViewVisual.dtBegin = dtCursor - tdCursorNew;
        m_vpDataViewVisual.dtEnd   = m_vpDataViewVisual.dtBegin + tdDeltaNew;

        assert ( m_vpDataViewVisual.dtBegin <= m_vpDataViewVisual.dtEnd );

        if ( m_vpDataViewVisual.dtEnd < m_vpDataViewExtents.dtEnd ) {
          //m_state = m_bSim ? EState::sim_review : EState::live_review;
        }

      }

      m_tdViewPortWidth = tdDeltaNew;

      //boost::posix_time::time_duration td( dtCursor.time_of_day() );
      //std::string sTime = boost::posix_time::to_simple_string( td );
      std::string sDT = boost::posix_time::to_simple_string( dtCursor );
      m_chartMaster.SetCrossHairTime( sDT );

      DrawChart();
    }

  }
  else {
    assert( true ); // test point
  }

  event.Skip();
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

void WinChartView::SetLive_trail() {
  m_bSim = false;
  m_state = EState::live_trail;
}

void WinChartView::SetLive_review() {
  m_bSim = false;
  m_state = EState::live_review;
}

void WinChartView::SetSim_review() {
  m_bSim = true;
  m_state = EState::sim_review;
}

void WinChartView::SetSim_trail() {
  m_bSim = true;
  m_state = EState::sim_trail;
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

              m_vpDataViewExtents = m_pChartDataView->GetExtents(); // TODO: obtain just end extent?

              switch ( m_state ) {
                case EState::live_trail:
                  m_vpDataViewVisual.dtEnd = ou::TimeSource::GlobalInstance().Internal() + one_sec; // works with real vs simulation time
                  m_vpDataViewVisual.dtBegin = m_vpDataViewVisual.dtEnd - m_tdViewPortWidth;
                  break;
                case EState::live_review:
                  break;
                case EState::sim_trail:
                  m_vpDataViewVisual = ViewPort_t( m_vpDataViewExtents.dtEnd - m_tdViewPortWidth, m_vpDataViewExtents.dtEnd + one_sec );
                  break;
                case EState::sim_review:
                  break;
              }

              //std::cout << "SetViewPort: " << m_vpDataViewVisual.dtBegin << " - " << m_vpDataViewVisual.dtEnd << std::endl;
              m_pChartDataView->SetViewPort( m_vpDataViewVisual );

              UpdateChartMaster();  // PROBLEM in lock 1

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

  m_chartMaster.DrawChart();   // PROBLEM in lock 2
}

void WinChartView::UnbindEvents() {

  m_timerGuiRefresh.Stop();

  SetChartDataView( nullptr );

  assert( Unbind( wxEVT_TIMER, &WinChartView::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() ) );

  assert( Unbind( wxEVT_PAINT, &WinChartView::HandlePaint, this, this->GetId() ) );
  assert( Unbind( wxEVT_SIZE, &WinChartView::HandleSize, this, this->GetId() ) );

  assert( Unbind( wxEVT_MOTION, &WinChartView::HandleMouseMotion, this, this->GetId() ) );
  assert( Unbind( wxEVT_MOUSEWHEEL, &WinChartView::HandleMouseWheel, this, this->GetId() ) );

  assert( Unbind( wxEVT_ENTER_WINDOW, &WinChartView::HandleMouseEnter, this, this->GetId() ) );
  assert( Unbind( wxEVT_LEAVE_WINDOW, &WinChartView::HandleMouseLeave, this, this->GetId() ) );

  assert( Unbind( wxEVT_LEFT_DOWN, &WinChartView::HandleMouseLeftDown, this, this->GetId() ) );
  assert( Unbind( wxEVT_LEFT_UP,   &WinChartView::HandleMouseLeftUp,   this, this->GetId() ) );

  assert( Unbind( wxEVT_RIGHT_DOWN, &WinChartView::HandleMouseRightDown, this, this->GetId() ) );
  assert( Unbind( wxEVT_RIGHT_UP,   &WinChartView::HandleMouseRightUp,   this, this->GetId() ) );

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
