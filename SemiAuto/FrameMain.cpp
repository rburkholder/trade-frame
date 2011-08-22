/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <wx/event.h>

#include "FrameMain.h"

FrameMain::FrameMain(const wxString& title, const wxPoint& pos, const wxSize& size)
  : wxFrame((wxFrame*)NULL, -1, title, pos, size)
{
  // create menubar
  wxMenuBar* menuBar = new wxMenuBar;
  // create menu
  wxMenu* menuFile = new wxMenu;
  menuFile->Append(ID_About, "&About...\tF1");
  menuFile->AppendSeparator();
  menuFile->Append(ID_Quit, "E&xit\tAlt-X");
  menuBar->Append(menuFile, "&File");

  wxMenu* menuConnect = new wxMenu;
  menuConnect->Append( ID_ConnectIB, "&Interactive Brokers" );
  menuConnect->Append( ID_ConnectIQFeed, "IQ&Feed" );
  menuConnect->Append( ID_ConnectSim, "&Simulation" );
  menuBar->Append( menuConnect, "&Connect" );

  wxMenu* menuDisConnect = new wxMenu;
  menuDisConnect->Append( ID_DisConnectIB, "&Interactive Brokers" );
  menuDisConnect->Append( ID_DisConnectIQFeed, "IQ&Feed" );
  menuDisConnect->Append( ID_DisConnectSim, "&Simulation" );
  menuBar->Append( menuDisConnect, "&Disconnect" );

  wxMenu* menuActivities = new wxMenu;
  menuActivities->Append( ID_AddPortfolio, "Add P&ortfolio" );
  menuActivities->Append( ID_ManualTrade, "Manual &Trade" );
  menuBar->Append( menuActivities, "&Activities" );

  // set frame menubar
  SetMenuBar(menuBar);
 
  Bind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnQuit, this, ID_Quit );
  Bind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnAbout, this, ID_About );
  Bind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnOpenFrameManualOrder, this, ID_ManualTrade );

  Bind( wxEVT_CLOSE_WINDOW, &FrameMain::OnCloseThis, this );
  //Bind( EVT_CLOSE, &FrameMain::OnCloseThis, this, ID_CloseWindow );
 
  CreateStatusBar();
  SetStatusText("Semi Auto Trading");

  Centre();

}


FrameMain::~FrameMain(void) {
}

void FrameMain::CleanUpForExit( void ) {
  for ( vFrameManualOrder_t::iterator iter = m_vFrameManualOrders.begin(); iter != m_vFrameManualOrders.end(); ++iter ) {
    (*iter)->Close();
    delete *iter;
  }
  m_vFrameManualOrders.clear();
}

void FrameMain::OnCloseThis( wxCloseEvent& event ) {
  CleanUpForExit();
  event.Skip();
}

void FrameMain::OnQuit(wxCommandEvent& WXUNUSED(event)) {
  CleanUpForExit();
  Close(TRUE);
}
 
void FrameMain::OnAbout(wxCommandEvent& WXUNUSED(event)) {
   wxMessageBox( "Semi-Auto Trading\nCopyright (2011) One Unified\nWritten By: Raymond Burkholder\ninfo@oneunified.net", "TradeFrame FrameWork",
        wxOK|wxICON_INFORMATION, this);
}

void FrameMain::OnClose( wxCommandEvent& event ) {
}

void FrameMain::OnOpenFrameManualOrder( wxCommandEvent& event ) {
  // need to keep a vector of these so can trade multiple symbols simulataneously
  // maybe something like genesis with market depth book built in
  FrameManualOrder* frame = new FrameManualOrder( this );
  m_vFrameManualOrders.push_back( frame );
  frame->Show( true );
  event.Skip();
}