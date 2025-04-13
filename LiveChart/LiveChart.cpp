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
#include <wx/splitter.h>
#include <wx/panel.h>

//#include <TFTrading/InstrumentManager.h>
//#include <TFTrading/AccountManager.h>
//#include <TFTrading/OrderManager.h>

#include "LiveChart.h"

namespace {
  static const std::string sStateFileName = "LiveChart.state";
}

IMPLEMENT_APP(AppLiveChart)

bool AppLiveChart::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "LiveChart" );
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
  //wxTreeCtrl* tree;
  m_pHdf5Root = new wxTreeCtrl( splitter );
  m_eLatestDatumType = CustomItemData::NoDatum;
  wxTreeItemId idRoot = m_pHdf5Root->AddRoot( "/", -1, -1, new CustomItemData( CustomItemData::Root, m_eLatestDatumType ) );
//  m_pHdf5Root->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId() );
  //m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId() );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_MENU, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  tree->AppendItem( idRoot, "second" );
//  tree->AppendItem( idRoot, "third" );

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

  splitter->SplitVertically( m_pHdf5Root, panelSplitterRightPanel, 0 );
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

  m_pChartData = new ChartData( m_pData1Provider );
  m_pData1Provider->Connect();

//  m_pWinChartView->SetOnRefreshData(
//    [this](){
//      ptime now = ou::TimeSource::Instance().External();
//      static boost::posix_time::time_duration::fractional_seconds_type fs( 1 );
//      boost::posix_time::time_duration td( 0, 0, 0, fs - now.time_of_day().fractional_seconds() );
//      ptime dtEnd = now + td;
//      static boost::posix_time::time_duration tdLength( 0, 10, 0 );
//      ptime dtBegin = dtEnd - tdLength;
//      m_pChartData->GetChartDataView()->SetViewPort( dtBegin, dtEnd );
//    } );

  m_pWinChartView->SetChartDataView( m_pChartData->GetChartDataView() );

  CallAfter(
    [this](){
      LoadState();
    }
  );

  return 1;

}

void AppLiveChart::HandleMenuActionSaveValues( void ) {
  m_worker.Run( MakeDelegate( this, &AppLiveChart::HandleSaveValues ) );
}

void AppLiveChart::HandleSaveValues( void ) {
  std::cout << "Saving collected values ... " << std::endl;
  try {
    //std::string sPrefixSession( "/app/LiveChart/" + m_sTSDataStreamStarted + "/" + m_pBundle->Name() );
    //std::string sPrefix86400sec( "/bar/86400/AtmIV/" + iter->second.sName.substr( 0, 1 ) + "/" + iter->second.sName );
    //std::string sPrefix86400sec( "/bar/86400/AtmIV/" + m_pBundle->Name() );
    //m_pBundle->SaveData( sPrefixSession, sPrefix86400sec );
    std::string sPrefixSession( "/app/LiveChart/" + m_sTSDataStreamStarted + "/"
      + m_pChartData->GetWatch()->GetInstrument()->GetInstrumentName() );
    m_pChartData->GetWatch()->SaveSeries( sPrefixSession );
  }
  catch(...) {
    std::cout << " ... issues with saving ... " << std::endl;
  }
  std::cout << "  ... Done " << std::endl;
}

int AppLiveChart::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors
  if ( m_db.IsOpen() ) m_db.Close();

  return wxAppConsole::OnExit();
}

void AppLiveChart::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose
  this->m_pData1Provider->Disconnect();
  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();

  SaveState();

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
  std::ofstream ofs( sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppLiveChart::LoadState() {
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

void AppLiveChart::HandleRegisterTables(  ou::db::Session& session ) {
}

void AppLiveChart::HandleRegisterRows(  ou::db::Session& session ) {
}

void AppLiveChart::HandlePopulateDatabase( void ) {
/*
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
*/
}

