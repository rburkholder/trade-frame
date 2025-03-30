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

#include <boost/log/trivial.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <wx/sizer.h>
#include <wx/filedlg.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/PanelChartHdf5.hpp>

#include "Hdf5Chart.hpp"

namespace {
  static const std::string c_sAppName( "Hdf5Chart" );
  static const std::string c_sVendorName( "One Unified Net Limited" );
  static const std::string c_sStateFileName = "Hdf5Chart.state";
  static const std::string c_sChoicesFilename( c_sAppName + ".cfg" );
}

IMPLEMENT_APP(AppHdf5Chart)

bool AppHdf5Chart::OnInit() {

  wxApp::SetVendorName( c_sVendorName );
  wxApp::SetAppDisplayName( c_sAppName );
  wxApp::SetVendorDisplayName( "(c)2024 " + c_sVendorName );

  if ( !wxApp::OnInit() ) {
    return false;
  }

  if ( config::Load( c_sChoicesFilename, m_choices ) ) {
    m_sHdf5FileName = m_choices.m_sHdf5File;
    if ( boost::filesystem::exists( m_sHdf5FileName ) ) {}
    else {
      BOOST_LOG_TRIVIAL(error) << m_sHdf5FileName << " does not exist";
      return false;
    }
  }
  else {
    // choices is default to tradeframe.hdf5
  }

  m_pFrameMain = new FrameMain( nullptr, wxID_ANY, "Hdf5 Chart" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
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

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelLogging, 1, wxALL | wxEXPAND, 0);
  m_pPanelLogging->SetMinSize( wxSize( wxID_ANY, 100 ) ); // minimum height

  m_pPanelChartHdf5 = new ou::tf::PanelChartHdf5( m_sHdf5FileName );
  m_pPanelChartHdf5->Create( m_pFrameMain, wxID_ANY );
  //m_pPanelChartHdf5 = new ou::tf::PanelChartHdf5( m_pFrameMain, wxID_ANY );
  sizerMain->Add( m_pPanelChartHdf5, 1, wxALL | wxEXPAND, 0);

  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_bData1Connected = false;
  m_bData2Connected = false;
  m_bExecConnected = false;

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppHdf5Chart::OnClose, this );  // start close of windows and controls

  m_pFrameMain->Bind( wxEVT_MOVE, &AppHdf5Chart::OnFrameMainAutoMove, this ); // intercept first move
  m_pFrameMain->Show( true ); // triggers the auto move

//  std::string sTimeZoneSpec( "date_time_zonespec.csv" );
//  if ( !boost::filesystem::exists( sTimeZoneSpec ) ) {
//    std::cout << "Required file does not exist:  " << sTimeZoneSpec << std::endl;
//  }

  int ixItem;
  ixItem = m_pFrameMain->AddFileMenuItem( _( "Select HDF5 File" ) );
  m_pFrameMain->Bind( wxEVT_COMMAND_MENU_SELECTED, &AppHdf5Chart::HandleSelectHdf5File, this, ixItem, -1 );

  FrameMain::vpItems_t vItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
//  vItems.push_back( new mi( "test", MakeDelegate( m_pPanelChartHdf5, &ou::tf::PanelChartHdf5::TestChart ) ) );
//  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

//  this->m_pData1Provider->Connect();

  return 1;
}

void AppHdf5Chart::OnFrameMainAutoMove( wxMoveEvent& event ) {
  // load state works properly _after_ first move (library initiated)

    CallAfter(
      [this](){
        const std::string sOldFileName( m_sHdf5FileName );
        LoadState();
        m_pFrameMain->Layout();
        if ( sOldFileName != m_sHdf5FileName ) {
          m_pPanelChartHdf5->SetFileName( m_sHdf5FileName );
        }
      }
    );

    m_pFrameMain->Unbind( wxEVT_MOVE, &AppHdf5Chart::OnFrameMainAutoMove, this );

    event.Skip(); // set to false if we want to ignore auto move

  }

void AppHdf5Chart::HandleSelectHdf5File( wxCommandEvent& event ) {
  CallAfter(
    [this](){
      wxFileDialog
        openFileDialog(
          m_pFrameMain,
          _("Select HDF5 file"),
          "", // default dir
          m_sHdf5FileName, // default file
          "HDF5 files (*.hdf5)|*.hdf5",
          wxFD_OPEN|wxFD_FILE_MUST_EXIST
        );
      if ( wxID_CANCEL == openFileDialog.ShowModal() ) {}
      else {
        m_sHdf5FileName = openFileDialog.GetPath();
        std::cout << "hdf5 file selected " << m_sHdf5FileName << std::endl;
        m_pPanelChartHdf5->SetFileName( m_sHdf5FileName );
      }
    } );
}

void AppHdf5Chart::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( c_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppHdf5Chart::LoadState() {
  try {
    std::cout << "Loading Config ..." << std::endl;
    std::ifstream ifs( c_sStateFileName );
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
