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

#include <iostream>

#include "Weeklies.h"

/*
 * NOTE:  SignalGenerator::ScanBars needs to be updated with begin/end dates for processing (200 daily bars required minimum)
 * TODO:  update gui to select begin/end dates of scan
 *
 * NOTE: spreadsheet is obtained from http://www.cboe.com/micro/weeklys/availableweeklys.aspx
 *
 * Input:  x64/weeklysmf.xls
 * Output: x64/debug/weeklies.xls
 */

IMPLEMENT_APP(AppWeeklies)

bool AppWeeklies::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Weeklies" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 800, 500 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerControls, 0, wxLEFT|wxTOP|wxRIGHT, 5 );

  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Show( true );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppWeeklies::OnClose, this );  // start close of windows and controls

  FrameMain::vpItems_t vItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  vItems.push_back( new mi( "Run Scan", MakeDelegate( this, &AppWeeklies::HandleMenuActionRunScan ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  return 1;

}

void AppWeeklies::HandleMenuActionRunScan( void ) {
  CallAfter( [this](){
    if ( m_worker.joinable() ) m_worker.join(); // need to finish off any previous thread
    m_worker = std::thread( &SignalGenerator::Run, std::ref( m_sg ) );
  });
}

int AppWeeklies::OnExit() {
  // Exit Steps: #4

  return wxAppConsole::OnExit();
}

void AppWeeklies::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose
  if ( m_worker.joinable() )
    m_worker.join();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  event.Skip();  // auto followed by Destroy();
}

