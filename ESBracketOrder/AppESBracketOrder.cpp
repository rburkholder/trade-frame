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

#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/splitter.h>

#include <TFTrading/Instrument.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/FrameOrderEntry.h>

#include "AppESBracketOrder.h"

IMPLEMENT_APP(AppESBracketOrder)

bool AppESBracketOrder::OnInit() {

  m_bInitialized = false;

  wxApp::OnInit();
  wxApp::SetAppDisplayName( "ES Bracket Trader" );
  wxApp::SetVendorName( "One Unified Net Limited" );
  wxApp::SetVendorDisplayName( "(c) 2019 One Unified Net Limited" );

  m_sStateFileName = "ESBracketTrader.state";

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "ES Bracket Trader" );
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

  m_splitLogGraph = new wxSplitterWindow( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxSize(100, 100), wxSP_LIVE_UPDATE );
  m_splitLogGraph->SetMinimumPaneSize(20);

  m_pPanelLogging = new ou::tf::PanelLogging( m_splitLogGraph, wxID_ANY );
  m_pPanelLogging->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

  m_pWinChartView = new ou::tf::WinChartView( m_splitLogGraph, wxID_ANY );
  m_pWinChartView->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

  m_splitLogGraph->SplitVertically(m_pPanelLogging, m_pWinChartView, m_splitLogGraph->GetSize().GetWidth() / 4 );

  //sizerMain->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
  //m_pPanelLogging->Show( true );

  sizerMain->Add( m_splitLogGraph, 1, wxGROW|wxALL, 2 );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppESBracketOrder::OnClose, this );  // start close of windows and controls
  m_pFrameMain->Show( true );

//  m_pFrameButtons = new wxFrame( m_pFrameMain, -1, "Button Bar", wxDefaultPosition, wxDefaultSize, wxSTAY_ON_TOP );
//  wxBoxSizer* sizerButtons = new wxBoxSizer( wxVERTICAL );
//  m_pFrameButtons->SetSizer( sizerButtons );

  m_pFrameOrderEntry = new FrameOrderEntry(  m_pFrameMain, wxID_ANY, "Order Entry", wxDefaultPosition, wxDefaultSize, wxCAPTION | wxSTAY_ON_TOP  );
  m_pFrameOrderEntry->Show( true );

  FrameMain::vpItems_t vItemsActions;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  vItemsActions.push_back( new mi( "Emit Bar Summary", MakeDelegate( this, &AppESBracketOrder::HandleMenuActionEmitBarSummary ) ) );
  wxMenu* pMenuActions = m_pFrameMain->AddDynamicMenu( "Actions", vItemsActions );

//  bool bOk( true );
  m_timerGuiRefresh.SetOwner( this );  // generates worker thread for IV calcs
  Bind( wxEVT_TIMER, &AppESBracketOrder::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );

  m_bIBConnected = false;
  m_pIB = boost::make_shared<ou::tf::ib::TWS>();
  m_pIB->SetClientId( 6 );
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

void AppESBracketOrder::HandleMenuActionEmitBarSummary() {
  m_pStrategy->EmitBarSummary();
}

void AppESBracketOrder::HandleQuote( const ou::tf::Quote& quote ) {
}

void AppESBracketOrder::HandleTrade( const ou::tf::Trade& trade ) {
}

void AppESBracketOrder::HandleGuiRefresh( wxTimerEvent& event ) {
}

void AppESBracketOrder::StartWatch() {
//  m_pWatch->OnQuote.Add( MakeDelegate( this, &AppESBracketOrder::HandleQuote ) );
//  m_pWatch->OnTrade.Add( MakeDelegate( this, &AppESBracketOrder::HandleTrade ) );
  m_pWatch->StartWatch();
}

void AppESBracketOrder::StopWatch() {
  m_pWatch->StopWatch();
//  m_pWatch->OnQuote.Remove( MakeDelegate( this, &AppESBracketOrder::HandleQuote ) );
//  m_pWatch->OnTrade.Remove( MakeDelegate( this, &AppESBracketOrder::HandleTrade ) );
}

void AppESBracketOrder::HandleIBConnecting( int ) {
  std::cout << "Interactive Brokers connecting ..." << std::endl;
}

void AppESBracketOrder::HandleIBConnected( int ) {
  m_bIBConnected = true;
  std::cout << "Interactive Brokers connected." << std::endl;

  if ( !m_bInitialized ) {
    static const std::string sBaseName( "ES" );
    ou::tf::Instrument::pInstrument_t pInstrument = std::make_shared<ou::tf::Instrument>( sBaseName, ou::tf::InstrumentType::Future, "GLOBEX", 2020, 3, 20 );
    m_pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_pIB );

    m_pIB->RequestContractDetails(
      sBaseName, pInstrument,
      [this]( const ou::tf::ib::TWS::ContractDetails& details, pInstrument_t& pInstrument ){
        std::cout << details.marketName << "," << details.contract.conId << std::endl;
        m_pStrategy = std::make_unique<Strategy>( m_pWatch );
        namespace ph = std::placeholders;
        m_pFrameOrderEntry->SetButtons(
          std::bind( &Strategy::HandleButtonUpdate, m_pStrategy.get() ),
          std::bind( &Strategy::HandleButtonSend,   m_pStrategy.get(), ph::_1 ),
          std::bind( &Strategy::HandleButtonCancel, m_pStrategy.get() )
          );
        m_pWinChartView->SetChartDataView( m_pStrategy->GetChartDataView() );
        StartWatch(); // need to wait for contract id on first time around
        }
      ,
      []( bool, pInstrument_t& ){}
      );
    m_bInitialized = true;
  }
  else {
    StartWatch();
  }
}

void AppESBracketOrder::HandleIBDisconnecting( int ) {
  std::cout << "Interactive Brokers disconnecting ..." << std::endl;
  if ( m_bInitialized ) {
    StopWatch();
  }
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

  //if ( m_bInitialized ) {
  //  StopWatch();
  //}

  m_pIB->Disconnect();

  SaveState();

  event.Skip();  // auto followed by Destroy();
}

int AppESBracketOrder::OnExit() { // step 2

  return wxApp::OnExit();
}
