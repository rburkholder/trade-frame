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

/*
 * File:    l1.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l1
 * Created: February 6, 2022 12:40
 */

#include <boost/timer/timer.hpp>
#include <boost/filesystem.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>

#include <rdaf/TF1.h>
#include <rdaf/TRint.h>
#include <rdaf/TCanvas.h>

#include "Config.h"
#include "l1.h"

namespace {
  static const std::string sConfigFilename( "rdaf_l1.cfg" );
  static const std::string sStateFileName( "rdaf_l1.state" );
  static const std::string sTimeZoneSpec( "../date_time_zonespec.csv" );
}

IMPLEMENT_APP(AppRdafL1)

bool AppRdafL1::OnInit() {

  m_pChartData = nullptr;

  wxApp::SetAppDisplayName( "rdaf l1" );
  wxApp::SetVendorName( "One Unified Net Limited" );
  wxApp::SetVendorDisplayName( "(c)2022 One Unified Net Limited" );

  wxApp::OnInit();

  config::Options options;

  if ( Load( sConfigFilename, options ) ) {
    m_sSymbol = options.sSymbol;
  }

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "rdaf l1" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 800, 500 );
  SetTopWindow( m_pFrameMain );

  // Sizer for FrameMain
  m_sizerFrame = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer( m_sizerFrame );

  // splitter
  m_splitterRow = new wxSplitterWindow( m_pFrameMain );
  m_splitterRow->SetMinimumPaneSize(10);
  m_splitterRow->SetSashGravity(0.2);

  // tree for viewed symbols
  m_ptreeChartables = new wxTreeCtrl( m_splitterRow );
  wxTreeItemId idRoot = m_ptreeChartables->AddRoot( "/", -1, -1, 0 );

  // panel for right side of splitter
  wxPanel* panelSplitterRight;
  panelSplitterRight = new wxPanel( m_splitterRow );

  // sizer for right side of splitter
  wxBoxSizer* sizerSplitterRight;
  sizerSplitterRight = new wxBoxSizer( wxVERTICAL );
  panelSplitterRight->SetSizer( sizerSplitterRight );

  // Sizer for Controls
  wxBoxSizer* sizerControls;
  sizerControls = new wxBoxSizer( wxHORIZONTAL );
  //m_sizerMain->Add( m_sizerControls, 0, wxEXPAND|wxLEFT|wxTOP|wxRIGHT, 5 );
  //panelSplitterRight->SetSizer( m_sizerControls );
  sizerSplitterRight->Add( sizerControls, 0, wxStretch::wxEXPAND|wxALL, 5 );

  // m_pPanelProviderControl
  m_pPanelProviderControl = new ou::tf::PanelProviderControl( panelSplitterRight, wxID_ANY );
  sizerControls->Add( m_pPanelProviderControl, 0, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 1);

  // m_pPanelLogging
  m_pPanelLogging = new ou::tf::PanelLogging( panelSplitterRight, wxID_ANY );
  sizerControls->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 1);

  // startup splitter
  m_splitterRow->SplitVertically( m_ptreeChartables, panelSplitterRight, 10);
  m_sizerFrame->Add( m_splitterRow, 1, wxEXPAND|wxALL, 1);

  LinkToPanelProviderControl();

//  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
//  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  //m_pWinChartView = new ou::tf::WinChartView( panelSplitterRightPanel, wxID_ANY );
  //sizerRight->Add( m_pWinChartView, 1, wxALL|wxEXPAND, 5);
  m_pWinChartView = new ou::tf::WinChartView( panelSplitterRight, wxID_ANY, wxDefaultPosition, wxSize(160, 90), wxNO_BORDER );
  sizerSplitterRight->Add( m_pWinChartView, 1, wxALL|wxEXPAND, 3);

  m_pFrameMain->SetAutoLayout( true );
  m_pFrameMain->Layout();
  m_pFrameMain->Show( true );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppRdafL1::OnClose, this );  // start close of windows and controls

  FrameMain::vpItems_t vItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  //vItems.push_back( new mi( "c1 Start Watch", MakeDelegate( this, &AppRdafL1::HandleMenuActionStartWatch ) ) );
  //vItems.push_back( new mi( "c2 Stop Watch", MakeDelegate( this, &AppRdafL1::HandleMenuActionStopWatch ) ) );
  //vItems.push_back( new mi( "d1 Start Chart", MakeDelegate( this, &AppRdafL1::HandleMenuActionStartChart ) ) );
  //vItems.push_back( new mi( "d2 Stop Chart", MakeDelegate( this, &AppRdafL1::HandleMenuActionStopChart ) ) );
  vItems.push_back( new mi( "e1 Save Values", MakeDelegate( this, &AppRdafL1::HandleMenuActionSaveValues ) ) );
  //m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  if ( !boost::filesystem::exists( sTimeZoneSpec ) ) {
    std::cout << "Required file does not exist:  " << sTimeZoneSpec << std::endl;
  }

  m_threadRdaf = std::move( std::thread(
    [this](){
      TRint app("app", &argc, argv);
      TCanvas* c = new TCanvas("c", "Something", 0, 0, 800, 600);
      TF1 *f1 = new TF1("f1","sin(x)", -5, 5);
      f1->SetLineColor(kBlue+1);
      f1->SetTitle("My graph;x; sin(x)");
      f1->Draw();
      c->Modified(); c->Update();
      app.Run();
    } ) );

  CallAfter(
    [this](){
      LoadState();
      m_splitterRow->Layout(); // the sash does not appear to update
      m_pFrameMain->Layout();
      m_sizerFrame->Layout();
    }
  );

  return 1;
}

void AppRdafL1::HandleMenuActionStartChart( void ) {
  //m_pWinChartView->SetChartDataView( m_pStrategy->GetChartDataView() );
}

void AppRdafL1::HandleMenuActionStopChart( void ) {
  //m_pWinChartView->SetChartDataView( nullptr );
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
}

void AppRdafL1::HandleMenuActionStopWatch( void ) {
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

int AppRdafL1::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors

  return wxAppConsole::OnExit();
}

void AppRdafL1::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose
  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  SaveState();
  event.Skip();  // auto followed by Destroy();
}

void AppRdafL1::OnData1Connected( int ) {
  m_bData1Connected = true;
//  AutoStartCollection();
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }

  // ** Note:  turn on iqfeed only, symbols not set for IB yet
  // TODO: need to fix this with start / stop

  if ( nullptr == m_pChartData ) {
    m_pChartData = new ChartData( m_pData1Provider, m_sSymbol ); // TODO: pass in instrument or watch for re-use elsewhere
    m_pWinChartView->SetChartDataView( m_pChartData->GetChartDataView(), true );
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

void AppRdafL1::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppRdafL1::LoadState() {
  try {
    std::cout << "Loading Config ..." << std::endl;
    std::ifstream ifs( sStateFileName );
    boost::archive::text_iarchive ia(ifs);
    ia & *this;
    std::cout << "  done." << std::endl;
  }
  catch(...) {
    std::cout << "load exception" << std::endl;
  }
}
