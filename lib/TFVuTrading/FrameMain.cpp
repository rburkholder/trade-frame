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
    wxMenu* itemMenuExit = new wxMenu;
    itemMenuExit->Append(ID_MENUEXIT, _("Exit"), wxEmptyString, wxITEM_NORMAL);
    m_menuBar->Append(itemMenuExit, _("Menu"));
    itemFrame1->SetMenuBar(m_menuBar);

    m_statusBar = new wxStatusBar( itemFrame1, ID_STATUSBAR, wxST_SIZEGRIP|wxNO_BORDER );
    m_statusBar->SetFieldsCount(2);
    itemFrame1->SetStatusBar(m_statusBar);
   
    Bind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuExitClick, this, ID_MENUEXIT );
    Bind( wxEVT_CLOSE_WINDOW, &FrameMain::OnClose, this );
}

void FrameMain::OnMenuExitClick( wxCommandEvent& event ) {
  this->Close();
}

void FrameMain::OnClose( wxCloseEvent& event ) {
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

