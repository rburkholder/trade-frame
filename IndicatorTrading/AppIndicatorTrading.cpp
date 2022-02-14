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
 * File:    AppIndicatorTrading.cpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: February 8, 2022 00:12
 */

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>

#include <TFTrading/Watch.h>
#include <TFTrading/Position.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/WinChartView.h>
#include <TFVuTrading/FrameControls.h>
#include <TFVuTrading/PanelOrderButtons.h>

#include "Config.h"
#include "InteractiveChart.h"
#include "AppIndicatorTrading.h"

namespace {
  static const std::string sAppName( "Indicator Trading" );
  static const std::string sConfigFilename( "IndicatorTrading.cfg" );
  static const std::string sStateFileName( "IndicatorTrading.state" );
  static const std::string sTimeZoneSpec( "../date_time_zonespec.csv" );
}

IMPLEMENT_APP(AppIndicatorTrading)

bool AppIndicatorTrading::OnInit() {

  //m_pChartData = nullptr;

  wxApp::SetAppDisplayName( sAppName );
  wxApp::SetVendorName( "One Unified Net Limited" );
  wxApp::SetVendorDisplayName( "(c)2022 One Unified Net Limited" );

  wxApp::OnInit();

  config::Options options;

  if ( Load( sConfigFilename, options ) ) {
    m_sSymbol = options.sSymbol;
  }
  else {
    return 0;
  }

  m_pFrameMain = new FrameMain( 0, wxID_ANY, sAppName );
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
  m_ptreeTradables = new wxTreeCtrl( m_splitterRow );
  wxTreeItemId idRoot = m_ptreeTradables->AddRoot( "/", -1, -1, 0 );

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
  m_splitterRow->SplitVertically( m_ptreeTradables, panelSplitterRight, 10);
  m_sizerFrame->Add( m_splitterRow, 1, wxEXPAND|wxALL, 1);

  LinkToPanelProviderControl();

//  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
//  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  //m_pWinChartView = new ou::tf::WinChartView( panelSplitterRightPanel, wxID_ANY );
  //sizerRight->Add( m_pWinChartView, 1, wxALL|wxEXPAND, 5);
  //m_pWinChartView = new ou::tf::WinChartView( panelSplitterRight, wxID_ANY, wxDefaultPosition, wxSize(160, 90), wxNO_BORDER );
  //sizerSplitterRight->Add( m_pWinChartView, 1, wxALL|wxEXPAND, 3);

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  ou::tf::Instrument::pInstrument_t pInstrument;
  pInstrument = std::make_shared<ou::tf::Instrument>( options.sSymbol ); // simple for an iqfeed watch
  pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, options.sSymbol );
  pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, this->m_pData1Provider ); // will need to be iqfeed provider, check?
  pPosition_t pPosition = std::make_shared<ou::tf::Position>( pWatch, m_pExecutionProvider );

  m_pInteractiveChart = new InteractiveChart( panelSplitterRight, wxID_ANY );
  m_pInteractiveChart->SetPosition( pPosition );

  sizerSplitterRight->Add( m_pInteractiveChart, 1, wxEXPAND | wxALL, 2 );

  m_pFrameMain->SetAutoLayout( true );
  m_pFrameMain->Layout();
  m_pFrameMain->Show( true );

  m_pFrameControls = new ou::tf::FrameControls(  m_pFrameMain, wxID_ANY, "Controls", wxPoint( 10, 10 ) );
  m_pPanelOrderButtons = new ou::tf::PanelOrderButtons( m_pFrameControls );
  m_pFrameControls->Attach( m_pPanelOrderButtons );

  m_pFrameControls->SetAutoLayout( true );
  m_pFrameControls->Layout();
  m_pFrameControls->Show( true );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppIndicatorTrading::OnClose, this );  // start close of windows and controls

  FrameMain::vpItems_t vItems;
  using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects
  //vItems.push_back( new mi( "c1 Start Watch", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionStartWatch ) ) );
  //vItems.push_back( new mi( "c2 Stop Watch", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionStopWatch ) ) );
  //vItems.push_back( new mi( "d1 Start Chart", MakeDelegate( this, &AppRdafL1::HandleMenuActionStartChart ) ) );
  //vItems.push_back( new mi( "d2 Stop Chart", MakeDelegate( this, &AppRdafL1::HandleMenuActionStopChart ) ) );
  //vItems.push_back( new mi( "e1 Save Values", MakeDelegate( this, &AppRdafL1::HandleMenuActionSaveValues ) ) );
  //m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  if ( !boost::filesystem::exists( sTimeZoneSpec ) ) {
    std::cout << "Required file does not exist:  " << sTimeZoneSpec << std::endl;
  }

  //if ( nullptr == m_pChartData ) {
  //  m_pChartData = new ChartData( m_pData1Provider, m_sSymbol, options );
  //  m_pWinChartView->SetChartDataView( m_pChartData->GetChartDataView(), true );
  //}

  CallAfter(
    [this](){
      LoadState();
      m_splitterRow->Layout(); // the sash does not appear to update
      m_pFrameMain->Layout();
      m_sizerFrame->Layout();
    }
  );

  std::cout << "symbol: " << m_sSymbol << std::endl;

  return 1;
}

void AppIndicatorTrading::HandleMenuActionStartChart( void ) {
}

void AppIndicatorTrading::HandleMenuActionStopChart( void ) {
  //m_pWinChartView->SetChartDataView( nullptr );
}

void AppIndicatorTrading::HandleSize( wxSizeEvent& event ) {
  //m_winChartView->DrawChart();
  //StartDrawChart();
}

void AppIndicatorTrading::HandleMouse( wxMouseEvent& event ) {
  event.Skip();
}

void AppIndicatorTrading::HandlePaint( wxPaintEvent& event ) {
//  if ( event.GetId() == m_winChart->GetId() ) {
//    wxPaintDC dc( m_winChart );
//    dc.DrawBitmap( *m_pChartBitmap, 0, 0);
//    m_bInDrawChart = false;
//  }
  //else
  event.Skip();
}

void AppIndicatorTrading::HandleMenuActionStartWatch( void ) {
  //m_pChartData->StartWatch();
}

void AppIndicatorTrading::HandleMenuActionStopWatch( void ) {
  //m_pChartData->StopWatch();
}

void AppIndicatorTrading::HandleMenuActionSaveValues( void ) {
  //m_worker.Run( MakeDelegate( this, &AppHedgedBollinger::HandleSaveValues ) );
}

void AppIndicatorTrading::HandleSaveValues( void ) {
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

int AppIndicatorTrading::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors

  return wxAppConsole::OnExit();
}

void AppIndicatorTrading::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose

  //m_pWinChartView->SetChartDataView( nullptr, false );
  //delete m_pChartData;
  //m_pChartData = nullptr;

  //m_pFrameControls->Close();

  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  SaveState();
  event.Skip();  // auto followed by Destroy();
}

void AppIndicatorTrading::OnData1Connected( int ) {
  m_bData1Connected = true;
//  AutoStartCollection();
  if ( nullptr != m_pInteractiveChart ) {
    m_pInteractiveChart->Connect();
  }
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }

  // ** Note:  turn on iqfeed only, symbols not set for IB yet
  // TODO: need to fix this with start / stop

}

void AppIndicatorTrading::OnData2Connected( int ) {
  m_bData2Connected = true;
//  AutoStartCollection();
  if ( m_bData2Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppIndicatorTrading::OnExecConnected( int ) {
  m_bExecConnected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppIndicatorTrading::OnData1Disconnected( int ) {
  m_bData1Connected = false;
  if ( nullptr != m_pInteractiveChart ) {
    m_pInteractiveChart->Disconnect();
  }
}

void AppIndicatorTrading::OnData2Disconnected( int ) {
  m_bData2Connected = false;
}

void AppIndicatorTrading::OnExecDisconnected( int ) {
  m_bExecConnected = false;
}

void AppIndicatorTrading::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppIndicatorTrading::LoadState() {
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
