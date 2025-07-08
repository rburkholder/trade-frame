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

#include <boost/date_time/posix_time/posix_time.hpp>

//#include <boost/lexical_cast.hpp>

#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/splitter.h>

//#include <TFTrading/InstrumentManager.h>
//#include <TFTrading/AccountManager.h>
//#include <TFTrading/OrderManager.h>

#include <TFVuTrading/TreeItem.hpp>

#include "LiveChart.hpp"

namespace {
  static const std::string c_sAppTitle(        "LiveChart" );
  static const std::string c_sAppNamePrefix(   "livechart" );
  static const std::string c_sChoicesFilename( c_sAppNamePrefix + ".cfg" );
  //static const std::string c_sDbName(          c_sAppNamePrefix + ".db" );
  static const std::string c_sStateFileName(   c_sAppNamePrefix + ".state" );
  //static const std::string c_sTimeZoneSpec( "../date_time_zonespec.csv" );
  static const std::string c_sVendorName( "One Unified Net Limited" );
}

IMPLEMENT_APP(AppLiveChart)

bool AppLiveChart::OnInit() {

  wxApp::SetAppDisplayName( c_sAppTitle );
  wxApp::SetVendorName( c_sVendorName );
  wxApp::SetVendorDisplayName( "(C)2025 " + c_sVendorName );

  if ( !wxApp::OnInit() ) {
    return false;
  }

  if ( config::Load( c_sChoicesFilename, m_choices ) ) {
    auto size = m_choices.vSymbol.size();
    if ( 0 == size ) {
      std::cout << "symbols required" << std::endl;
      return false;
    }
  }

  m_pFrameMain = new FrameMain( 0, wxID_ANY, c_sAppTitle );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 800, 500 );
  SetTopWindow( m_pFrameMain );

  // Sizer for FrameMain
  wxBoxSizer* sizerFrame;
  sizerFrame = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer( sizerFrame );

  // splitter
  wxSplitterWindow* splitter;
  splitter = new wxSplitterWindow( m_pFrameMain );
  splitter->SetMinimumPaneSize(10);
  splitter->SetSashGravity(0.2);

  // tree
  m_pTreeChart = new wxTreeCtrl( splitter );
  m_ptiRoot = new ou::tf::TreeItem( m_pTreeChart, "charts" );
  ou::tf::TreeItem::Bind( splitter, m_pTreeChart );

  // panel for right side of splitter
  wxPanel* panelSplitterRightPanel;
  panelSplitterRightPanel = new wxPanel( splitter );

  // sizer for right side of splitter
  wxBoxSizer* sizerRight;
  sizerRight = new wxBoxSizer( wxVERTICAL );
  panelSplitterRightPanel->SetSizer( sizerRight );

  // Sizer for Controls
  wxBoxSizer* sizerControls;
  sizerControls = new wxBoxSizer( wxHORIZONTAL );
  //m_sizerMain->Add( m_sizerControls, 0, wxEXPAND|wxLEFT|wxTOP|wxRIGHT, 5 );
  //panelSplitterRight->SetSizer( m_sizerControls );
  sizerRight->Add( sizerControls, 0, wxStretch::wxEXPAND|wxALL, 5 );

  // populate variable in FrameWork01
  m_pPanelProviderControl = new ou::tf::PanelProviderControl( panelSplitterRightPanel, wxID_ANY );
  sizerControls->Add( m_pPanelProviderControl, 0, wxALIGN_LEFT|wxRIGHT, 5);
//  m_pPanelProviderControl->Show( true );

  LinkToPanelProviderControl();

  m_pPanelLogging = new ou::tf::PanelLogging( panelSplitterRightPanel, wxID_ANY );
  sizerControls->Add( m_pPanelLogging, 1, wxALL | wxEXPAND, 0);
//  m_pPanelLogging->Show( true );

  splitter->SplitVertically( m_pTreeChart, panelSplitterRightPanel, 0 );
  sizerFrame->Add( splitter, 1, wxGROW|wxALL, 5 );

  m_pWinChartView = new ou::tf::WinChartView( panelSplitterRightPanel, wxID_ANY );

  sizerRight->Add( m_pWinChartView, 1, wxALL|wxEXPAND, 5);

  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  sizerFrame->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pFrameMain->Show( true );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppLiveChart::OnClose, this );  // start close of windows and controls

  std::string sTimeZoneSpec( "../date_time_zonespec.csv" );
  if ( !boost::filesystem::exists( sTimeZoneSpec ) ) {
    std::cout << "Required file does not exist:  " << sTimeZoneSpec << std::endl;
  }

//  m_db.Open( sDbName );

  FrameMain::vpItems_t vItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
//  vItems.push_back( new mi( "a1 New Symbol List Remote", MakeDelegate( this, &AppLiveChart::HandleMenuAction0ObtainNewIQFeedSymbolListRemote ) ) );
//  vItems.push_back( new mi( "a2 New Symbol List Local", MakeDelegate( this, &AppLiveChart::HandleMenuAction1ObtainNewIQFeedSymbolListLocal ) ) );
//  vItems.push_back( new mi( "a3 Load Symbol List", MakeDelegate( this, &AppLiveChart::HandleMenuAction2LoadIQFeedSymbolList ) ) );
//  vItems.push_back( new mi( "b1 Initialize Symbols", MakeDelegate( this, &AppLiveChart::HandleMenuActionInitializeSymbolSet ) ) );
//  vItems.push_back( new mi( "c1 Start Watch", MakeDelegate( this, &AppLiveChart::HandleMenuActionStartWatch ) ) );
//  vItems.push_back( new mi( "c2 Start Chart", MakeDelegate( this, &AppLiveChart::HandleMenuActionStartChart ) ) );
//  vItems.push_back( new mi( "c3 Stop Watch", MakeDelegate( this, &AppLiveChart::HandleMenuActionStopWatch ) ) );
  vItems.push_back( new mi( "d1 Save Values", MakeDelegate( this, &AppLiveChart::HandleMenuActionSaveValues ) ) );
//  vItems.push_back( new mi( "e1 Libor Yield Curve", MakeDelegate( this, &AppLiveChart::HandleMenuActionEmitYieldCurve ) ) );
//  vItems.push_back( new mi( "e1 Load Tree", MakeDelegate( this, &AppLiveChart::HandleMenuActionLoadTree ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  for ( const config::Choices::vSymbol_t::value_type& vt: m_choices.vSymbol ) {
    mapChart_t::iterator iter = m_mapChart.find( vt );
    if ( m_mapChart.end() != iter ) {
      std::cout << "duplicate symbol: " << vt << std::endl;
    }
    else {
      auto result = m_mapChart.emplace( vt, Chart() );
      assert( result.second );
      iter = result.first;
      Chart& chart( iter->second );
      chart.m_pChartData = new ChartData( m_pData1Provider, vt );
      chart.m_pti = m_ptiRoot->AppendChild(
        vt
      , [this,pcdv = chart.m_pChartData->GetChartDataView()]( ou::tf::TreeItem* pti ){
        m_pWinChartView->SetChartDataView( pcdv );
      });
    }
  }

  m_pData1Provider->Connect();

  CallAfter(
    [this](){
      LoadState();
    }
  );

  return 1;

}

void AppLiveChart::HandleMenuActionSaveValues() {
  m_worker.Run( MakeDelegate( this, &AppLiveChart::HandleSaveValues ) );
}

void AppLiveChart::HandleSaveValues() {
  std::cout << "Saving collected values ... " << std::endl;
  try {
    //std::string sPrefixSession( "/app/LiveChart/" + m_sTSDataStreamStarted + "/" + m_pBundle->Name() );
    //std::string sPrefix86400sec( "/bar/86400/AtmIV/" + iter->second.sName.substr( 0, 1 ) + "/" + iter->second.sName );
    //std::string sPrefix86400sec( "/bar/86400/AtmIV/" + m_pBundle->Name() );
    //m_pBundle->SaveData( sPrefixSession, sPrefix86400sec );
    const std::string sPrefix( "/app/livechart/" + m_sTSDataStreamStarted );
    for ( const mapChart_t::value_type& vt: m_mapChart ) {
      vt.second.m_pChartData->SaveSeries( sPrefix );
    }
  }
  catch(...) {
    std::cout << " ... issues with saving ... " << std::endl;
  }
  std::cout << "  ... Done " << std::endl;
}

int AppLiveChart::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors
  //if ( m_db.IsOpen() ) m_db.Close();

  return wxAppConsole::OnExit();
}

void AppLiveChart::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose
  this->m_pData1Provider->Disconnect();
  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();

  SaveState();

  m_mapChart.clear();

  event.Skip();  // auto followed by Destroy();
}

void AppLiveChart::OnData1Connected( int ) {
  m_bData1Connected = true;
  //ou::tf::libor::SetWatchOn( m_pData1Provider );
//  m_libor.SetWatchOn( m_pData1Provider );
//  AutoStartCollection();
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppLiveChart::OnData2Connected( int ) {
  m_bData2Connected = true;
//  AutoStartCollection();
  if ( m_bData2Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppLiveChart::OnExecConnected( int ) {
  m_bExecConnected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppLiveChart::OnData1Disconnected( int ) {
//  m_libor.SetWatchOff();
  m_bData1Connected = false;
}

void AppLiveChart::OnData2Disconnected( int ) {
  m_bData2Connected = false;
}

void AppLiveChart::OnExecDisconnected( int ) {
  m_bExecConnected = false;
}

void AppLiveChart::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( c_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppLiveChart::LoadState() {
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

//void AppLiveChart::HandleRegisterTables(  ou::db::Session& session ) {
//}

//void AppLiveChart::HandleRegisterRows(  ou::db::Session& session ) {
//}

/*
void AppLiveChart::HandlePopulateDatabase() {
  ou::tf::AccountManager::pAccountAdvisor_t pAccountAdvisor
    = ou::tf::AccountManager::Instance().ConstructAccountAdvisor( "aaRay", "Raymond Burkholder", "One Unified" );

  ou::tf::AccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::AccountManager::Instance().ConstructAccountOwner( "aoRay", "aaRay", "Raymond", "Burkholder" );

  ou::tf::AccountManager::pAccount_t pAccountIB
    = ou::tf::AccountManager::Instance().ConstructAccount( "ib01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::AccountManager::pAccount_t pAccountIQFeed
    = ou::tf::AccountManager::Instance().ConstructAccount( "iq01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

  ou::tf::AccountManager::pAccount_t pAccountSimulator
    = ou::tf::AccountManager::Instance().ConstructAccount( "sim01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderSimulator, "Sim", "acctid", "login", "password" );

  m_pPortfolioMaster
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio(
    "Master", "aoRay", "", ou::tf::Portfolio::Master, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Hedged Bollinger" );

  m_pPortfolioCurrencyUSD
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio(
    "USD", "aoRay", "Master", ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Hedged Bollinger" );
}
*/

