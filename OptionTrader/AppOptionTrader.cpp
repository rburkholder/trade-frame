/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    AppOptionTrader.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 17, 2025 08:41:21
 */

#include <boost/log/trivial.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/sizer.h>

#include <TFTrading/AcquireFundamentals.h>

#include <TFVuTrading/FrameMain.h>

#include "AppOptionTrader.hpp"
#include "BookOfOptionChains.hpp"

namespace {
  static const std::string c_sAppTitle(        "Option Trader" );
  static const std::string c_sAppNamePrefix(   "optiontrader" );
  static const std::string c_sChoicesFilename( c_sAppNamePrefix + ".cfg" );
  static const std::string c_sDbName(          c_sAppNamePrefix + ".db" );
  static const std::string c_sStateFileName(   c_sAppNamePrefix + ".state" );
  static const std::string c_sVendorName( "One Unified Net Limited" );
}

IMPLEMENT_APP(AppOptionTrader)

bool AppOptionTrader::OnInit() {

  wxApp::SetAppDisplayName(    c_sAppTitle );
  wxApp::SetVendorName(        c_sVendorName );
  wxApp::SetVendorDisplayName( "(c)2025 " + c_sVendorName );

  if ( !wxApp::OnInit() ) {
    return false;
  }

  m_pFrameMain = new FrameMain( 0, wxID_ANY, c_sAppTitle );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  m_pFrameMain->SetSize( 800, 500 );
  SetTopWindow( m_pFrameMain );

  // Sizer for FrameMain
  wxBoxSizer* sizerFrame;
  sizerFrame = new wxBoxSizer( wxHORIZONTAL );
  m_pFrameMain->SetSizer( sizerFrame );

  m_pBookOfOptionChains = new ou::tf::BookOfOptionChains( m_pFrameMain );
  sizerFrame->Add( m_pBookOfOptionChains, 1, wxALL | wxEXPAND, 0 );
  //m_pBookOfOptionChains->Show();

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppOptionTrader::OnClose, this );  // start close of windows and controls

  //FrameMain::vpItems_t vItems;
  //typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  //vItems.push_back( new mi( "Add Symbol", MakeDelegate( this, &AppOptionTrader::HandleMenuActionAddSymbol ) ) );
  //m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  LoadState();

  m_pFrameMain->Layout();
  m_pFrameMain->Show(); // triggers the auto move

  m_piqfeed = ou::tf::iqfeed::Provider::Factory();
  m_piqfeed->OnConnected.Add( MakeDelegate( this, &AppOptionTrader::HandleIQFeedConnected ) );
  m_piqfeed->Connect();

  return true;

}

void AppOptionTrader::HandleIQFeedConnected( int ) {
}

//void AppOptionTrader::HandleMenuActionAddSymbol() {
//}

void AppOptionTrader::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( c_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppOptionTrader::LoadState() {
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

int AppOptionTrader::OnExit() {
  // Exit Steps: #4
  return wxAppConsole::OnExit();
}

void AppOptionTrader::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose

  //m_pwcv->SetChartDataView( nullptr, false );

  m_pAcquireFundamentals_burial.reset();

  m_piqfeed->Disconnect();
  m_piqfeed.reset();

  m_pFrameMain->Unbind( wxEVT_CLOSE_WINDOW, &AppOptionTrader::OnClose, this );

  SaveState();

  event.Skip();  // auto followed by Destroy();
}
