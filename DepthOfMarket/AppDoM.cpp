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

/*
TODO:
  * select symbol
  * start watch:
      obtain trades, match against orders
      obtain quotes, match against top of each book
  * start Market Depth
      validate against trade stream for actual orders (limits vs market)
*/

#include <algorithm>

#include <memory>
#include <wx/sizer.h>

#include "Config.h"

#include "AppDoM.h"
#include "TFVuTrading/MarketDepth/PanelTrade.h"
#include "wx/defs.h"

IMPLEMENT_APP(AppDoM)

bool AppDoM::OnInit() {

  int code = 1;

  config::Options options;

  if ( !Load( options ) ) {
    code = 0;
  }
  else {

    m_pFrameMain = new FrameMain( nullptr, wxID_ANY, "Depth of Market" );
    wxWindowID idFrameMain = m_pFrameMain->GetId();
    //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
    //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
    //m_pFrameMain->Center();
  //  m_pFrameMain->Move( -2500, 50 );
    m_pFrameMain->SetSize( 675, 700 );
    SetTopWindow( m_pFrameMain );

    //Bind(
    //  wxEVT_SIZE,
    //  [this](wxSizeEvent& event){
    //    std::cout << "w=" << event.GetSize().GetWidth() << ",h=" << event.GetSize().GetHeight() << std::endl;
    //    event.Skip();
    //    }//,
    //  //idFrameMain
    //  );

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

    wxBoxSizer* sizerTrade = new wxBoxSizer( wxHORIZONTAL );
    m_sizerMain->Add( sizerTrade, 1, wxEXPAND|wxALL, 5 );

    m_pPanelTrade = new ou::tf::l2::PanelTrade( m_pFrameMain, wxID_ANY );
    sizerTrade->Add( m_pPanelTrade, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);

    wxBoxSizer* sizerStatus = new wxBoxSizer( wxHORIZONTAL );
    m_sizerMain->Add( sizerStatus, 1, wxEXPAND|wxALL, 5 );

    m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
    sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
    m_pPanelLogging->Show( true );

    m_pFrameMain->Show( true );

    m_pDispatch = std::make_unique<DoMDispatch>( options.sSymbolName );

    std::cout << "watching " << options.sSymbolName << std::endl;

    using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects

  //  FrameMain::vpItems_t vItemsLoadSymbols;
  //  vItemsLoadSymbols.push_back( new mi( "New Symbol List Remote", MakeDelegate( this, &AppIQFeedGetHistory::HandleNewSymbolListRemote ) ) );
  //  vItemsLoadSymbols.push_back( new mi( "New Symbol List Local", MakeDelegate( this, &AppIQFeedGetHistory::HandleNewSymbolListLocal ) ) );
  //  vItemsLoadSymbols.push_back( new mi( "Local Binary Symbol List", MakeDelegate( this, &AppIQFeedGetHistory::HandleLocalBinarySymbolList ) ) );
  //  wxMenu* pMenuSymbols = m_pFrameMain->AddDynamicMenu( "Load Symbols", vItemsLoadSymbols );

  }

  return code;
}

void AppDoM::OnClose( wxCloseEvent& event ) {

  m_pDispatch.reset();

  if ( m_bData1Connected ) {
    m_pDispatch->Disconnect();
  }

  //if ( m_worker.joinable() ) m_worker.join();
  //m_timerGuiRefresh.Stop();
  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a

  //SaveState();

  //if ( m_db.IsOpen() ) m_db.Close();
  event.Skip();  // auto followed by Destroy();
}

int AppDoM::OnExit() {

//  if ( m_db.IsOpen() ) m_db.Close();

  return 0;
}

void AppDoM::OnData1Connected( int ) {
  std::cout << "Depth of Market connected" << std::endl;
  m_pDispatch->Connect();
}

void AppDoM::OnData1Disconnecting( int ) {
  m_pDispatch->Disconnect();
}

void AppDoM::OnData1Disconnected( int ) {
  std::cout << "Depth of Market disconnected" << std::endl;
}
