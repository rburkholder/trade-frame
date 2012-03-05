/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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
#include "PanelLogging.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelLogging::PanelLogging(void) {
  Init();
}

PanelLogging::PanelLogging( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelLogging::~PanelLogging(void) {
  if ( 0 != m_pOldStreamBuf ) {
    std::cout.rdbuf(m_pOldStreamBuf);
    m_pOldStreamBuf = 0;
  }
}

void PanelLogging::Init() {
  m_txtLogging = 0;
  m_pOldStreamBuf = 0;
}

bool PanelLogging::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())     {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;
}

void PanelLogging::CreateControls() {    

    PanelLogging* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    m_txtLogging = new wxTextCtrl( itemPanel1, ID_TEXTLOGGING, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
    //m_txtLogging = new wxTextCtrl( itemPanel1, ID_TEXTLOGGING, wxEmptyString, wxDefaultPosition, wxSize(400, 300), wxTE_MULTILINE|wxTE_READONLY );
    itemBoxSizer2->Add(m_txtLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 5);

    m_pOldStreamBuf = std::cout.rdbuf();
    std::cout.rdbuf(m_txtLogging);

}


wxBitmap PanelLogging::GetBitmapResource( const wxString& name )
{
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelLogging::GetIconResource( const wxString& name )
{
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou
