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

#include <TFVuTrading/ValidatorInstrumentName.h>

#include "DialogInstrumentSelect.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

IMPLEMENT_DYNAMIC_CLASS( DialogInstrumentSelect, wxDialog )

DialogInstrumentSelect::DialogInstrumentSelect(void) {
  Init();
}

DialogInstrumentSelect::DialogInstrumentSelect( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {
    Init();
    Create(parent, id, caption, pos, size, style);
}

DialogInstrumentSelect::~DialogInstrumentSelect(void) {
}

void DialogInstrumentSelect::Init() {

    m_cbSymbol = NULL;
    m_lblDescription = NULL;
    m_btnOk = NULL;
    m_btnCancel = NULL;

}

void DialogInstrumentSelect::SetDataExchange( DataExchange* pde ) {
  DialogBase::SetDataExchange( pde );
  if ( 0 != pde ) {
    m_cbSymbol->SetValidator( ou::tf::InstrumentNameValidator( &pde->sSymbolName ) );  // caps, alpha, numeric, @
  }
  else {
    m_cbSymbol->SetValidator( wxDefaultValidator );
  }
}

bool DialogInstrumentSelect::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {

    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    DialogBase::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer()) {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;

}

void DialogInstrumentSelect::CreateControls() {  

    DialogInstrumentSelect* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxArrayString m_cbSymbolStrings;
    m_cbSymbol = new wxComboBox( itemDialog1, ID_CBSymbol, wxEmptyString, wxDefaultPosition, wxSize(170, -1), m_cbSymbolStrings, wxCB_DROPDOWN );
    itemBoxSizer3->Add(m_cbSymbol, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_lblDescription = new wxStaticText( itemDialog1, ID_LblDescription, _("Description"), wxDefaultPosition, wxSize(300, -1), 0 );
    itemBoxSizer3->Add(m_lblDescription, 1, wxALIGN_CENTER_VERTICAL, 2);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnOk = new wxButton( itemDialog1, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(m_btnOk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemBoxSizer6->Add(20, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnCancel = new wxButton( itemDialog1, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);
    
    Bind( wxEVT_COMMAND_TEXT_UPDATED, &DialogInstrumentSelect::HandleTextChange, this, ID_CBSymbol );

}

void DialogInstrumentSelect::HandleTextChange( wxCommandEvent& event ) {
  wxString text( m_cbSymbol->GetValue() );
  std::string sText( text.c_str() );
  if ( 0 == sText.length() ) {
    m_lblDescription->SetLabel( "" );
    if ( 0 != m_btnOk ) {
      m_btnOk->Enable( false );
    }
  }
  else {
    DataExchange* pde = reinterpret_cast<DialogInstrumentSelect::DataExchange*>( m_pDataExchange );
    if ( 0 != pde->lookup ) {
      std::string sDescription;
      pde->lookup( sText, sDescription );
      m_lblDescription->SetLabel( sDescription );
      if ( 0 != m_btnOk ) {
        m_btnOk->Enable( 0 != sDescription.length() );
      }
    }
  }
}

} // namespace tf
} // namespace ou
