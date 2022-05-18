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

#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>

#include "PanelLogging.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

wxDEFINE_EVENT( EVT_ConsoleString, ConsoleStringEvent );

PanelLogging::PanelLogging() {
  Init();
}

PanelLogging::PanelLogging( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create( parent, id, pos, size, style );
}

PanelLogging::~PanelLogging() {
}

void PanelLogging::Init() {
  m_txtLogging = nullptr;
  m_pOldStreamBuf = nullptr;
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
    //itemBoxSizer2->Add(m_txtLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 5);
    itemBoxSizer2->Add(m_txtLogging, 1, wxGROW|wxALL, 3);

    m_pOldStreamBuf = std::cout.rdbuf();
    //std::cout.rdbuf(m_txtLogging);
    std::cout.rdbuf( &m_csb );

    Bind( wxEVT_DESTROY, &PanelLogging::OnDestroy, this );
    //Bind( wxEVT_CLOSE_WINDOW, &PanelLogging::OnClose, this );
    Bind( EVT_ConsoleString, &PanelLogging::HandleConsoleLine1, this );

    m_csb.SetOnEmitString( MakeDelegate( this, &PanelLogging::HandleConsoleLine0 ) );
}

wxBitmap PanelLogging::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelLogging::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

void PanelLogging::HandleConsoleLine0( csb_t::Buf* pBuf ) {
  this->QueueEvent( new ConsoleStringEvent( EVT_ConsoleString, pBuf ) );
}

void PanelLogging::HandleConsoleLine1( ConsoleStringEvent& event ) {
  std::string s( event.GetBuf()->buf, event.GetBuf()->n );
  //m_txtLogging->SetValue( s );
  m_txtLogging->AppendText( s );
  m_csb.ReturnBuffer( event.GetBuf() );
}

void PanelLogging::OnDestroy( wxWindowDestroyEvent& event ) {

  Unbind( EVT_ConsoleString, &PanelLogging::HandleConsoleLine1, this );
  //Unbind( wxEVT_CLOSE_WINDOW, &PanelLogging::OnClose, this );
  Unbind( wxEVT_DESTROY, &PanelLogging::OnDestroy, this );

  m_csb.SetOnEmitString( nullptr );
  if ( nullptr != m_pOldStreamBuf ) {
    std::cout.rdbuf( m_pOldStreamBuf );
    m_pOldStreamBuf = nullptr;
  }

  //std::cout << "PanelLogging::OnDestroy" << std::endl;

  event.Skip();  // auto followed by Destroy();
}

} // namespace tf
} // namespace ou
