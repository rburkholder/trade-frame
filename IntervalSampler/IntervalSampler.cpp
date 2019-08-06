/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
 * File:    main.cpp
 * Author:  raymond@burkholder.net
 * Project: IntervalSampler
 * Created on August 6, 2019, 11:08 AM
 */

// An IQFeed based project to collect data from symbols at an interval

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "IntervalSampler.h"

IMPLEMENT_APP(AppIntervalSampler)

bool AppIntervalSampler::OnInit() {

  wxApp::OnInit();
  wxApp::SetAppDisplayName( "Interval Sampler" );
  wxApp::SetVendorName( "One Unified Net Limited" );
  wxApp::SetVendorDisplayName( "(c) 2019 One Unified Net Limited" );

  m_sStateFileName = "IntervalSampler.state";

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Interval Sampler" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind(
  //  wxEVT_SIZE,
  //  [this](wxSizeEvent& event){
  //    std::cout << "w=" << event.GetSize().GetWidth() << ",h=" << event.GetSize().GetHeight() << std::endl;
  //    event.Skip();
  //    },
  //  idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
  //m_pFrameMain->Move( 200, 100 );
  //m_pFrameMain->SetSize( 1400, 800 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* sizerMain = new wxBoxSizer( wxVERTICAL );
  m_pFrameMain->SetSizer( sizerMain );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  sizerMain->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppIntervalSampler::OnClose, this );  // start close of windows and controls
  m_pFrameMain->Show( true );

  m_pIQFeed = boost::make_shared<ou::tf::IQFeedProvider>();
  m_bIQFeedConnected = false;

  m_pIQFeed->OnConnecting.Add( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedConnecting ) );
  m_pIQFeed->OnConnected.Add( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedConnected ) );
  m_pIQFeed->OnDisconnecting.Add( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedDisconnecting ) );
  m_pIQFeed->OnDisconnected.Add( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedDisconnected ) );
  m_pIQFeed->OnError.Add( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedError ) );

  CallAfter(
    [this](){
      LoadState();
    }
  );

  m_pIQFeed->Connect();

  return true;
}

int AppIntervalSampler::OnExit() {

  m_pIQFeed->Disconnect();

  m_pIQFeed->OnConnecting.Remove( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedConnecting ) );
  m_pIQFeed->OnConnected.Remove( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedConnected ) );
  m_pIQFeed->OnDisconnecting.Remove( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedDisconnecting ) );
  m_pIQFeed->OnDisconnected.Remove( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedDisconnected ) );
  m_pIQFeed->OnError.Remove( MakeDelegate( this, &AppIntervalSampler::HandleIQFeedError ) );
  return wxApp::OnExit();
}

void AppIntervalSampler::HandleIQFeedConnecting( int e ) {  // cross thread event
  std::cout << "IQFeed connecting ..." << std::endl;
}

void AppIntervalSampler::HandleIQFeedConnected( int e ) {  // cross thread event
  m_bIQFeedConnected = true;
  std::cout << "IQFeed connected." << std::endl;
}

void AppIntervalSampler::HandleIQFeedDisconnecting( int e ) {  // cross thread event
  std::cout << "IQFeed disconnecting ..." << std::endl;
}

void AppIntervalSampler::HandleIQFeedDisconnected( int e ) { // cross thread event
  m_bIQFeedConnected = false;
  std::cout << "IQFeed disconnected." << std::endl;
}

void AppIntervalSampler::HandleIQFeedError( size_t e ) {
  std::cout << "HandleIQFeedError: " << e << std::endl;
}

void AppIntervalSampler::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( m_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppIntervalSampler::LoadState() {
  try {
    std::cout << "Loading Config ..." << std::endl;
    std::ifstream ifs( m_sStateFileName );
    boost::archive::text_iarchive ia(ifs);
    ia & *this;
    std::cout << "  done." << std::endl;
  }
  catch(...) {
    std::cout << "load exception" << std::endl;
  }
}

void AppIntervalSampler::OnClose( wxCloseEvent& event ) {

  SaveState();

  event.Skip();  // auto followed by Destroy();
}

