/************************************************************************
 * Copyright(c) 2026, One Unified. All rights reserved.                 *
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
 * File:    PanelDebug.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: January 26, 2026 19:56:26
 */

#include <boost/log/trivial.hpp>

#include <wx/sizer.h>
#include <wx/stattext.h>

#include "PanelDebug.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelDebug::PanelDebug(): wxPanel() {
  Init();
}

PanelDebug::PanelDebug( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxPanel()
{
  Init();
  Create( parent, id, pos, size, style, name );
}

PanelDebug::~PanelDebug() {
}

void PanelDebug::Init() {
}

bool PanelDebug::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {

    SetExtraStyle( wxWS_EX_VALIDATE_RECURSIVELY );
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if ( GetSizer() ) {
      GetSizer()->SetSizeHints( this );
    }

  return true;
}

void PanelDebug::CreateControls() {

  PanelDebug* itemPanel1 = this;

  m_pSizer = new wxBoxSizer( wxVERTICAL );
  itemPanel1->SetSizer( m_pSizer );

  Bind( wxEVT_DESTROY, &PanelDebug::OnDestroy, this );

}

void PanelDebug::Update( const std::string& key, const std::string& value ) {
  mapFields_t::iterator iter = m_mapFields.find( key );
  if ( m_mapFields.end() == iter ) {
    wxBoxSizer* pSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pSizer->Add( pSizer, 0, wxGROW|wxALL, 0 );
    wxStaticText* pTextKey = new wxStaticText( this, wxID_ANY, key + " = ", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    pSizer->Add( pTextKey, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 4 );
    wxStaticText* pTextValue = new wxStaticText( this, wxID_ANY, value, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    pSizer->Add( pTextValue, 1, wxGROW|wxRIGHT, 4 );
    m_mapFields.emplace( key, std::pair( pTextKey, pTextValue ) );
  }
  else {
    iter->second.second->SetLabel( value );
    // maybe, if blank, remove the key
  }
  Layout();
}

void PanelDebug::OnDestroy( wxWindowDestroyEvent& event ) {

  assert( Unbind( wxEVT_DESTROY, &PanelDebug::OnDestroy, this ) );
  event.Skip( true );  // auto followed by Destroy();

}

wxBitmap PanelDebug::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

} // namespace tf
} // namespace ou

