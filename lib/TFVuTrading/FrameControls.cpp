/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    FrameControls.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFVuTrading
 * Created: February 11, 2022 15:42
 */

#include "FrameControls.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

FrameControls::FrameControls() {
    Init();
}

FrameControls::FrameControls(
  wxWindow* parent, wxWindowID id,
  const wxString& caption, const wxPoint& pos, const wxSize& size, long style
) {
    Init();
    Create( parent, id, caption, pos, size, style );
}

bool FrameControls::Create(
  wxWindow* parent, wxWindowID id, const wxString& caption,
  const wxPoint& pos, const wxSize& size, long style
) {
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    //if (GetSizer()) {
    //    GetSizer()->SetSizeHints(this);
    //}
    //Centre();
    return true;
}

FrameControls::~FrameControls() {
}

void FrameControls::Init() {
}

void FrameControls::CreateControls() {

    FrameControls* itemFrame1 = this;

    m_sizerFrame = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(m_sizerFrame);

    Bind( wxEVT_CLOSE_WINDOW, &FrameControls::OnClose, this );

}

bool FrameControls::ShowToolTips() {
    return true;
}

void FrameControls::Attach( wxWindow* pWindow ) {
  m_sizerFrame->Add( pWindow, 1, wxStretch::wxEXPAND|wxALL, 1 );
  m_sizerFrame->Layout();
  pWindow->Show();
}

void FrameControls::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #3 -> Appxxx::OnExit
  Unbind( wxEVT_CLOSE_WINDOW, &FrameControls::OnClose, this );
//  Unbind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuExitClick, this, ID_MENUEXIT );  // causes crash
  // http://docs.wxwidgets.org/trunk/classwx_close_event.html
  event.Skip();  // continue with base class stuff
}

} // namespace tf
} // namespace ou
