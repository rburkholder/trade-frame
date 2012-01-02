/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

// Strategy1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include <wx/mstream.h>
#include <wx/bitmap.h>

#include "Strategy1.h"

IMPLEMENT_APP(AppStrategy1)

bool AppStrategy1::OnInit() {

  m_pStrategy = 0;

  m_bReadyToDrawChart = false;

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Strategy Simulator" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 4400, 1500 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxHORIZONTAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxVERTICAL );
  m_sizerMain->Add( m_sizerControls, 0, wxALL, 5 );

  m_pPanelSimulationControl = new PanelSimulationControl( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelSimulationControl, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxBOTTOM, 5);
  m_pPanelSimulationControl->Show( true );

  m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelProviderControl, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxBOTTOM, 5);
  m_pPanelProviderControl->Show( true );

  m_winChart = new wxWindow( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxSize(160, 90), wxNO_BORDER );
  m_sizerMain->Add( m_winChart, 1, wxALL|wxEXPAND, 5);
  wxWindowID idChart = m_winChart->GetId();
  m_winChart->Bind( wxEVT_PAINT, &AppStrategy1::HandlePaint, this, idChart );
  m_winChart->Bind( wxEVT_SIZE, &AppStrategy1::HandleSize, this, idChart );

  m_pPanelSimulationControl->SetOnStartSimulation( MakeDelegate( this, &AppStrategy1::HandleBtnSimulationStart) );
  m_pPanelSimulationControl->SetOnDrawChart( MakeDelegate( this, &AppStrategy1::HandleBtnDrawChart) );

  LinkToPanelProviderControl();

  m_pFrameMain->Show( true );

  return 1;

}

int AppStrategy1::OnExit() {

//  m_pPanelSimulationControl->SetOnStartSimulation( 0 );
//  m_pPanelSimulationControl->SetOnDrawChart( 0 );

  return 0;
}

void AppStrategy1::HandleBtnSimulationStart( void ) {
  if ( 0 != m_pStrategy ) {
    delete m_pStrategy;
  }
  m_pStrategy = new Strategy( m_pData1Provider, m_pExecutionProvider );
  switch ( Mode() ) {
  case EModeUnknown:
    // do nothing.  maybe emit a message somewhere
    break;
  case EModeSimulation:
    m_pStrategy->Start( m_pPanelSimulationControl->GetGroupDirectory() );
    break;
  case EModeLive:
    m_pStrategy->Start();
    break;
  }
  
}

void AppStrategy1::HandleBtnDrawChart( void ) {
  m_bReadyToDrawChart = true;
  m_winChart->RefreshRect( m_winChart->GetClientRect(), false );
}

void AppStrategy1::HandleDrawChart( const MemBlock& m ) {
  wxMemoryInputStream in( m.data, m.len );
  wxBitmap bmp( wxImage( in, wxBITMAP_TYPE_BMP) );
  wxPaintDC cdc( m_winChart );
  cdc.DrawBitmap(bmp, 0, 0);
}

void AppStrategy1::HandlePaint( wxPaintEvent& event ) {
  if ( m_bReadyToDrawChart ) {
    try {
      wxSize size = m_winChart->GetClientSize();
      m_chart.SetChartDimensions( size.GetWidth(), size.GetHeight() );
      m_chart.SetChartDataView( &m_pStrategy->GetChartDataView() );
      m_chart.SetOnDrawChart( MakeDelegate( this, &AppStrategy1::HandleDrawChart ) );
      m_chart.DrawChart( );
    }
    catch (...) {
    }
  }
}

void AppStrategy1::HandleSize( wxSizeEvent& event ) { 
  m_winChart->RefreshRect( m_winChart->GetClientRect(), false );
}

void AppStrategy1::HandleFrameMainSize( wxSizeEvent& event ) {
  event.Skip();
}

void AppStrategy1::HandleFrameMainMove( wxMoveEvent& event ) {
  event.Skip();
}

