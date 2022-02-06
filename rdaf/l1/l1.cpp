/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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

// Started 2022/02/06

#include <boost/timer/timer.hpp>

#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/timer.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>

#include "l1.h"

IMPLEMENT_APP(AppRdafL1)

bool AppRdafL1::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "rdaf l1" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 800, 500 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerFrame;
  m_sizerFrame = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(m_sizerFrame);

  wxBoxSizer* m_sizerFrameRow1;
  m_sizerFrameRow1 = new wxBoxSizer( wxHORIZONTAL );
  m_sizerFrame->Add(m_sizerFrameRow1, 0, wxGROW|wxALL, 2 );

  // splitter
  wxSplitterWindow* m_splitterRow1;
  m_splitterRow1 = new wxSplitterWindow( m_pFrameMain );
  m_splitterRow1->SetMinimumPaneSize(10);
  m_splitterRow1->SetSashGravity(0.2);

  // tree for viewed symbols
  m_ptreeChartables = new wxTreeCtrl( m_splitterRow1 );
  wxTreeItemId idRoot = m_ptreeChartables->AddRoot( "/", -1, -1, 0 );

  // panel for right side of splitter
  wxPanel* m_panelSplitterRight;
  m_panelSplitterRight = new wxPanel( m_splitterRow1 );

  // sizer for right side of splitter
  wxBoxSizer* m_sizerSplitterRight;
  m_sizerSplitterRight = new wxBoxSizer( wxHORIZONTAL );
  m_panelSplitterRight->SetSizer( m_sizerSplitterRight );

  // m_pPanelProviderControl
  m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_panelSplitterRight, wxID_ANY );
  m_sizerSplitterRight->Add( m_pPanelProviderControl, 0, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 1);

  // m_pPanelLogging
  m_pPanelLogging = new ou::tf::PanelLogging( m_panelSplitterRight, wxID_ANY );
  m_sizerSplitterRight->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 1);

  // startup splitter
  m_splitterRow1->SplitVertically(m_ptreeChartables, m_panelSplitterRight, 10);
  m_sizerFrameRow1->Add(m_splitterRow1, 1, wxEXPAND|wxALL, 1);

  m_tws->SetClientId( 2 );

  LinkToPanelProviderControl();

//  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
//  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pFrameMain->SetAutoLayout( true );
  m_pFrameMain->Layout();
  m_pFrameMain->Show( true );

  // ** Note:  turn on iqfeed only, symbols not set for IB yet

  m_pWinChartView = new ou::tf::WinChartView( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxSize(160, 90), wxNO_BORDER );
  m_sizerFrame->Add( m_pWinChartView, 1, wxALL|wxEXPAND, 3);

  // should already be initialized in the framework
  //m_bData1Connected = false;
  //m_bData2Connected = false;
  //m_bExecConnected = false;

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppRdafL1::OnClose, this );  // start close of windows and controls

  m_timerGuiRefresh.SetOwner( this );  // generates worker thread for IV calcs
  Bind( wxEVT_TIMER, &AppRdafL1::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );

  FrameMain::vpItems_t vItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  vItems.push_back( new mi( "c1 Start Watch", MakeDelegate( this, &AppRdafL1::HandleMenuActionStartWatch ) ) );
  vItems.push_back( new mi( "c2 Stop Watch", MakeDelegate( this, &AppRdafL1::HandleMenuActionStopWatch ) ) );
  vItems.push_back( new mi( "d1 Start Chart", MakeDelegate( this, &AppRdafL1::HandleMenuActionStartChart ) ) );
  vItems.push_back( new mi( "d2 Stop Chart", MakeDelegate( this, &AppRdafL1::HandleMenuActionStopChart ) ) );
  vItems.push_back( new mi( "e1 Save Values", MakeDelegate( this, &AppRdafL1::HandleMenuActionSaveValues ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  return 1;

}

void AppRdafL1::HandleMenuActionStartChart( void ) {
  //m_pWinChartView->SetChartDataView( m_pStrategy->GetChartDataView() );
}

void AppRdafL1::HandleMenuActionStopChart( void ) {
  m_pWinChartView->SetChartDataView( nullptr );
}

void AppRdafL1::HandleSize( wxSizeEvent& event ) {
  //m_winChartView->DrawChart();
  //StartDrawChart();
}

void AppRdafL1::HandleMouse( wxMouseEvent& event ) {
  event.Skip();
}

void AppRdafL1::HandlePaint( wxPaintEvent& event ) {
//  if ( event.GetId() == m_winChart->GetId() ) {
//    wxPaintDC dc( m_winChart );
//    dc.DrawBitmap( *m_pChartBitmap, 0, 0);
//    m_bInDrawChart = false;
//  }
  //else
  event.Skip();
}

void AppRdafL1::HandleMenuActionStartWatch( void ) {

  //m_pBundle->StartWatch();

  m_timerGuiRefresh.Start( 250 );

}

void AppRdafL1::HandleMenuActionStopWatch( void ) {

  //m_pBundle->StopWatch();

  m_timerGuiRefresh.Stop();

}

void AppRdafL1::HandleMenuActionSaveValues( void ) {
  //m_worker.Run( MakeDelegate( this, &AppHedgedBollinger::HandleSaveValues ) );
}

void AppRdafL1::HandleSaveValues( void ) {
  std::cout << "Saving collected values ... " << std::endl;
  try {
//    std::string sPrefixSession( "/app/AppRdafL1/" + m_sTSDataStreamStarted + "/" + m_pBundle->Name() );
    //std::string sPrefix86400sec( "/bar/86400/AtmIV/" + iter->second.sName.substr( 0, 1 ) + "/" + iter->second.sName );
//    std::string sPrefix86400sec( "/app/AppRdafL1/AtmIV/" + m_pBundle->Name() );
//    m_pBundle->SaveData( sPrefixSession, sPrefix86400sec );
  }
  catch(...) {
    std::cout << " ... issues with saving ... " << std::endl;
  }
  std::cout << "  ... Done " << std::endl;
}

void AppRdafL1::HandleGuiRefresh( wxTimerEvent& event ) {
}

int AppRdafL1::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors
  //m_timerGuiRefresh.Stop();

  return wxAppConsole::OnExit();
}

void AppRdafL1::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose
  m_timerGuiRefresh.Stop();
  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  event.Skip();  // auto followed by Destroy();
}

void AppRdafL1::OnData1Connected( int ) {
  m_bData1Connected = true;
//  AutoStartCollection();
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppRdafL1::OnData2Connected( int ) {
  m_bData2Connected = true;
//  AutoStartCollection();
  if ( m_bData2Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppRdafL1::OnExecConnected( int ) {
  m_bExecConnected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppRdafL1::OnData1Disconnected( int ) {
  m_bData1Connected = false;
}

void AppRdafL1::OnData2Disconnected( int ) {
  m_bData2Connected = false;
}

void AppRdafL1::OnExecDisconnected( int ) {
  m_bExecConnected = false;
}
