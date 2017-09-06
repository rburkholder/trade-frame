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

#include "stdafx.h"

#include "ArmsIndex.h"

IMPLEMENT_APP(AppArmsIndex)

bool AppArmsIndex::OnInit() {

  wxApp::OnInit();
  wxApp::SetAppDisplayName( "Arms Index" );
  wxApp::SetVendorName( "OneUnified" );
  wxApp:SetVendorDisplayName( "One Unified" );

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Arms Index" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 900, 800 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  //wxBoxSizer* m_sizerControls;
  //m_sizerControls = new wxBoxSizer( wxHORIZONTAL );
  //m_sizerMain->Add( m_sizerControls, 1, wxLEFT|wxTOP|wxRIGHT, 5 );

  // populate variable in FrameWork01
  //m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
  //m_sizerControls->Add( m_pPanelProviderControl, 0, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 5);
  //m_pPanelProviderControl->Show( true );

  //LinkToPanelProviderControl();
  
    auto pSplitterPanels = new wxSplitterWindow( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DBORDER|wxSP_3DSASH|wxEXPAND );
    pSplitterPanels->SetMinimumPaneSize(100);

    auto pPanelUpper = new wxPanel( pSplitterPanels, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    auto pSizerUpper = new wxBoxSizer(wxVERTICAL);
    pPanelUpper->SetSizer(pSizerUpper);

    auto pPanelLower = new wxPanel( pSplitterPanels, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    auto pSizerLower = new wxBoxSizer(wxVERTICAL);
    pPanelLower->SetSizer(pSizerLower);

  //m_pPanelArmsIndex = new ou::tf::PanelArmsIndex( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxSize(600, 200) );
  m_pPanelArmsIndex = new ou::tf::PanelArmsIndex( pPanelUpper, wxID_ANY );
  pSizerUpper->Add( m_pPanelArmsIndex, 1, wxGROW|wxALL |wxEXPAND , 1);
  m_pPanelArmsIndex->Show( true );

  m_pPanelLogging = new ou::tf::PanelLogging( pPanelLower, wxID_ANY );
  pSizerLower->Add( m_pPanelLogging, 1, wxGROW|wxALL|wxEXPAND, 1 );
  m_pPanelLogging->Show( true );

    pSplitterPanels->SplitHorizontally(pPanelUpper, pPanelLower, 600);
    m_sizerMain->Add(pSplitterPanels, 1, wxGROW|wxALL, 2);
  
  
  
  //wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  //m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  std::cout << "(c) 2017 One Unified Ltd.  All Rights Reserved.  info@oneunified.net" << std::endl;

  m_db.OnRegisterTables.Add( MakeDelegate( this, &AppArmsIndex::HandleRegisterTables ) );
  m_db.OnRegisterRows.Add( MakeDelegate( this, &AppArmsIndex::HandleRegisterRows ) );
  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppArmsIndex::HandlePopulateDatabase ) );
  m_db.SetOnLoadDatabaseHandler( MakeDelegate( this, &AppArmsIndex::HandleLoadDatabase ) );

  m_sDbName = "ArmsIndex.db";
  if ( boost::filesystem::exists( m_sDbName ) ) {
//    boost::filesystem::remove( sDbName );
  }

//  FrameMain::vpItems_t vItems;
//  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
//  vItems.push_back( new mi( "a1 New Symbol List Remote", MakeDelegate( m_pIQFeedSymbolListOps, &ou::tf::IQFeedSymbolListOps::ObtainNewIQFeedSymbolListRemote ) ) );
//  vItems.push_back( new mi( "a2 New Symbol List Local", MakeDelegate( m_pIQFeedSymbolListOps, &ou::tf::IQFeedSymbolListOps::ObtainNewIQFeedSymbolListLocal ) ) );
//  vItems.push_back( new mi( "a3 Load Symbol List", MakeDelegate( m_pIQFeedSymbolListOps, &ou::tf::IQFeedSymbolListOps::LoadIQFeedSymbolList ) ) );
//  vItems.push_back( new mi( "a4 Save Symbol Subset", MakeDelegate( this, &AppStickShift::HandleMenuActionSaveSymbolSubset ) ) );
//  vItems.push_back( new mi( "a5 Load Symbol Subset", MakeDelegate( this, &AppStickShift::HandleMenuActionLoadSymbolSubset ) ) );
//  m_pFrameMain->AddDynamicMenu( "Actions", vItems );


  m_timerGuiRefresh.SetOwner( this );

  Bind( wxEVT_TIMER, &AppArmsIndex::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppArmsIndex::OnClose, this );  // start close of windows and controls

  Bind( EVENT_PROVIDER_CONNECTED, &AppArmsIndex::HandleProviderConnected, this );

  m_pFrameMain->SetAutoLayout( true );
  m_pFrameMain->Layout();
  m_pFrameMain->Show( true );

  m_bData1Connected = false;
  m_bExecConnected = false;
  m_bStarted = false;

  //this->m_pData1Provider->Connect();
  this->m_iqfeed->Connect();

  return 1;

}

void AppArmsIndex::Start( void ) {
  if ( !m_bStarted ) {
    m_bStarted = true;
    m_pPanelArmsIndex->SetProvider( m_iqfeed );
    m_timerGuiRefresh.Start( 500 );
  }
}

void AppArmsIndex::HandleGuiRefresh( wxTimerEvent& event ) {
  m_pPanelArmsIndex->UpdateGUI();
}

void AppArmsIndex::HandleProviderConnected( EventProviderConnected& event ) {
  // In foreground thread
  Start();
}

void AppArmsIndex::OnIQFeedConnected( int ) {  // crosses thread
//  Start();
  // In background thread
  QueueEvent( new EventProviderConnected( EVENT_PROVIDER_CONNECTED, -1, m_iqfeed ) );
}

void AppArmsIndex::OnData1Connected( int ) {
  m_bData1Connected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
    //Start();
  }
}

void AppArmsIndex::OnExecConnected( int ) {
  m_bExecConnected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
    //Start();
  }
}

void AppArmsIndex::OnData1Disconnected( int ) {
  std::cout << "Data1 Disconnected" << std::endl;
  m_bData1Connected = false;
}

void AppArmsIndex::OnExecDisconnected( int ) {
  std::cout << "Exec1 Disconnected" << std::endl;
  m_bExecConnected = false;
}

void AppArmsIndex::OnClose( wxCloseEvent& event ) {
//  pm.OnPortfolioLoaded.Remove( MakeDelegate( this, &AppStickShift::HandlePortfolioLoad ) );
//  pm.OnPositionLoaded.Remove( Mak
  
  m_timerGuiRefresh.Stop();
  
  this->m_iqfeed->Disconnect();
  
  if ( m_db.IsOpen() ) m_db.Close();
  
  //DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

int AppArmsIndex::OnExit() {

//  DelinkFromPanelProviderControl();  generates stack errors
  //m_timerGuiRefresh.Stop();
  
  //this->m_iqfeed->Disconnect();
  
  //if ( m_db.IsOpen() ) m_db.Close();

  return wxApp::OnExit();
}

void AppArmsIndex::HandleRegisterTables(  ou::db::Session& session ) {
}

void AppArmsIndex::HandleRegisterRows(  ou::db::Session& session ) {
}

void AppArmsIndex::HandlePopulateDatabase( void ) {
}

void AppArmsIndex::HandleLoadDatabase( void ) {
//    ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
//    pm.LoadActivePortfolios();
}

