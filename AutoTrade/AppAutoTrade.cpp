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

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <wx/sizer.h>

#include <TFTrading/Watch.h>
#include <TFTrading/Position.h>
#include <TFTrading/BuildInstrument.hpp>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/WinChartView.h>

#include "Strategy.hpp"
#include "AppAutoTrade.hpp"

namespace {
  static const std::string c_sAppTitle(        "Auto Trade Example" );
  static const std::string c_sAppNamePrefix(   "AutoTrade" );
  static const std::string c_sChoicesFilename( c_sAppNamePrefix + ".cfg" );
  static const std::string c_sDbName(          c_sAppNamePrefix + ".db" );
  static const std::string c_sStateFileName(   c_sAppNamePrefix + ".state" );
  static const std::string c_sTimeZoneSpec( "../date_time_zonespec.csv" );
}

IMPLEMENT_APP(AppAutoTrade)

bool AppAutoTrade::OnInit() {

  wxApp::SetAppDisplayName( c_sAppTitle );
  wxApp::SetVendorName( "One Unified Net Limited" );
  wxApp::SetVendorDisplayName( "(c)2023 One Unified Net Limited" );

  wxApp::OnInit();

  m_nTSDataStreamSequence = 0;
  m_bConnectedLatch = false;

  if ( Load( c_sChoicesFilename, m_choices ) ) {
    m_sSymbol = m_choices.sSymbol_Trade;
  }
  else {
    return false;
  }

  {
    std::stringstream ss;
    auto dt = ou::TimeSource::GlobalInstance().External();
    ss
      << ou::tf::Instrument::BuildDate( dt.date() )
      << " "
      << dt.time_of_day()
      ;
    m_sTSDataStreamStarted = ss.str();  // will need to make this generic if need some for multiple providers.
  }

  //if ( choices.bSimStart ) {
    // just always delete it
    if ( boost::filesystem::exists( c_sDbName ) ) {
    boost::filesystem::remove( c_sDbName );
    }
  //}

  m_pdb = std::make_unique<ou::tf::db>( c_sDbName );

  if ( m_choices.bSimStart ) {
    if ( 0 < m_choices.sGroupDirectory.size() ) {
      m_sim->SetGroupDirectory( m_choices.sGroupDirectory );
    }
  }

  m_tws->SetClientId( m_choices.nIbInstance );

  m_pFrameMain = new FrameMain( 0, wxID_ANY, c_sAppTitle );
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
  m_pStrategy = std::make_unique<Strategy>( m_ChartDataView, m_choices );

  if ( m_choices.bSimStart ) {
    boost::regex expr{ "(20[2-3][0-9][0-1][0-9][0-3][0-9])" };
    boost::smatch what;
    if ( boost::regex_search( m_choices.sGroupDirectory, what, expr ) ) {
      boost::gregorian::date date( boost::gregorian::from_undelimited_string( what[ 0 ] ) );
      std::cout << "date " << date << std::endl;
      m_pStrategy->InitForUSEquityExchanges( date );
    }
  }

  //m_pFrameMain->SetAutoLayout( true );
  //m_pFrameMain->Layout();
  m_pFrameMain->Show( true );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppAutoTrade::OnClose, this );  // start close of windows and controls

  FrameMain::vpItems_t vItems;
  using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects
  vItems.push_back( new mi( "Close, Done", MakeDelegate( this, &AppAutoTrade::HandleMenuActionCloseAndDone ) ) );
  vItems.push_back( new mi( "Save Values", MakeDelegate( this, &AppAutoTrade::HandleMenuActionSaveValues ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  vItems.clear();
  vItems.push_back( new mi( "Stop", MakeDelegate( this, &AppAutoTrade::HandleMenuActionStopChart ) ) );
  vItems.push_back( new mi( "Start", MakeDelegate( this, &AppAutoTrade::HandleMenuActionStartChart ) ) );
  m_pFrameMain->AddDynamicMenu( "Chart", vItems );

  if ( !boost::filesystem::exists( c_sTimeZoneSpec ) ) {
    std::cout << "Required file does not exist:  " << c_sTimeZoneSpec << std::endl;
  }

  CallAfter(
    [this](){
      LoadState();
    }
  );

  if ( m_choices.bSimStart ) {
    CallAfter(
      [this](){
        using Provider_t = ou::tf::PanelProviderControl::Provider_t;
        m_pPanelProviderControl->SetProvider( Provider_t::ESim, Provider_t::ESim, Provider_t::ESim );
        m_pPanelProviderControl->SetSimulatorState( ou::tf::ProviderOn );
        m_sim->Connect();
      }
    );
  }

  return true;
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
        "/app/" + c_sAppNamePrefix + "/" +
        m_sTSDataStreamStarted + "-" +
        boost::lexical_cast<std::string>( m_nTSDataStreamSequence ) ); // sequence number on each save
      std::cout << "  ... Done " << std::endl;
    }
  );
}

void AppAutoTrade::HandleMenuActionStopChart() {
  m_pWinChartView->SetChartDataView( nullptr );
}

void AppAutoTrade::HandleMenuActionStartChart() {
  m_pWinChartView->SetChartDataView( &m_ChartDataView );
}

void AppAutoTrade::ConstructLiveInstrument() {

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  m_pBuildInstrumentIQFeed = std::make_unique<ou::tf::BuildInstrument>( m_iqfeed );

  switch ( m_pExecutionProvider->ID() ) {
    case ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderIB:
      m_pBuildInstrumentExec = std::make_unique<ou::tf::BuildInstrument>( m_iqfeed, m_tws );
      break;
    case ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderIQF:
      m_pBuildInstrumentExec = std::make_unique<ou::tf::BuildInstrument>( m_iqfeed );
      m_iqfeed->EnableExecution( true );
      break;
    default:
      assert( false );
      break;
  }

  m_pBuildInstrumentExec->Queue(
    m_sSymbol,
    [this]( pInstrument_t pInstrument, bool bConstructed ){
      if ( bConstructed ) {
        ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
        im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
      }

      ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
      const ou::tf::Instrument::idInstrument_t& idInstrument( pInstrument->GetInstrumentName() );

      pPosition_t pPosition;
      if ( pm.PositionExists( "USD", idInstrument ) ) {
        pPosition = pm.GetPosition( "USD", idInstrument );
        std::cout << "position loaded " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
      }
      else {
        pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_iqfeed );
        pPosition = pm.ConstructPosition(
          "USD", idInstrument, "ema",
          m_pData1Provider->GetName(), m_pExecutionProvider->GetName(),
          m_pExecutionProvider,
          pWatch
        );
        std::cout << "position constructed " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
      }
      m_pStrategy->SetPosition( pPosition );

      // these need to come after the SetPostition
      if ( !m_choices.sSymbol_Tick.empty() ) {
        m_pBuildInstrumentIQFeed->Queue(
          m_choices.sSymbol_Tick,
          [this]( pInstrument_t pInstrument, bool bConstructed ){
            pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_iqfeed );
            m_pStrategy->SetTick( pWatch );
          } );
      }
      if ( !m_choices.sSymbol_Trin.empty() ) {
        m_pBuildInstrumentIQFeed->Queue(
          m_choices.sSymbol_Trin,
          [this]( pInstrument_t pInstrument, bool bConstructed ){
            pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_iqfeed );
            m_pStrategy->SetTrin( pWatch );
          } );
      }

    } );
}

void AppAutoTrade::ConstructSimInstrument() {

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  ou::tf::Instrument::pInstrument_t pInstrument = std::make_shared<ou::tf::Instrument>( m_sSymbol );
  const ou::tf::Instrument::idInstrument_t& idInstrument( pInstrument->GetInstrumentName() );
  ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
  im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
  pPosition_t pPosition;
  if ( pm.PositionExists( "USD", idInstrument ) ) {
    pPosition = pm.GetPosition( "USD", idInstrument );
    std::cout << "sim: probably should delete database first" << std::endl;
    std::cout << "sim: position loaded " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
  }
  else {
    pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_pData1Provider );
    pPosition = pm.ConstructPosition(
      "USD", idInstrument, "ema",
      m_pData1Provider->GetName(), m_pExecutionProvider->GetName(),
      m_pExecutionProvider,
      pWatch
    );
    std::cout << "Constructed " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
  }
  m_pStrategy->SetPosition( pPosition );

  if ( !m_choices.sSymbol_Tick.empty() ) {
    ou::tf::Instrument::pInstrument_t pInstrument = std::make_shared<ou::tf::Instrument>( m_choices.sSymbol_Tick, ou::tf::InstrumentType::Index, "DTN" );
    pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_pData1Provider );
    m_pStrategy->SetTick( pWatch );
  }
  if ( !m_choices.sSymbol_Trin.empty() ) {
    ou::tf::Instrument::pInstrument_t pInstrument = std::make_shared<ou::tf::Instrument>( m_choices.sSymbol_Trin, ou::tf::InstrumentType::Index, "DTN" );
    pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_pData1Provider );
    m_pStrategy->SetTrin( pWatch );
  }

  FrameMain::vpItems_t vItems;
  using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects
  vItems.push_back( new mi( "Start", MakeDelegate( this, &AppAutoTrade::HandleMenuActionSimStart ) ) );
  vItems.push_back( new mi( "Stop",  MakeDelegate( this, &AppAutoTrade::HandleMenuActionSimStop ) ) );
  vItems.push_back( new mi( "Stats",  MakeDelegate( this, &AppAutoTrade::HandleMenuActionSimEmitStats ) ) );
  m_pFrameMain->AddDynamicMenu( "Simulation", vItems );

  m_sim->Run();

}

void AppAutoTrade::HandleMenuActionSimStart() {
  CallAfter(
    [this](){
      m_sim->Run();
    }
  );
}

void AppAutoTrade::HandleMenuActionSimStop() {
  CallAfter(
    [this](){
      m_sim->Stop();
    }
  );
}

void AppAutoTrade::HandleMenuActionSimEmitStats() {
  CallAfter(
    [this](){
      std::stringstream ss;
      m_sim->EmitStats( ss );
      std::cout << "Stats: " << ss.str() << std::endl;
    } );
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
  if ( !m_bConnectedLatch ) {
    if ( m_bData1Connected && m_bExecConnected ) {
      bool bValidCombo( false );
      if (
           ( ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderIQF == m_pData1Provider->ID() )
        && ( ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderIB  == m_pExecutionProvider->ID() )
      ) {
        bValidCombo = true;
        ConstructLiveInstrument();
      }
      else {
        if (
             ( ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderSimulator == m_pData1Provider->ID() )
          && ( ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderSimulator == m_pExecutionProvider->ID() )
        ) {
          bValidCombo = true;
          ConstructSimInstrument();
        }
        else {
          if (
               ( ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderIQF == m_pData1Provider->ID() )
            && ( ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderIQF == m_pExecutionProvider->ID() )
          ) {
            bValidCombo = true;
            ConstructLiveInstrument();
          }
        }
      }
      if ( bValidCombo ) {
        m_bConnectedLatch = true;
      }
      else {
        std::cout << "invalid combo of data and execution providers" << std::endl;
      }
    }
  }
}

void AppAutoTrade::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( c_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppAutoTrade::LoadState() {
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
