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
  m_statusBar = NULL;
}

void FrameMain::CreateControls( void ) {

    FrameMain* itemFrame1 = this;

    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* itemMenu3 = new wxMenu;
    itemMenu3->Append(ID_MENUEXIT, _("Exit"), wxEmptyString, wxITEM_NORMAL);
    menuBar->Append(itemMenu3, _("Menu"));
    itemFrame1->SetMenuBar(menuBar);

    //m_panelMain = new wxPanel( itemFrame1, ID_PANELMAIN, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxTAB_TRAVERSAL );

    m_statusBar = new wxStatusBar( itemFrame1, ID_STATUSBAR, wxST_SIZEGRIP|wxNO_BORDER );
    m_statusBar->SetFieldsCount(2);
    itemFrame1->SetStatusBar(m_statusBar);
    
}

wxBitmap FrameMain::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon FrameMain::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}

