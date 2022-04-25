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

#include <iostream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <wx/sizer.h>

#include "Hdf5Chart.h"

namespace {
  const static std::string sStateFileName = "Hdf5Chart.state";
}

IMPLEMENT_APP(AppHdf5Chart)

bool AppHdf5Chart::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Hdf5 Chart" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 800, 500 );
  SetTopWindow( m_pFrameMain );

  // Sizer for FrameMain
  wxBoxSizer* sizerMain;
  sizerMain = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(sizerMain);

  // Sizer for Controls
  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxHORIZONTAL );
  sizerMain->Add( m_sizerControls, 0, wxEXPAND|wxLEFT|wxTOP|wxRIGHT, 5 );
  //panelSplitterRight->SetSizer( m_sizerControls );
  //sizerRight->Add( m_sizerControls, 0, wxStretch::wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
  m_pPanelLogging->SetMinSize( wxSize( 100, 100 ) );
//  m_pPanelLogging->Show( true );

  m_pPanelChartHdf5 = new ou::tf::PanelChartHdf5( m_pFrameMain, wxID_ANY );
  sizerMain->Add( m_pPanelChartHdf5, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);

  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pFrameMain->Show( true );

  m_bData1Connected = false;
  m_bData2Connected = false;
  m_bExecConnected = false;

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppHdf5Chart::OnClose, this );  // start close of windows and controls

//  std::string sTimeZoneSpec( "date_time_zonespec.csv" );
//  if ( !boost::filesystem::exists( sTimeZoneSpec ) ) {
//    std::cout << "Required file does not exist:  " << sTimeZoneSpec << std::endl;
//  }

  FrameMain::vpItems_t vItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
//  vItems.push_back( new mi( "test", MakeDelegate( m_pPanelChartHdf5, &ou::tf::PanelChartHdf5::TestChart ) ) );
//  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

//  this->m_pData1Provider->Connect();

  CallAfter(
    [this](){
      LoadState();
    }
  );

  return 1;

}

void AppHdf5Chart::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppHdf5Chart::LoadState() {
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

int AppHdf5Chart::OnExit() {
  std::cout << "AppHdf5Chart::OnExit" << std::endl;
  // Exit Steps: #4

  return wxAppConsole::OnExit();
}

void AppHdf5Chart::OnClose( wxCloseEvent& event ) {
  std::cout << "AppHdf5Chart::OnClose" << std::endl;
  // Exit Steps: #2 -> FrameMain::OnClose
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a

  SaveState();

  event.Skip();  // auto followed by Destroy();
}

void AppHdf5Chart::OnData1Connected( int ) {
  m_bData1Connected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppHdf5Chart::OnData2Connected( int ) {
  m_bData2Connected = true;
  if ( m_bData2Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppHdf5Chart::OnExecConnected( int ) {
  m_bExecConnected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppHdf5Chart::OnData1Disconnected( int ) {
  m_bData1Connected = false;
}

void AppHdf5Chart::OnData2Disconnected( int ) {
  m_bData2Connected = false;
}

void AppHdf5Chart::OnExecDisconnected( int ) {
  m_bExecConnected = false;
}
