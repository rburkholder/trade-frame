/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    AppDoM.cpp
 * Author:  raymond@burkholder.net
 * Project: App Depth of Market
 * Created on October 12, 2021, 23:04
 */

#include <algorithm>

#include <wx/sizer.h>

#include "AppDoM.h"

IMPLEMENT_APP(AppDoM)

bool AppDoM::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Depth of Market" );
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
  m_sizerControls->Add( m_pPanelProviderControl, 1, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 5);
  m_pPanelProviderControl->Show( true );

  LinkToPanelProviderControl();

  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Show( true );

  using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects

//  FrameMain::vpItems_t vItemsLoadSymbols;
//  vItemsLoadSymbols.push_back( new mi( "New Symbol List Remote", MakeDelegate( this, &AppIQFeedGetHistory::HandleNewSymbolListRemote ) ) );
//  vItemsLoadSymbols.push_back( new mi( "New Symbol List Local", MakeDelegate( this, &AppIQFeedGetHistory::HandleNewSymbolListLocal ) ) );
//  vItemsLoadSymbols.push_back( new mi( "Local Binary Symbol List", MakeDelegate( this, &AppIQFeedGetHistory::HandleLocalBinarySymbolList ) ) );
//  wxMenu* pMenuSymbols = m_pFrameMain->AddDynamicMenu( "Load Symbols", vItemsLoadSymbols );

  return 1;

}

int AppDoM::OnExit() {

//  DelinkFromPanelProviderControl();  generates stack errors

//  if ( m_db.IsOpen() ) m_db.Close();

  return 0;
}

void AppDoM::OnData1Connected( int ) {
  std::cout << "Depth of Market connected" << std::endl;
  m_dispatch.Connect();
}

void AppDoM::OnData1Disconnecting( int ) {
  m_dispatch.Disconnect();
}

void AppDoM::OnData1Disconnected( int ) {
  std::cout << "Depth of Market disconnected" << std::endl;
}
