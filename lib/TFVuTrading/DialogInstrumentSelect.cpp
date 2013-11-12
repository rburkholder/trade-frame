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

#include "StdAfx.h"

//#include <wx/validate.h>
//#include <wx/valtext.h>

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
  std::cout << "DialogInstrumentSelect deleted" << std::endl;
}

void DialogInstrumentSelect::Init() {

    m_cbSymbol = NULL;
    m_lblDescription = NULL;
    m_btnOk = NULL;
    m_btnCancel = NULL;

    m_pDataExchange = 0;

}

void DialogInstrumentSelect::SetDataExchange( DataExchange* pde ) {
  m_pDataExchange = pde;
  if ( 0 != pde ) {
    //m_cbSymbol->SetValidator( wxTextValidator( wxFILTER_ALPHANUMERIC, &pde->sSymbolName ) );
    m_cbSymbol->SetValidator( ou::tf::InstrumentNameValidator( &pde->sSymbolName, ou::tf::InstrumentNameValidator::eCapsAlphaNum ) );
  }
  else {
    m_cbSymbol->SetValidator( wxDefaultValidator );
  }
}

bool DialogInstrumentSelect::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {

    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

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
    m_cbSymbol = new wxComboBox( itemDialog1, ID_CBSymbol, wxEmptyString, wxDefaultPosition, wxSize(100, -1), m_cbSymbolStrings, wxCB_DROPDOWN );
    itemBoxSizer3->Add(m_cbSymbol, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblDescription = new wxStaticText( itemDialog1, ID_LblDescription, _("Description"), wxDefaultPosition, wxSize(200, -1), 0 );
    itemBoxSizer3->Add(m_lblDescription, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_btnOk = new wxButton( itemDialog1, ID_BTNOK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(m_btnOk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnCancel = new wxButton( itemDialog1, ID_BTNCancel, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemDialog1->SetAffirmativeId( ID_BTNOK );  // wxID_OK
    itemDialog1->SetEscapeId( ID_BTNCancel );  // wxID_CANCEL

    // http://docs.wxwidgets.org/2.8/wx_wxdialog.html#wxdialogsetescapeid
    Bind( wxEVT_CLOSE_WINDOW, &DialogInstrumentSelect::OnClose, this );
    Bind( wxEVT_COMMAND_BUTTON_CLICKED, &DialogInstrumentSelect::OnOk, this, ID_BTNOK );
    Bind( wxEVT_COMMAND_BUTTON_CLICKED, &DialogInstrumentSelect::OnCancel, this, ID_BTNCancel );

}

// http://docs.wxwidgets.org/trunk/overview_validator.html
void DialogInstrumentSelect::OnOk( wxCommandEvent& event ) {
  // ok button
  if ( Validate() && TransferDataFromWindow() ) {
    if ( IsModal() )
      EndModal(wxID_OK);
    else {
      SetReturnCode(wxID_OK);
      m_pDataExchange->bOk = true;
      this->Show(false);
      if ( 0 != m_OnDoneHandler ) m_OnDoneHandler( m_pDataExchange );
    }
  }
}

void DialogInstrumentSelect::OnCancel( wxCommandEvent& event ) {
  // cancel or escape
  // could be merged with OnClose
  if ( IsModal() )
    EndModal(wxID_CANCEL);
  else {
    SetReturnCode(wxID_CANCEL);
    m_pDataExchange->bOk = false;
    this->Show(false);
    if ( 0 != m_OnDoneHandler ) m_OnDoneHandler( m_pDataExchange );
  }
}

void DialogInstrumentSelect::OnClose( wxCloseEvent& event ) {
  // close button
  if ( IsModal() )
    EndModal(wxID_CANCEL);
  else {
    SetReturnCode(wxID_CANCEL);
    m_pDataExchange->bOk = false;
    this->Show(false);
    if ( 0 != m_OnDoneHandler ) m_OnDoneHandler( m_pDataExchange );
  }
  event.Skip();
}


wxBitmap DialogInstrumentSelect::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon DialogInstrumentSelect::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}


} // namespace tf
} // namespace ou
