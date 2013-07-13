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

// Generic frame for wx based applications

#include "StdAfx.h"

#include "FrameMain.h"

FrameMain::FrameMain(void) {
  Init();
}

FrameMain::FrameMain( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create( parent, id, caption, pos, size, style );
}

FrameMain::~FrameMain(void) {
}

bool FrameMain::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {

  wxFrame::Create( parent, id, caption, pos, size, style );

  CreateControls();
  Centre();
  return true;
}

void FrameMain::Init() {
  m_menuBar = NULL;
  m_statusBar = NULL;
}

void FrameMain::CreateControls( void ) {

    FrameMain* itemFrame1 = this;

    m_menuBar = new wxMenuBar;
    itemFrame1->SetMenuBar(m_menuBar);

    wxMenu* itemMenuFile = new wxMenu;
    itemMenuFile->Append(ID_MENUEXIT, _("Exit"), wxEmptyString, wxITEM_NORMAL);
    m_menuBar->Append(itemMenuFile, _("File"));

    wxMenu* itemMenuAction = new wxMenu;
    itemMenuAction->Append(ID_MENUACTION1, _("Action1"), wxEmptyString, wxITEM_NORMAL);
    itemMenuAction->Append(ID_MENUACTION2, _("Action2"), wxEmptyString, wxITEM_NORMAL);
    itemMenuAction->Append(ID_MENUACTION3, _("Action3"), wxEmptyString, wxITEM_NORMAL);
    itemMenuAction->Append(ID_MENUACTION4, _("Action4"), wxEmptyString, wxITEM_NORMAL);
    itemMenuAction->Append(ID_MENUACTION5, _("Action5"), wxEmptyString, wxITEM_NORMAL);
    m_menuBar->Append(itemMenuAction, _("Actions"));

    m_statusBar = new wxStatusBar( itemFrame1, ID_STATUSBAR, wxST_SIZEGRIP|wxNO_BORDER );
    m_statusBar->SetFieldsCount(2);
    itemFrame1->SetStatusBar(m_statusBar);
   
    Bind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuExitClick, this, ID_MENUEXIT );
    Bind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuAction1Click, this, ID_MENUACTION1 );
    Bind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuAction2Click, this, ID_MENUACTION2 );
    Bind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuAction3Click, this, ID_MENUACTION3 );
    Bind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuAction4Click, this, ID_MENUACTION4 );
    Bind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuAction5Click, this, ID_MENUACTION5 );
    Bind( wxEVT_CLOSE_WINDOW, &FrameMain::OnClose, this );
}

void FrameMain::OnMenuExitClick( wxCommandEvent& event ) {
  // Exit Steps:  #1 -> Appxxx::OnClose
  this->Close();
}

void FrameMain::OnMenuAction1Click( wxCommandEvent& event ) {
  if ( 0 != OnAction1 ) OnAction1();
}

void FrameMain::OnMenuAction2Click( wxCommandEvent& event ) {
  if ( 0 != OnAction2 ) OnAction2();
}

void FrameMain::OnMenuAction3Click( wxCommandEvent& event ) {
  if ( 0 != OnAction3 ) OnAction3();
}

void FrameMain::OnMenuAction4Click( wxCommandEvent& event ) {
  if ( 0 != OnAction4 ) OnAction4();
}

void FrameMain::OnMenuAction5Click( wxCommandEvent& event ) {
  if ( 0 != OnAction5 ) OnAction5();
}

void FrameMain::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #3 -> Appxxx::OnExit
  Unbind( wxEVT_CLOSE_WINDOW, &FrameMain::OnClose, this );
//  Unbind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuExitClick, this, ID_MENUEXIT );  // causes crash
  // http://docs.wxwidgets.org/trunk/classwx_close_event.html
  event.Skip();  // continue with base class stuff
}

wxBitmap FrameMain::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon FrameMain::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}

