/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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
 * Created on December 22, 2015, 3:04 PM
 */

#include <wx/sizer.h>
#include <wx/icon.h>
#include <wx/textentry.h>
#include <wx/valnum.h>
#include <wx/valgen.h>

#include <TFVuTrading/ValidatorInstrumentName.h>
#include <wx-3.0/wx/textctrl.h>
#include <boost/lexical_cast.hpp>
//#include <TFVuTrading/wxETKBaseValidator.h>

#include "DialogPickSymbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

DialogPickSymbol::DialogPickSymbol() {
  Init();
}

DialogPickSymbol::DialogPickSymbol( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, caption, pos, size, style);
}

bool DialogPickSymbol::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {
  
////@begin PanelPickSymbol creation
    //SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY); // from dialogblocks
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);  // from DialogInstrumentSelect
    DialogBase::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer() ) {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end PanelPickSymbol creation
    return true;
}

DialogPickSymbol::~DialogPickSymbol() {
}

void DialogPickSymbol::Init() {
    m_radioEquity = NULL;
    m_radioOption = NULL;
    m_radioFuture = NULL;
    m_radioFOption = NULL;
    m_textSymbol = NULL;
    m_txtSymbolDescription = NULL;
    m_textComposite = NULL;
    m_txtCompositeDescription = NULL;
    m_dateExpiry = NULL;
    m_textStrike = NULL;
    m_radioOptionPut = NULL;
    m_radioOptionCall = NULL;
    m_radioCurrencyUSD = NULL;
    m_radioCurrencyCAD = NULL;
    m_btnOk = NULL;
    m_btnCancel = NULL;
}

void DialogPickSymbol::CreateControls() {    

    DialogPickSymbol* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_LEFT|wxALL, 5);

    m_radioEquity = new wxRadioButton( itemPanel1, ID_RADIO_EQUITY, _("Equity"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_radioEquity->SetValue(true);
    m_radioEquity->Enable(false);
    itemBoxSizer3->Add(m_radioEquity, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_radioOption = new wxRadioButton( itemPanel1, ID_RADIO_OPTION, _("Option"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioOption->SetValue(false);
    m_radioOption->Enable(false);
    itemBoxSizer3->Add(m_radioOption, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_radioFuture = new wxRadioButton( itemPanel1, ID_RADIO_FUTURE, _("Future"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioFuture->SetValue(false);
    m_radioFuture->Enable(false);
    itemBoxSizer3->Add(m_radioFuture, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_radioFOption = new wxRadioButton( itemPanel1, ID_RADIO_FOPTION, _("FOption"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioFOption->SetValue(false);
    m_radioFOption->Enable(false);
    itemBoxSizer3->Add(m_radioFOption, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer8, 0, wxGROW|wxALL, 5);

    m_textSymbol = new wxTextCtrl( itemPanel1, ID_TEXT_SYMBOL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_textSymbol->Enable(false);
    itemBoxSizer8->Add(m_textSymbol, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtSymbolDescription = new wxStaticText( itemPanel1, ID_STATIC_SYMBOL_DESCRIPTION, _("Symbol Description"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(m_txtSymbolDescription, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer11, 0, wxALIGN_LEFT|wxALL, 5);

    m_textComposite = new wxTextCtrl( itemPanel1, ID_TEXT_COMPOSITE, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    itemBoxSizer11->Add(m_textComposite, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtCompositeDescription = new wxStaticText( itemPanel1, ID_STATIC_COMPOSITE_DESCRIPTION, _("Composite Description"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(m_txtCompositeDescription, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer14 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer14, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer14->Add(itemBoxSizer15, 0, wxALIGN_TOP|wxALL, 5);

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer15->Add(itemBoxSizer16, 0, wxALIGN_LEFT|wxALL, 2);

    wxStaticText* itemStaticText17 = new wxStaticText( itemPanel1, wxID_STATIC, _("Expiry:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer16->Add(itemStaticText17, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_dateExpiry = new wxDatePickerCtrl( itemPanel1, ID_DATE_EXPIRY, wxDateTime(), wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT|wxDP_SHOWCENTURY );
    m_dateExpiry->Enable(false);
    itemBoxSizer16->Add(m_dateExpiry, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer15->Add(itemBoxSizer19, 0, wxALIGN_LEFT|wxALL, 2);

    wxStaticText* itemStaticText20 = new wxStaticText( itemPanel1, wxID_STATIC, _("Strike:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer19->Add(itemStaticText20, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_textStrike = new wxTextCtrl( itemPanel1, ID_TEXT_STRIKE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_textStrike->SetMaxLength(20);
    m_textStrike->Enable(false);
    itemBoxSizer19->Add(m_textStrike, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer22 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer15->Add(itemBoxSizer22, 0, wxALIGN_LEFT|wxALL, 2);

    wxStaticText* itemStaticText23 = new wxStaticText( itemPanel1, wxID_STATIC, _("Option Side:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer22->Add(itemStaticText23, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_radioOptionPut = new wxRadioButton( itemPanel1, ID_RADIO_PUT, _("Put"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_radioOptionPut->SetValue(false);
    m_radioOptionPut->Enable(false);
    itemBoxSizer22->Add(m_radioOptionPut, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_radioOptionCall = new wxRadioButton( itemPanel1, ID_RADIO_CALL, _("Call"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioOptionCall->SetValue(false);
    m_radioOptionCall->Enable(false);
    itemBoxSizer22->Add(m_radioOptionCall, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer26 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer14->Add(itemBoxSizer26, 0, wxALIGN_TOP|wxALL, 2);

    m_radioCurrencyUSD = new wxRadioButton( itemPanel1, ID_RADIO_USD, _("USD"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_radioCurrencyUSD->SetValue(true);
    m_radioCurrencyUSD->Enable(false);
    itemBoxSizer26->Add(m_radioCurrencyUSD, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    m_radioCurrencyCAD = new wxRadioButton( itemPanel1, ID_RADIO_CAD, _("CAD"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioCurrencyCAD->SetValue(false);
    m_radioCurrencyCAD->Enable(false);
    itemBoxSizer26->Add(m_radioCurrencyCAD, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer29 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer29, 0, wxGROW|wxALL, 2);

    itemBoxSizer29->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_btnOk = new wxButton( itemPanel1, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnOk->Enable(false);
    itemBoxSizer29->Add(m_btnOk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);

    itemBoxSizer29->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_btnCancel = new wxButton( itemPanel1, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer29->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    itemBoxSizer29->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);
    
    Bind( wxEVT_COMMAND_TEXT_UPDATED, &DialogPickSymbol::HandleSymbolChange, this, ID_TEXT_SYMBOL );

  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &DialogPickSymbol::HandleRadioEquity, this, ID_RADIO_EQUITY );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &DialogPickSymbol::HandleRadioOption, this, ID_RADIO_OPTION );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &DialogPickSymbol::HandleRadioFuture, this, ID_RADIO_FUTURE );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &DialogPickSymbol::HandleRadioFOption, this, ID_RADIO_FOPTION );
  
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &DialogPickSymbol::HandleRadioPut, this, ID_RADIO_PUT );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &DialogPickSymbol::HandleRadioCall, this, ID_RADIO_CALL );
  
  Bind( wxEVT_DATE_CHANGED, &DialogPickSymbol::HandleExpiryChanged, this, ID_DATE_EXPIRY );

  //Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelManualOrder::OnBtnBuy, this, ID_BtnBuy );
  //Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelManualOrder::OnBtnSell, this, ID_BtnSell );

  //Bind( wxEVT_IDLE, &PanelManualOrder::OnInstrumentSymbolTextIdle, this, ID_TxtInstrumentSymbol );
  //Bind( wxEVT_COMMAND_TEXT_UPDATED, &PanelManualOrder::OnInstrumentSymbolTextUpdated, this, ID_TxtInstrumentSymbol );
  //Bind( wxEVT_COMMAND_TEXT_ENTER, &PanelManualOrder::OnInstrumentSymbolTextEnter, this, ID_TxtInstrumentSymbol );

}

void DialogPickSymbol::HandleSymbolChange( wxCommandEvent& event ) {
  wxString text( m_textSymbol->GetValue() );
  std::string sText( text.c_str() );
  if ( 0 == sText.length() ) {
    m_txtSymbolDescription->SetLabel( "" );
    if ( 0 != m_btnOk ) {
      m_btnOk->Enable( false );
    }
  }
  else {
    DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
    if ( !pde->signalLookupDescription.empty() ) {
      m_txtSymbolDescription->SetLabel( "" );  // temporary blank out while obtaining new description
      std::string sDescription;
      pde->signalLookupDescription( sText, sDescription );
      //sDescription = pde->lookup( sText );
      m_txtSymbolDescription->SetLabel( sDescription );
      if ( 0 != m_btnOk ) {
        m_btnOk->Enable( 0 != sDescription.length() );
      }
    }
  }
}

void DialogPickSymbol::HandleRadioEquity( wxCommandEvent& event ) {
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  DisableOptionFields();
  pde->it = InstrumentType::Stock;
  UpdateComposite();
  m_textStrike->SetFocus();
}

void DialogPickSymbol::HandleRadioOption( wxCommandEvent& event ) {
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  pde->it = InstrumentType::Option;
  m_radioOptionPut->Enable();
  m_radioOptionCall->Enable();
  m_dateExpiry->Enable();
  m_textStrike->Enable();
  UpdateComposite();
  m_textStrike->SetFocus();
}

void DialogPickSymbol::HandleRadioFuture( wxCommandEvent& event ) {
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  pde->it = InstrumentType::Future;
  DisableOptionFields();
  m_dateExpiry->Enable();
  UpdateComposite();
  m_textStrike->SetFocus();
}
 
void DialogPickSymbol::HandleRadioFOption( wxCommandEvent& event ) {
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  pde->it = InstrumentType::FuturesOption;
  m_radioOptionPut->Enable();
  m_radioOptionCall->Enable();
  m_dateExpiry->Enable();
  m_textStrike->Enable();
  UpdateComposite();
  m_textStrike->SetFocus();
}
  
void DialogPickSymbol::HandleRadioPut( wxCommandEvent& event ) {
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  pde->os = OptionSide::Put;
  UpdateComposite();
}

void DialogPickSymbol::HandleRadioCall( wxCommandEvent& event ) {
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  pde->os = OptionSide::Call;
  UpdateComposite();
}
  
void DialogPickSymbol::HandleExpiryChanged( wxDateEvent& event ) {
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  const wxDateTime& dt( event.GetDate() );
  pde->year = dt.GetYear();
  pde->month = dt.GetMonth();
  pde->day = dt.GetDay();
  UpdateComposite();
}

void DialogPickSymbol::UpdateComposite( void ) {
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  pde->sUnderlyingSymbolName = this->m_textSymbol->GetValue();
  pde->dblStrike = boost::lexical_cast<double>( this->m_textStrike->GetValue() );
  pde->signalComposeComposite( pde );
  m_textComposite->SetValue( pde->sCompositeName );
  m_txtCompositeDescription->SetLabel( pde->sCompositeDescription );
}

void DialogPickSymbol::SetDataExchange( DataExchange* pde ) {
  DialogBase::SetDataExchange( pde );
  if ( 0 != pde ) {
    m_btnOk->Enable();
    m_textSymbol->Enable();
    m_textSymbol->SetValidator( ou::tf::InstrumentNameValidator( &pde->sUnderlyingSymbolName, ou::tf::InstrumentNameValidator::eCapsAlphaNum ) );
    //m_textSymbol->SetValidator( wxETKTextValidator( wxFILTER_UPPERCASE, &pde->sUnderlyingSymbolName, m_textSymbol ) ); // wxFILTER_ALPHANUMERIC_STRICT
    m_textStrike->SetValidator( wxFloatingPointValidator<double>( 2, &pde->dblStrike, wxNUM_VAL_DEFAULT  ) );
    m_radioEquity->Enable();
    m_radioOption->Enable();
    m_radioFuture->Enable();
    m_radioFOption->Enable();
    m_textStrike->SetFocus();
  }
  else {
    //m_textSymbol->SetValidator( wxDefaultValidator );
    m_radioEquity->Disable();
    m_radioOption->Disable();
    m_radioFuture->Disable();
    m_radioFOption->Disable();
    m_textSymbol->Disable();
    DisableOptionFields();
  }
}

void DialogPickSymbol::DisableOptionFields( void ) {
    m_radioOptionPut->Disable();
    m_radioOptionCall->Disable();
    m_dateExpiry->Disable();
    m_textStrike->Disable();
}

wxBitmap DialogPickSymbol::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon DialogPickSymbol::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou
