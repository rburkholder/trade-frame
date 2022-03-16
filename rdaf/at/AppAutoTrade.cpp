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
 * Project: rdaf/at
 * Created: March 7, 2022 14:35
 */

#include <sstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>

#include <TFTrading/Watch.h>
#include <TFTrading/Position.h>
#include <TFTrading/BuildInstrument.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/WinChartView.h>

#include "Strategy.h"
#include "AppAutoTrade.h"

namespace {
  static const std::string sAppName( "ROOT AutoTrade (rdaf_at)" );
  static const std::string sChoicesFilename( "rdaf/at/choices.cfg" );
  static const std::string sDbName( "rdaf/at/example.db" );
  static const std::string sStateFileName( "rdaf/at/example.state" );
  static const std::string sTimeZoneSpec( "../date_time_zonespec.csv" );
}

namespace {

  class CustomItemData: public wxTreeItemData {
  public:
    std::string sSymbol;
    wxMenu* pMenuPopup;
    CustomItemData( wxMenu* pMenuPopup_ )
    : pMenuPopup( pMenuPopup_ )
    {}
    CustomItemData( const std::string& sSymbol_ )
    : sSymbol( sSymbol_ ), pMenuPopup( nullptr )
    {}
    virtual ~CustomItemData() {
      if ( nullptr != pMenuPopup ) {
        // assumes binds are cleared as well
        delete pMenuPopup;
        pMenuPopup = nullptr;
      }
    }
  };
}

IMPLEMENT_APP(AppAutoTrade)

bool AppAutoTrade::OnInit() {

  wxApp::SetAppDisplayName( sAppName );
  wxApp::SetVendorName( "One Unified Net Limited" );
  wxApp::SetVendorDisplayName( "(c)2022 One Unified Net Limited" );

  wxApp::OnInit();

  m_nTSDataStreamSequence = 0;
  {
    std::stringstream ss;
    auto dt = ou::TimeSource::Instance().External();
    ss
      << ou::tf::Instrument::BuildDate( dt.date() )
      << "-"
      << dt.time_of_day()
      ;
    m_sTSDataStreamStarted = ss.str();  // will need to make this generic if need some for multiple providers.
  }

  if ( !ou::tf::config::Load( sChoicesFilename, m_choices ) ) {
    return 0;
  }

  //if ( m_options.bSimStart ) {
    // just always delete it
    if ( boost::filesystem::exists( sDbName ) ) {
    boost::filesystem::remove( sDbName );
    }
  //}

  m_pdb = std::make_unique<ou::tf::db>( sDbName );

  if ( 0 < m_choices.sGroupDirectory.size() ) {
    m_sim->SetGroupDirectory( m_choices.sGroupDirectory );
  }

  m_tws->SetClientId( m_choices.ib_instance );

  m_pFrameMain = new FrameMain( 0, wxID_ANY, sAppName );
  wxWindowID idFrameMain = m_pFrameMain->GetId();

  m_pFrameMain->SetSize( 800, 500 );
  SetTopWindow( m_pFrameMain );

    wxBoxSizer* sizerFrame;
    wxBoxSizer* sizerUpper;
    wxBoxSizer* sizerLower;

    sizerFrame = new wxBoxSizer(wxVERTICAL);
    m_pFrameMain->SetSizer(sizerFrame);

    sizerUpper = new wxBoxSizer(wxHORIZONTAL);
    sizerFrame->Add(sizerUpper, 0, wxGROW, 2);

    m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_pPanelProviderControl->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    sizerUpper->Add(m_pPanelProviderControl, 0, wxALIGN_CENTER_VERTICAL, 2);

    m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_pPanelLogging->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    sizerUpper->Add(m_pPanelLogging, 1, wxGROW, 2);

    sizerLower = new wxBoxSizer(wxVERTICAL);
    sizerFrame->Add(sizerLower, 1, wxGROW, 2);

    m_splitterData = new wxSplitterWindow( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );
    m_splitterData->SetMinimumPaneSize(20);

    m_treeSymbols = new wxTreeCtrl( m_splitterData, wxID_ANY, wxDefaultPosition, wxSize(100, 100), wxTR_SINGLE );

    m_pWinChartView = new ou::tf::WinChartView( m_splitterData, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_pWinChartView->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    m_splitterData->SplitVertically(m_treeSymbols, m_pWinChartView, 50);
    sizerLower->Add(m_splitterData, 1, wxGROW, 2);

  LinkToPanelProviderControl();

  wxTreeItemId idRoot = m_treeSymbols->AddRoot( "/", -1, -1, nullptr );
  //m_treeSymbols->Bind( wxEVT_TREE_ITEM_MENU, &AppAutoTrade::HandleTreeEventItemMenu, this, m_treeSymbols->GetId() );
  //m_treeSymbols->Bind( wxEVT_TREE_ITEM_RIGHT_CLICK, &AppAutoTrade::HandleTreeEventItemRightClick, this, m_treeSymbols->GetId() );
  m_treeSymbols->Bind( wxEVT_TREE_SEL_CHANGED, &AppAutoTrade::HandleTreeEventItemChanged, this, m_treeSymbols->GetId() );

  boost::gregorian::date date;
  if ( m_choices.bStartSimulator ) {
    boost::regex expr{ "(20[2-3][0-9][0-1][0-9][0-3][0-9])" };
    boost::smatch what;
    if ( boost::regex_search( m_choices.sGroupDirectory, what, expr ) ) {
      date = boost::gregorian::from_undelimited_string( what[ 0 ] );
      std::cout << "simulation date " << date << std::endl;

    }
  }

  for ( ou::tf::config::choices_t::mapInstance_t::value_type& vt: m_choices.mapInstance ) {

    auto& [sSymbol, choices] = vt;
    std::cout << "creating strategy for: " << sSymbol << std::endl;

    Strategy::config_t config(
      sSymbol,
      m_choices.nTimeBins, m_choices.dblTimeUpper, m_choices.dblTimeLower,
      choices.nPriceBins, choices.dblPriceUpper, choices.dblPriceLower,
      choices.nVolumeBins, choices.nVolumeUpper, choices.nVolumeLower
      );

    pStrategy_t pStrategy = std::make_unique<Strategy>( "rdaf/at/" + m_sTSDataStreamStarted, config );
    m_pWinChartView->SetChartDataView( &pStrategy->GetChartDataView() );

    if ( m_choices.bStartSimulator ) {
      pStrategy->InitForUSEquityExchanges( date );
    }

    m_mapStrategy.emplace( sSymbol, std::move( pStrategy ) );

    wxTreeItemId idSymbol = m_treeSymbols->AppendItem( idRoot, sSymbol, -1, -1, new CustomItemData( sSymbol ) );

  }

  m_treeSymbols->ExpandAll();

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

  m_pBuildInstrument = std::make_unique<ou::tf::BuildInstrument>( m_iqfeed, m_tws );

  CallAfter(
    [this](){
      LoadState();
    }
  );

  if ( m_choices.bStartSimulator ) {
    CallAfter(
      [this](){
        using Provider_t = ou::tf::PanelProviderControl::Provider_t;
        m_pPanelProviderControl->SetProvider( Provider_t::ESim, Provider_t::ESim, Provider_t::ESim );
        m_pPanelProviderControl->SetSimulatorState( ou::tf::ProviderOn );
        m_sim->Connect();
      }
    );
  }

  return 1;
}

void AppAutoTrade::HandleTreeEventItemMenu( wxTreeEvent& event ) {
  wxTreeItemData* pData = m_treeSymbols->GetItemData( event.GetItem() );
  //if ( nullptr != pData ) {
  //  CustomItemData* pCustom = dynamic_cast<CustomItemData*>( pData );
  //  if ( pCustom->pMenuPopup ) {
  //    m_pFrameMain->PopupMenu( pCustom->pMenuPopup, event.GetPoint() );
  //  }
  //}
  //else {
  //  std::cout << "no item data" << std::endl;
  //}
  event.Skip();
}

void AppAutoTrade::HandleTreeEventItemChanged( wxTreeEvent& event ) {
  wxTreeItemData* pData = m_treeSymbols->GetItemData( event.GetItem() );
  if ( nullptr != pData ) {
    CustomItemData* pCustom = dynamic_cast<CustomItemData*>( pData );
    assert( 0 < pCustom->sSymbol.size() );

    mapStrategy_t::iterator iter = m_mapStrategy.find( pCustom->sSymbol );
    assert( m_mapStrategy.end() != iter );
    m_pWinChartView->SetChartDataView( &iter->second->GetChartDataView() );
  }

  event.Skip();
}

void AppAutoTrade::HandleMenuActionCloseAndDone() {
  std::cout << "Closing & Done" << std::endl;
  for ( mapStrategy_t::value_type& vt: m_mapStrategy ) {
    vt.second->CloseAndDone();
  }
}

void AppAutoTrade::HandleMenuActionSaveValues() {
  std::cout << "Saving collected values ... " << std::endl;
  CallAfter(
    [this](){
      m_nTSDataStreamSequence++;
      for ( mapStrategy_t::value_type& vt: m_mapStrategy ) {
        vt.second->SaveWatch(
          "/app/rdaf/at/" +
          m_sTSDataStreamStarted + "-" +
          boost::lexical_cast<std::string>( m_nTSDataStreamSequence ) ); // sequence number on each save
      }
      std::cout << "  ... Done " << std::endl;
    }
  );
}

void AppAutoTrade::ConstructIBInstrument( const std::string& sSymbol ) {

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  m_pBuildInstrument->Queue(
    sSymbol,
    [this,&sSymbol]( pInstrument_t pInstrument ){
      const ou::tf::Instrument::idInstrument_t& idInstrument( pInstrument->GetInstrumentName() );
      ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
      pPosition_t pPosition;
      if ( pm.PositionExists( "USD", idInstrument ) ) {
        pPosition = pm.GetPosition( "USD", idInstrument );
        std::cout << "position loaded " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
      }
      else {
        pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_iqfeed );
        pPosition = pm.ConstructPosition(
          "USD", idInstrument, "ema",
          "ib01", "iq01", m_pExecutionProvider,
          pWatch
        );
        std::cout << "position constructed " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
      }
      mapStrategy_t::iterator iterStrategy = m_mapStrategy.find( sSymbol );
      assert( m_mapStrategy.end() != iterStrategy );
      iterStrategy->second->SetPosition( pPosition );
    } );

}

void AppAutoTrade::ConstructSimInstrument( const std::string& sSymbol ) {

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  ou::tf::Instrument::pInstrument_t pInstrument = std::make_shared<ou::tf::Instrument>( sSymbol );
  const ou::tf::Instrument::idInstrument_t& idInstrument( pInstrument->GetInstrumentName() );
  ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance().Instance() );
  im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
  pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_pData1Provider );
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
      "sim01", "sim01", m_pExecutionProvider,
      pWatch
    );
    std::cout << "Constructed " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
  }
  mapStrategy_t::iterator iterStrategy = m_mapStrategy.find( sSymbol );
  assert( m_mapStrategy.end() != iterStrategy );
  iterStrategy->second->SetPosition( pPosition );

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
      for ( mapStrategy_t::value_type& vt: m_mapStrategy ) {
        ConstructIBInstrument( vt.first );
      }

    }
    if (
         ( ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderSimulator == m_pData1Provider->ID() )
      && ( ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderSimulator == m_pExecutionProvider->ID() )
    ) {
      bValidCombo = true;
      for ( mapStrategy_t::value_type& vt: m_mapStrategy ) {
        ConstructSimInstrument( vt.first );
      }

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
