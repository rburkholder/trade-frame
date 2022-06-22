/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include "stdafx.h"

#include <wx/mstream.h>

#ifdef _M_X64
#include <OUCharting/ChartDirector64/chartdir.h>
#else
#include <OUCharting/ChartDirector/chartdir.h>
#endif

#include "TestWx.h"

IMPLEMENT_APP(AppTestWx)

bool AppTestWx::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "TestWx" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 800, 500 );
  SetTopWindow( m_pFrameMain );

  // Sizer for FrameMain
  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  // Sizer for Controls
  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerControls, 0, wxEXPAND|wxLEFT|wxTOP|wxRIGHT, 5 );
  //panelSplitterRight->SetSizer( m_sizerControls );
  //sizerRight->Add( m_sizerControls, 0, wxStretch::wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelLogging, 1, wxALL | wxEXPAND, 0);
//  m_pPanelLogging->Show( true );

  m_pPanel = new wxPanel( m_pFrameMain, wxID_ANY );
  m_sizerMain->Add( m_pPanel, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);


  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pFrameMain->Show( true );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppTestWx::OnClose, this );  // start close of windows and controls

  FrameMain::vpItems_t vItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  vItems.push_back( new mi( "test", MakeDelegate( this, &AppTestWx::TestChart ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  //std::cout << wxSETUPH_PATH_STR << std::endl;

  return 1;

}

void AppTestWx::TestChart( void ) {

  typedef std::vector<int> vColours_t;
  vColours_t m_vColours;

  // http://krazydad.com/tutorials/makecolors.php
  double freq = 0.1;
  for ( int i = 0.0; i < 32.0; ++i ) {
    int red = std::sin( i * freq + 0.0 ) * 127 + 128;
    int grn = std::sin( i * freq + 2.0 ) * 127 + 128;
    int blu = std::sin( i * freq + 4.0 ) * 127 + 128;
    m_vColours.push_back( ( ( ( red << 8 ) + grn ) << 8 ) + blu );
  }

  typedef std::vector<double> vDoubles_t;

  vDoubles_t m_vx;
  m_vx.push_back( 1 );
  m_vx.push_back( 2 );
  m_vx.push_back( 3 );
  m_vx.push_back( 9 );
  m_vx.push_back( 3 );
  m_vx.push_back( 2 );
  m_vx.push_back( 1 );
  m_vx.push_back( 6 );
  m_vx.push_back( 4 );
  m_vx.push_back( 3 );
  m_vx.push_back( 2 );
  m_vx.push_back( 7 );
  m_vx.push_back( 5 );
  m_vx.push_back( 8 );
  m_vx.push_back( 2 );
  m_vx.push_back( 3 );
  m_vx.push_back( 5 );
  m_vx.push_back( 8 );

  vDoubles_t m_vy;
  m_vy.push_back( 1 );
  m_vy.push_back( 2 );
  m_vy.push_back( 3 );
  m_vy.push_back( 4 );
  m_vy.push_back( 5 );
  m_vy.push_back( 6 );
  m_vy.push_back( 5 );
  m_vy.push_back( 2 );
  m_vy.push_back( 4 );
  m_vy.push_back( 7 );
  m_vy.push_back( 8 );
  m_vy.push_back( 3 );
  m_vy.push_back( 9 );
  m_vy.push_back( 2 );
  m_vy.push_back( 5 );
  m_vy.push_back( 9 );
  m_vy.push_back( 3 );
  m_vy.push_back( 1 );

  assert( m_vx.size() == m_vy.size() );

  wxSize size = m_pPanel->GetClientSize();
  int m_nPixelsX( size.GetX() );
  int m_nPixelsY( size.GetY() );

  XYChart chart( m_nPixelsX, m_nPixelsY );

  // do an overlay for the leading line, so can get a different color on it
  chart.addTitle( "Trin vs Delta Index" );
  chart.setPlotArea( 40, 10, m_nPixelsX - 50, m_nPixelsY - 35, 0xffffff, -1, 0xc0c0c0, 0xc0c0c0, -1 );
  chart.yAxis()->setLogScale( true );
  chart.addText(  45,  20, "NormDecl" );
  chart.addText(  45, m_nPixelsY / 2 - 15, "WeakDecl" );
  chart.addText(  45, m_nPixelsY - 45, "UpSoon" );
  chart.addText( m_nPixelsX / 2, m_nPixelsY - 45, "UpSn!!" );
  chart.addText( m_nPixelsX - 70, m_nPixelsY - 45, "NormAdv" );
  chart.addText( m_nPixelsX - 70,  m_nPixelsY / 2 - 15, "WeakAdv" );
  chart.addText( m_nPixelsX - 70,  20, "DnSoon" );
  chart.addText( m_nPixelsX / 2,  20, "DnSn!!" );

  vColours_t::const_iterator iterc( m_vColours.begin() );
  int depth( 0 );

  typedef vDoubles_t::const_reverse_iterator iterDoubles;
  iterDoubles iterx1( m_vx.rbegin() );
  iterDoubles iterx2( iterx1 ); ++iterx2;
  iterDoubles itery1( m_vy.rbegin() );
  iterDoubles itery2( itery1 ); ++itery2;
  while ( m_vx.rend() != iterx2 ) {
    DoubleArray dax( &(*iterx2), 2 );
    DoubleArray day( &(*itery2), 2 );
    LineLayer* pLLIndu = chart.addLineLayer( day, *iterc, "y" );
    pLLIndu->setXData( dax );
    pLLIndu->setLineWidth( 2 );
    pLLIndu->setBorderColor( 0xff000000 );
    //pLLIndu->moveFront();
    ++iterx1; ++iterx2; ++itery1; ++itery2;
    ++iterc;  
    if ( m_vColours.end() == iterc ) --iterc;
  }

  MemBlock m = chart.makeChart( BMP );

  wxMemoryInputStream in( m.data, m.len );
  wxBitmap bmp( wxImage( in, wxBITMAP_TYPE_BMP) );
  wxClientDC cdc( m_pPanel );
  cdc.DrawBitmap(bmp, 0, 0);
}

void AppTestWx::OnClose( wxCloseEvent& event ) {

  // Exit Steps: #2 -> FrameMain::OnClose
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

int AppTestWx::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors
  //m_timerGuiRefresh.Stop();
//  this->m_pData1Provider->Disconnect();

  return wxAppConsole::OnExit();
}

