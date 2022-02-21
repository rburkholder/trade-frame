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
 * File:    AppAutoTrade.cpp
 * Author:  raymond@burkholder.net
 * Project: AutoTrade
 * Created: February 14, 2022 10:06
 */

#include <sstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/lexical_cast.hpp>

#include <wx/sizer.h>

#include <TFTrading/Watch.h>
#include <TFTrading/Position.h>
#include <TFTrading/BuildInstrument.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/WinChartView.h>

#include "Config.h"
#include "Strategy.h"
#include "AppAutoTrade.h"

namespace {
  static const std::string sAppName( "Auto Trade Example" );
  static const std::string sConfigFilename( "AutoTrade.cfg" );
  static const std::string sDbName( "AutoTrade.db" );
  static const std::string sStateFileName( "AutoTrade.state" );
  static const std::string sTimeZoneSpec( "../date_time_zonespec.csv" );
}

IMPLEMENT_APP(AppAutoTrade)

bool AppAutoTrade::OnInit() {

  wxApp::SetAppDisplayName( sAppName );
  wxApp::SetVendorName( "One Unified Net Limited" );
  wxApp::SetVendorDisplayName( "(c)2022 One Unified Net Limited" );

  wxApp::OnInit();

  m_nTSDataStreamSequence = 0;

  config::Options options;

  if ( Load( sConfigFilename, options ) ) {
    m_sSymbol = options.sSymbol;
  }
  else {
    return 0;
  }

  {
    std::stringstream ss;
    auto dt = ou::TimeSource::Instance().External();
    ss
      << ou::tf::Instrument::BuildDate( dt.date() )
      << " "
      << dt.time_of_day()
      ;
    m_sTSDataStreamStarted = ss.str();  // will need to make this generic if need some for multiple providers.
  }

  m_pdb = std::make_unique<ou::tf::db>( sDbName );

  if ( 0 < options.sGroupDirectory.size() ) {
    m_sim->SetGroupDirectory( options.sGroupDirectory );
  }

  m_tws->SetClientId( options.nIbInstance );

  m_pFrameMain = new FrameMain( 0, wxID_ANY, sAppName );
  wxWindowID idFrameMain = m_pFrameMain->GetId();

  m_pFrameMain->SetSize( 800, 500 );
  SetTopWindow( m_pFrameMain );

    wxBoxSizer* sizerFrame;
    wxBoxSizer* sizerLeft;
    wxBoxSizer* sizerRight;

    // Sizer for FrameMain
    sizerFrame = new wxBoxSizer(wxHORIZONTAL);
    m_pFrameMain->SetSizer(sizerFrame);

    // Sizer for Controls, Logging
    sizerLeft = new wxBoxSizer(wxVERTICAL);
    sizerFrame->Add(sizerLeft, 0, wxGROW, 2);

    // m_pPanelProviderControl
    m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_pPanelProviderControl->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    sizerLeft->Add(m_pPanelProviderControl, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    // m_pPanelLogging
    m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_pPanelLogging->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    sizerLeft->Add(m_pPanelLogging, 1, wxGROW, 2);

    sizerRight = new wxBoxSizer(wxHORIZONTAL);
    sizerFrame->Add(sizerRight, 1, wxGROW, 2);

    // m_pPanelChart
    m_pWinChartView = new ou::tf::WinChartView( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_pWinChartView->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    sizerRight->Add(m_pWinChartView, 1, wxGROW, 2);

  LinkToPanelProviderControl();

  std::cout << "symbol: " << m_sSymbol << std::endl;

  m_pWinChartView->SetChartDataView( &m_ChartDataView );
  m_pStrategy = std::make_unique<Strategy>( m_ChartDataView, options );

  m_pFrameMain->SetAutoLayout( true );
  m_pFrameMain->Layout();
  m_pFrameMain->Show( true );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppAutoTrade::OnClose, this );  // start close of windows and controls

  FrameMain::vpItems_t vItems;
  using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects
  vItems.push_back( new mi( "Close, Done", MakeDelegate( this, &AppAutoTrade::HandleMenuActionCloseAndDone ) ) );
  vItems.push_back( new mi( "Save Values", MakeDelegate( this, &AppAutoTrade::HandleMenuActionSaveValues ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  if ( !boost::filesystem::exists( sTimeZoneSpec ) ) {
    std::cout << "Required file does not exist:  " << sTimeZoneSpec << std::endl;
  }

  CallAfter(
    [this](){
      LoadState();
    }
  );

  if ( options.bSimStart ) {
    CallAfter(
      [this](){
        using Provider_t = ou::tf::PanelProviderControl::Provider_t;
        m_pPanelProviderControl->SetProvider( Provider_t::ESim, Provider_t::ESim, Provider_t::ESim );
        m_pPanelProviderControl->SetSimulatorState( ou::tf::ProviderOn );
        m_sim->Connect();
        m_sim->Run();
      }
    );
  }

  return 1;
}

void AppAutoTrade::HandleMenuActionCloseAndDone() {
  std::cout << "Closing & Done" << std::endl;
  m_pStrategy->CloseAndDone();
}

void AppAutoTrade::HandleMenuActionSaveValues() {
  std::cout << "Saving collected values ... " << std::endl;
  CallAfter(
    [this](){
      m_nTSDataStreamSequence++;
      m_pStrategy->SaveWatch(
        "/app/AutoTrade/" +
        m_sTSDataStreamStarted + "-" +
        boost::lexical_cast<std::string>( m_nTSDataStreamSequence ) ); // sequence number on each save
      std::cout << "  ... Done " << std::endl;
    }
  );
}

void AppAutoTrade::ConstructIBInstrument() {

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  m_pBuildInstrument = std::make_unique<ou::tf::BuildInstrument>( m_iqfeed, m_tws );
  m_pBuildInstrument->Add(
    m_sSymbol,
    [this]( pInstrument_t pInstrument ){
      const ou::tf::Instrument::idInstrument_t& idInstrument( pInstrument->GetInstrumentName() );
      ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
      pPosition_t pPosition;
      if ( pm.PositionExists( "USD", idInstrument ) ) {
        pPosition = pm.GetPosition( "USD", idInstrument );
        std::cout << "loaded " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
      }
      else {
        pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_iqfeed );
        pPosition = pm.ConstructPosition(
          "USD", idInstrument, "ema",
          "ib01", "iq01", m_pExecutionProvider,
          pWatch
        );
        std::cout << "Constructed " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
      }
      m_pStrategy->SetPosition( pPosition );
    } );

}

void AppAutoTrade::ConstructSimInstrument() {

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  ou::tf::Instrument::pInstrument_t pInstrument;
  pInstrument = std::make_shared<ou::tf::Instrument>( m_sSymbol );
  // TODO: will need to turn off database here?
  pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_pData1Provider );
  pPosition_t pPosition = std::make_shared<ou::tf::Position>( pWatch, m_pExecutionProvider );
  // TODO: check that both providers are the sim providers
  std::cout << "Constructed simulation " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
  m_pStrategy->SetPosition( pPosition );

  FrameMain::vpItems_t vItems;
  using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects
  vItems.push_back( new mi( "Start", MakeDelegate( this, &AppAutoTrade::HandleMenuActionSimStart ) ) );
  vItems.push_back( new mi( "Stop",  MakeDelegate( this, &AppAutoTrade::HandleMenuActionSimStop ) ) );
  vItems.push_back( new mi( "Stats",  MakeDelegate( this, &AppAutoTrade::HandleMenuActionSimEmitStats ) ) );
  m_pFrameMain->AddDynamicMenu( "Simulation", vItems );

}

void AppAutoTrade::HandleMenuActionSimStart() {
  m_sim->Run();
}

void AppAutoTrade::HandleMenuActionSimStop() {
  m_sim->Stop();
}

void AppAutoTrade::HandleMenuActionSimEmitStats() {
  std::stringstream ss;
  m_sim->EmitStats( ss );
  std::cout << "Stats: " << ss.str() << std::endl;
}

int AppAutoTrade::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors

  return wxAppConsole::OnExit();
}

void AppAutoTrade::OnClose( wxCloseEvent& event ) {
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

void AppAutoTrade::OnData1Connected( int ) {
  //m_bData1Connected = true;
  ConfirmProviders();
}

void AppAutoTrade::OnData2Connected( int ) {
  //m_bData2Connected = true;
  // Data2 Connection not used
}

void AppAutoTrade::OnExecConnected( int ) {
  //m_bExecConnected = true;
  ConfirmProviders();
}

void AppAutoTrade::OnData1Disconnected( int ) {
  //m_bData1Connected = false;
}

void AppAutoTrade::OnData2Disconnected( int ) {
  //m_bData2Connected = false;
}

void AppAutoTrade::OnExecDisconnected( int ) {
  //m_bExecConnected = false;
}

void AppAutoTrade::ConfirmProviders() {
  if ( m_bData1Connected && m_bExecConnected ) {
    bool bValidCombo( false );
    if (
         ( ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderIQF == m_pData1Provider->ID() )
      && ( ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderIB  == m_pExecutionProvider->ID() )
    ) {
      bValidCombo = true;
      ConstructIBInstrument();
    }
    if (
         ( ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderSimulator == m_pData1Provider->ID() )
      && ( ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderSimulator == m_pExecutionProvider->ID() )
    ) {
      bValidCombo = true;
      ConstructSimInstrument();
    }
    if ( !bValidCombo ) {
      std::cout << "invalid combo of data and execution providers" << std::endl;
    }
  }
}

void AppAutoTrade::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppAutoTrade::LoadState() {
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
