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
 * File:   AppESBracketOrder.cpp
 * Author: raymond@burkholder.net
 * Project: ESBracketOrder
 * Created on December 28, 2019, 21:44 PM
 */

#include <fstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <TFTrading/Instrument.h>

#include "AppESBracketOrder.h"

IMPLEMENT_APP(AppESBracketOrder)

bool AppESBracketOrder::OnInit() {

  wxApp::OnInit();
  wxApp::SetAppDisplayName( "ES Bracket Trader" );
  wxApp::SetVendorName( "One Unified Net Limited" );
  wxApp::SetVendorDisplayName( "(c) 2019 One Unified Net Limited" );

  m_sStateFileName = "ESBracketTrader.state";

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "ES Bracket Trader Trader" );
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

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppESBracketOrder::OnClose, this );  // start close of windows and controls
  m_pFrameMain->Show( true );

//  bool bOk( true );

  m_bIBConnected = false;
  m_pIB = boost::make_shared<ou::tf::IBTWS>();
  m_pIB->OnConnecting.Add( MakeDelegate( this, &AppESBracketOrder::HandleIBConnecting ) );
  m_pIB->OnConnected.Add( MakeDelegate( this, &AppESBracketOrder::HandleIBConnected ) );
  m_pIB->OnDisconnecting.Add( MakeDelegate( this, &AppESBracketOrder::HandleIBDisconnecting ) );
  m_pIB->OnDisconnected.Add( MakeDelegate( this, &AppESBracketOrder::HandleIBDisconnected ) );
  m_pIB->OnError.Add( MakeDelegate( this, &AppESBracketOrder::HandleIBError ) );
  m_pIB->Connect();

  CallAfter(
    [this](){
      LoadState();
    }
  );

  return true;

}

void AppESBracketOrder::HandleIBConnecting( int ) {
  std::cout << "Interactive Brokers connecting ..." << std::endl;
}

void AppESBracketOrder::HandleIBConnected( int ) {
  m_bIBConnected = true;
  std::cout << "Interactive Brokers connected." << std::endl;

  std::string sBaseName( "ES" );
  ou::tf::Instrument::pInstrument_t pInstrument = boost::make_shared<ou::tf::Instrument>( sBaseName, ou::tf::InstrumentType::Future, "CME", 2020, 3, 20 );

  m_pIB->RequestContractDetails(
    sBaseName, pInstrument,
    [this]( const ou::tf::IBTWS::ContractDetails& details, pInstrument_t& pInstrument ){
      std::cout << details.marketName << "," << details.summary.conId << std::endl;
      }
    ,
    [](){}
    );

}

void AppESBracketOrder::HandleIBDisconnecting( int ) {
  std::cout << "Interactive Brokers disconnecting ..." << std::endl;
}

void AppESBracketOrder::HandleIBDisconnected( int ) {
  m_bIBConnected = false;
  std::cout << "Interactive Brokers disconnected." << std::endl;
}

void AppESBracketOrder::HandleIBError( size_t e ) {
  std::cout << "HandleIBError: " << e << std::endl;
}

void AppESBracketOrder::SaveState( bool bSilent ) {
  if ( !bSilent ) std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( m_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  if ( !bSilent ) std::cout << "  done." << std::endl;
}

void AppESBracketOrder::LoadState() {
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

void AppESBracketOrder::OnClose( wxCloseEvent& event ) { // step 1

  SaveState();

  event.Skip();  // auto followed by Destroy();
}

int AppESBracketOrder::OnExit() { // step 2

  return wxApp::OnExit();
}
