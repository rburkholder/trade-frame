/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ChartInteractive.cpp
 * Author: raymond@burkholder.net
 * 
 * Created on October 16, 2016, 5:53 PM
 */

#include <wx/mstream.h>
#include <wx/bitmap.h>
#include <wx/dcclient.h>

#include "ChartInteractive.h"

ChartInteractive::ChartInteractive(): wxWindow() {
  Init();
}

ChartInteractive::ChartInteractive( 
  wxWindow* parent, wxWindowID id, 
  const wxPoint& pos, 
  const wxSize& size, 
  long style )
{
  
  Init();
  Create(parent, id, pos, size, style);
  
}

ChartInteractive::~ChartInteractive() {
}

void ChartInteractive::Init( void ) {
  m_pChartDataView = 0;
  //m_bPaintingChart = false;
  //m_bReadyToDrawChart = false;
}

bool ChartInteractive::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

  wxWindow::Create( parent, id, pos, size, style );

  CreateControls();
//  if (GetSizer())     {
//      GetSizer()->SetSizeHints(this);
//  }
//  Centre();
  return true;
}

void ChartInteractive::CreateControls() {    
  
  Bind( wxEVT_CLOSE_WINDOW, &ChartInteractive::OnClose, this );  // start close of windows and controls
  
  Bind( wxEVT_PAINT, &ChartInteractive::HandlePaint, this );
  Bind( wxEVT_SIZE, &ChartInteractive::HandleSize, this );
  
  Bind( wxEVT_MOUSEWHEEL, &ChartInteractive::HandleMouse, this );
  Bind( wxEVT_MOTION, &ChartInteractive::HandleMouse, this );
  Bind( wxEVT_LEAVE_WINDOW, &ChartInteractive::HandleMouse, this );
  Bind( wxEVT_ENTER_WINDOW, &ChartInteractive::HandleMouse, this );  

  // this GuiRefresh initialization should come after all else
  m_timerGuiRefresh.SetOwner( this );
  Bind( wxEVT_TIMER, &ChartInteractive::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  m_timerGuiRefresh.Start( 250 );

}

void ChartInteractive::HandleMouse( wxMouseEvent& event ) { 
//  if ( event.LeftIsDown() ) std::cout << "Left is down" << std::endl;
//  if ( event.MiddleIsDown() ) std::cout << "Middle is down" << std::endl;
//  if ( event.RightIsDown() ) std::cout << "Right is down" << std::endl;
//  wxCoord x, y;
//  event.GetPosition( &x, &y );
//  std::cout << x << "," << y << std::endl;
//  std::cout << event.AltDown() << "," << event.ControlDown() << "," << event.ShiftDown() << std::endl;
//  std::cout << event.GetWheelAxis() << "," << event.GetWheelDelta() << "," << event.GetWheelRotation() << std::endl;
  // 0,120,-120
  event.Skip();
}

void ChartInteractive::HandleSize( wxSizeEvent& event ) { 
  // let the timer do the refresh instead
  //this->RefreshRect( this->GetClientRect(), false );
}

void ChartInteractive::HandlePaint( wxPaintEvent& event ) {
  if ( 0 != m_pChartDataView ) {
    try {
      //m_bPaintingChart = true;
      wxSize size = this->GetClientSize();
      m_chartMaster.SetChartDimensions( size.GetWidth(), size.GetHeight() );
      // turn this into a lambda instead
      m_chartMaster.SetOnDrawChart( MakeDelegate( this, &ChartInteractive::HandleDrawChart ) );
      m_chartMaster.DrawChart( );
    }
    catch (...) {
    }
  }
  //m_bPaintingChart = false;
}

// http://www.chartdir.com/forum/download_thread.php?bn=chartdir_support&thread=1144757575#N1144760096
void ChartInteractive::HandleDrawChart( const MemBlock& m ) {
  wxMemoryInputStream in( m.data, m.len );
  wxBitmap bmp( wxImage( in, wxBITMAP_TYPE_BMP) );
  wxPaintDC cdc( this );
  cdc.DrawBitmap(bmp, 0, 0);
}

void ChartInteractive::HandleGuiRefresh( wxTimerEvent& event ) {
  if ( 0 != m_pChartDataView ) {
    this->RefreshRect( this->GetClientRect(), false );
  }
}

void ChartInteractive::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  m_timerGuiRefresh.Stop();
  event.Skip();  // auto followed by Destroy();
}

