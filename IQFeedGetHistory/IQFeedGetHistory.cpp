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

// CAV.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "IQFeedGetHistory.h"

IMPLEMENT_APP(AppIQFeedGetHistory)

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>
#include <TFTrading/OrderManager.h>
#include <TFTrading/PortfolioManager.h>

//#include <TFIQFeed/ParseMktSymbolDiskFile.h>

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
  m_sizerControls->Add( m_pPanelProviderControl, 1, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 5);
  m_pPanelProviderControl->Show( true );

  LinkToPanelProviderControl();
/*
  m_pPanelOptionsParameters = new PanelOptionsParameters( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelOptionsParameters, 1, wxEXPAND|wxALIGN_LEFT, 0);
  m_pPanelOptionsParameters->Show( true );
  m_pPanelOptionsParameters->SetOnStart( MakeDelegate( this, &AppStrategyRunner::HandleBtnStart ) );
  m_pPanelOptionsParameters->SetOnStop( MakeDelegate( this, &AppStrategyRunner::HandleBtnStop ) );
  m_pPanelOptionsParameters->SetOnSave( MakeDelegate( this, &AppStrategyRunner::HandleBtnSave ) );
  m_pPanelOptionsParameters->SetOptionNearDate( boost::gregorian::date( 2012, 4, 20 ) );
  m_pPanelOptionsParameters->SetOptionFarDate( boost::gregorian::date( 2012, 6, 15 ) );
*/
  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Show( true );

//  m_db.OnRegisterTables.Add( MakeDelegate( this, &AppCollectAndView::HandleRegisterTables ) );
//  m_db.OnRegisterRows.Add( MakeDelegate( this, &AppCollectAndView::HandleRegisterRows ) );
//  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppCollectAndView::HandlePopulateDatabase ) );

  // maybe set scenario with database and with in memory data structure
//  m_db.Open( "cav.db" );

  FrameMain::vpItems_t vItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  vItems.push_back( new mi( "10 days", MakeDelegate( this, &AppIQFeedGetHistory::HandleMenuActionDays10 ) ) );
  vItems.push_back( new mi( "30 days", MakeDelegate( this, &AppIQFeedGetHistory::HandleMenuActionDays30 ) ) );
  vItems.push_back( new mi( "100 days", MakeDelegate( this, &AppIQFeedGetHistory::HandleMenuActionDays100 ) ) );
  vItems.push_back( new mi( "0 days", MakeDelegate( this, &AppIQFeedGetHistory::HandleMenuActionDays0 ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );


  // test that iqfeed is connected.



  return 1;

}

int AppIQFeedGetHistory::OnExit() {

//  DelinkFromPanelProviderControl();  generates stack errors

//  if ( m_db.IsOpen() ) m_db.Close();

  return 0;
}

void AppIQFeedGetHistory::HandleMenuActionDays10( void ) {
  m_pWorker = new Worker( "", 10 );
}

void AppIQFeedGetHistory::HandleMenuActionDays30( void ) {
  m_pWorker = new Worker( "", 30 );
}

void AppIQFeedGetHistory::HandleMenuActionDays100( void ) {
  m_pWorker = new Worker( "", 100 );
}

void AppIQFeedGetHistory::HandleMenuActionDays0( void ) {
  m_pWorker = new Worker( "", 0 );
}
