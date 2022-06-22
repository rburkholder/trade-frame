/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include <algorithm>

#include "IQFeedGetHistory.h"

IMPLEMENT_APP(AppIQFeedGetHistory)

bool AppIQFeedGetHistory::OnInit() {

  m_pWorker = 0;

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "IQFeed Get History" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 500, 600 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerControls, 0, wxLEFT|wxTOP|wxRIGHT, 5 );

  m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelProviderControl, 1, wxEXPAND|wxRIGHT, 5);
  m_pPanelProviderControl->Show( true );

  LinkToPanelProviderControl();

  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Show( true );

  m_pIQFeedSymbolListOps = new ou::tf::IQFeedSymbolListOps( m_listIQFeedSymbols );
  m_pIQFeedSymbolListOps->Status.connect( [this]( const std::string sStatus ){
    CallAfter( [sStatus](){  // cross threads to foreground
      std::cout << sStatus << std::endl;
    });
  });
  m_pIQFeedSymbolListOps->Done.connect( [this]( ou::tf::IQFeedSymbolListOps::ECompletionCode code ) {
    switch ( code ) {
      case ou::tf::IQFeedSymbolListOps::ECompletionCode::ccCleared:
        DisableMenuActionDays();
        break;
      case ou::tf::IQFeedSymbolListOps::ECompletionCode::ccDone:
        EnableMenuActionDays();
        break;
      case ou::tf::IQFeedSymbolListOps::ECompletionCode::ccSaved:
        break;
    }
  });

  using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects

  FrameMain::vpItems_t vItemsLoadSymbols;
  vItemsLoadSymbols.push_back( new mi( "New Symbol List Remote", MakeDelegate( this, &AppIQFeedGetHistory::HandleNewSymbolListRemote ) ) );
  vItemsLoadSymbols.push_back( new mi( "New Symbol List Local", MakeDelegate( this, &AppIQFeedGetHistory::HandleNewSymbolListLocal ) ) );
  vItemsLoadSymbols.push_back( new mi( "Local Binary Symbol List", MakeDelegate( this, &AppIQFeedGetHistory::HandleLocalBinarySymbolList ) ) );
  wxMenu* pMenuSymbols = m_pFrameMain->AddDynamicMenu( "Load Symbols", vItemsLoadSymbols );

  FrameMain::vpItems_t vItemsLoadDays;
  vItemsLoadDays.push_back( new mi( "10 days", MakeDelegate( this, &AppIQFeedGetHistory::HandleMenuActionDays10 ) ) );
  vItemsLoadDays.push_back( new mi( "30 days", MakeDelegate( this, &AppIQFeedGetHistory::HandleMenuActionDays30 ) ) );
  vItemsLoadDays.push_back( new mi( "100 days", MakeDelegate( this, &AppIQFeedGetHistory::HandleMenuActionDays100 ) ) );
  vItemsLoadDays.push_back( new mi( "150 days", MakeDelegate( this, &AppIQFeedGetHistory::HandleMenuActionDays150 ) ) );
  vItemsLoadDays.push_back( new mi( "200 days", MakeDelegate( this, &AppIQFeedGetHistory::HandleMenuActionDays200 ) ) );
  vItemsLoadDays.push_back( new mi( "0 days", MakeDelegate( this, &AppIQFeedGetHistory::HandleMenuActionDays0 ) ) );
  m_pMenuLoadDays = m_pFrameMain->AddDynamicMenu( "Load History", vItemsLoadDays );

  DisableMenuActionDays();

  return 1;

}

void AppIQFeedGetHistory::DisableMenuActionDays() {
  wxMenuItemList& list( m_pMenuLoadDays->GetMenuItems() );
  std::for_each( list.begin(), list.end(), []( wxMenuItem* mi ){
    mi->Enable( false );
  });
}

void AppIQFeedGetHistory::EnableMenuActionDays() {
  wxMenuItemList& list( m_pMenuLoadDays->GetMenuItems() );
  std::for_each( list.begin(), list.end(), []( wxMenuItem* mi ){
    mi->Enable( true );
  });
}

int AppIQFeedGetHistory::OnExit() {

//  DelinkFromPanelProviderControl();  generates stack errors

//  if ( m_db.IsOpen() ) m_db.Close();

  return 0;
}

void AppIQFeedGetHistory::HandleNewSymbolListRemote( void ) {
  CallAfter(
    [this](){
      m_pIQFeedSymbolListOps->ObtainNewIQFeedSymbolListRemote();
    });
}

void AppIQFeedGetHistory::HandleNewSymbolListLocal( void ) {
  CallAfter(
    [this](){
      m_pIQFeedSymbolListOps->ObtainNewIQFeedSymbolListLocal();
    });
}

void AppIQFeedGetHistory::HandleLocalBinarySymbolList( void ) {
  CallAfter(
    [this](){
      m_pIQFeedSymbolListOps->LoadIQFeedSymbolList();
    });
}

void AppIQFeedGetHistory::HandleMenuActionDays10( void ) {
  StartWorker( "", 10 );
}

void AppIQFeedGetHistory::HandleMenuActionDays30( void ) {
  StartWorker( "", 30 );
}

void AppIQFeedGetHistory::HandleMenuActionDays100( void ) {
  StartWorker( "", 100 );
}

void AppIQFeedGetHistory::HandleMenuActionDays150( void ) {
  StartWorker( "", 150 );
}

void AppIQFeedGetHistory::HandleMenuActionDays200( void ) {
  StartWorker( "", 200 );
}

void AppIQFeedGetHistory::HandleMenuActionDays0( void ) {
  StartWorker( "", 0 );
}

void AppIQFeedGetHistory::StartWorker( const std::string& s, size_t nDatums ) {
  if ( this->m_bIQFeedConnected ) {
    m_pWorker = new Worker( m_listIQFeedSymbols, s, nDatums );
  }
  else {
    std::cout << "No can do.  IQFeed not connected" << std::endl;
  }
}

void AppIQFeedGetHistory::OnData1Connected( int ) {
  std::cout << "History connection connected" << std::endl;
}

void AppIQFeedGetHistory::OnData1Disconnected( int ) {
  std::cout << "History connection disconnected" << std::endl;
}
