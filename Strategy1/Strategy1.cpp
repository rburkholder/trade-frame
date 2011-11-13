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

  m_bReadyToDrawChart = false;

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Strategy Simulator" );
  m_pFrameMain->Center();
  m_pFrameMain->Show( true );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxHORIZONTAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  m_pPanelSimulationControl = new PanelSimulationControl( m_pFrameMain, wxID_ANY );
  m_sizerMain->Add( m_pPanelSimulationControl, 0, wxALIGN_LEFT|wxTOP|wxBOTTOM|wxLEFT, 5);
  m_pPanelSimulationControl->Show( true );

  //ID_CHART
  m_winChart = new wxWindow( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxSize(160, 90), wxNO_BORDER );
  m_sizerMain->Add( m_winChart, 1, wxALL|wxEXPAND, 5);
  wxWindowID id = m_winChart->GetId();
  m_winChart->Bind( wxEVT_PAINT, &AppStrategy1::HandlePaint, this, id );
  m_winChart->Bind( wxEVT_SIZE, &AppStrategy1::HandleSize, this, id );

//  m_pPanelFinancialChart = new PanelFinancialChart( m_pFrameMain, wxID_ANY );
//  m_pPanelFinancialChart->Show( true );

  m_pPanelSimulationControl->SetOnStartSimulation( MakeDelegate( this, &AppStrategy1::HandleBtnSimulationStart) );
  m_pPanelSimulationControl->SetOnDrawChart( MakeDelegate( this, &AppStrategy1::HandleBtnDrawChart) );

  m_pStrategy = new Strategy;

  return 1;

}

int AppStrategy1::OnExit() {

//  m_pPanelSimulationControl->SetOnStartSimulation( 0 );
//  m_pPanelSimulationControl->SetOnDrawChart( 0 );

  return 0;
}

void AppStrategy1::HandleBtnSimulationStart( void ) {
  m_pStrategy->Start( "" );
}

void AppStrategy1::HandleBtnDrawChart( void ) {
  m_bReadyToDrawChart = true;
  m_winChart->RefreshRect( m_winChart->GetRect(), false );
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
  m_winChart->RefreshRect( m_winChart->GetRect(), false );
}