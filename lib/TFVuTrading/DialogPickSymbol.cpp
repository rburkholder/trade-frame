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

#include "DialogPickSymbol.h"

// from https://wiki.wxwidgets.org/WxNotebook
// wxWindow objects do not transfer the focus to their first child widget; while wxPanel pages do.

namespace ou { // One Unified
namespace tf { // TradeFrame

wxDEFINE_EVENT( EVT_SetFocus, SetFocusEvent );

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

  m_bIBSymbolChanging = false;
  m_bOptionOnly = false;
  m_bFuturesOptionOnly = false;
  
    m_radioEquity = NULL;
    m_radioOption = NULL;
    m_radioFuture = NULL;
    m_radioFOption = NULL;
    m_txtIQFRootName = NULL;
    m_txtIBName = NULL;
    m_txtSymbolDescription = NULL;
    m_txtIQFeedFullName = NULL;
    m_txtContractId = NULL;
    m_txtIQFeedDescription = NULL;
    m_dateExpiry = NULL;
    m_txtStrike = NULL;
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

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer8, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer8->Add(itemBoxSizer9, 1, wxALIGN_LEFT|wxALL, 2);

    wxStaticText* itemStaticText10 = new wxStaticText( itemPanel1, wxID_STATIC, _("IQFeed:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemStaticText10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txtIQFRootName = new wxTextCtrl( itemPanel1, ID_TEXT_IQF_SYMBOL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_txtIQFRootName->Enable(false);
    itemBoxSizer9->Add(m_txtIQFRootName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    itemBoxSizer9->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText13 = new wxStaticText( itemPanel1, wxID_STATIC, _("IB:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemStaticText13, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txtIBName = new wxTextCtrl( itemPanel1, ID_TEXT_IB_SYMBOL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(m_txtIBName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtSymbolDescription = new wxStaticText( itemPanel1, ID_STATIC_SYMBOL_DESCRIPTION, _("Symbol Description"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(m_txtSymbolDescription, 0, wxALIGN_LEFT|wxALL, 2);

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer16, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer16->Add(itemBoxSizer17, 0, wxGROW|wxALL, 5);

    m_txtIQFeedFullName = new wxTextCtrl( itemPanel1, ID_TEXT_COMPOSITE, wxEmptyString, wxDefaultPosition, wxSize(120, -1), wxTE_READONLY );
    itemBoxSizer17->Add(m_txtIQFeedFullName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    itemBoxSizer17->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtContractId = new wxStaticText( itemPanel1, ID_STATIC_CONTRACTID, _("contract"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer17->Add(m_txtContractId, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtIQFeedDescription = new wxStaticText( itemPanel1, ID_STATIC_COMPOSITE_DESCRIPTION, _("Composite Description"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer16->Add(m_txtIQFeedDescription, 1, wxALIGN_LEFT|wxALL, 2);

    wxBoxSizer* itemBoxSizer22 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer22, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer23 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer22->Add(itemBoxSizer23, 0, wxALIGN_TOP|wxALL, 5);

    wxBoxSizer* itemBoxSizer24 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer23->Add(itemBoxSizer24, 0, wxALIGN_LEFT|wxALL, 2);

    wxStaticText* itemStaticText25 = new wxStaticText( itemPanel1, wxID_STATIC, _("Strike:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer24->Add(itemStaticText25, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtStrike = new wxTextCtrl( itemPanel1, ID_TEXT_STRIKE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_txtStrike->SetMaxLength(20);
    m_txtStrike->Enable(false);
    itemBoxSizer24->Add(m_txtStrike, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer27 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer23->Add(itemBoxSizer27, 0, wxGROW|wxALL, 2);

    wxStaticText* itemStaticText28 = new wxStaticText( itemPanel1, wxID_STATIC, _("Option Side:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer27->Add(itemStaticText28, 0, wxALIGN_LEFT|wxALL, 2);

    wxBoxSizer* itemBoxSizer29 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer27->Add(itemBoxSizer29, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

    m_radioOptionPut = new wxRadioButton( itemPanel1, ID_RADIO_PUT, _("Put"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_radioOptionPut->SetValue(false);
    m_radioOptionPut->Enable(false);
    itemBoxSizer29->Add(m_radioOptionPut, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    itemBoxSizer29->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_radioOptionCall = new wxRadioButton( itemPanel1, ID_RADIO_CALL, _("Call"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioOptionCall->SetValue(false);
    m_radioOptionCall->Enable(false);
    itemBoxSizer29->Add(m_radioOptionCall, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer33 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer22->Add(itemBoxSizer33, 0, wxALIGN_TOP|wxALL, 5);

    wxBoxSizer* itemBoxSizer34 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer33->Add(itemBoxSizer34, 0, wxALIGN_LEFT|wxALL, 2);

    wxStaticText* itemStaticText35 = new wxStaticText( itemPanel1, wxID_STATIC, _("Expiry:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer34->Add(itemStaticText35, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_dateExpiry = new wxDatePickerCtrl( itemPanel1, ID_DATE_EXPIRY, wxDateTime(), wxDefaultPosition, wxSize(120, -1), wxDP_DEFAULT|wxDP_SHOWCENTURY );
    m_dateExpiry->Enable(false);
    itemBoxSizer34->Add(m_dateExpiry, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer37 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer33->Add(itemBoxSizer37, 0, wxGROW|wxALL, 2);

    wxStaticText* itemStaticText38 = new wxStaticText( itemPanel1, wxID_STATIC, _("Currency:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer37->Add(itemStaticText38, 0, wxALIGN_LEFT|wxALL, 2);

    wxBoxSizer* itemBoxSizer39 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer37->Add(itemBoxSizer39, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

    m_radioCurrencyUSD = new wxRadioButton( itemPanel1, ID_RADIO_USD, _("USD"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_radioCurrencyUSD->SetValue(true);
    m_radioCurrencyUSD->Enable(false);
    itemBoxSizer39->Add(m_radioCurrencyUSD, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    itemBoxSizer39->Add(19, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_radioCurrencyCAD = new wxRadioButton( itemPanel1, ID_RADIO_CAD, _("CAD"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioCurrencyCAD->SetValue(false);
    m_radioCurrencyCAD->Enable(false);
    itemBoxSizer39->Add(m_radioCurrencyCAD, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxBoxSizer* itemBoxSizer43 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer43, 0, wxGROW|wxALL, 2);

    itemBoxSizer43->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_btnOk = new wxButton( itemPanel1, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnOk->Enable(false);
    itemBoxSizer43->Add(m_btnOk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 4);

    itemBoxSizer43->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_btnCancel = new wxButton( itemPanel1, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer43->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    itemBoxSizer43->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);
    
  Bind( wxEVT_COMMAND_TEXT_UPDATED, &DialogPickSymbol::HandleIQFSymbolChanged, this, ID_TEXT_IQF_SYMBOL );
  Bind( wxEVT_COMMAND_TEXT_UPDATED, &DialogPickSymbol::HandleIBSymbolChanged, this, ID_TEXT_IB_SYMBOL );
  Bind( wxEVT_COMMAND_TEXT_UPDATED, &DialogPickSymbol::HandleStrikeChanged, this, ID_TEXT_STRIKE );

  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &DialogPickSymbol::HandleRadioEquity, this, ID_RADIO_EQUITY );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &DialogPickSymbol::HandleRadioOption, this, ID_RADIO_OPTION );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &DialogPickSymbol::HandleRadioFuture, this, ID_RADIO_FUTURE );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &DialogPickSymbol::HandleRadioFOption, this, ID_RADIO_FOPTION );
  
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &DialogPickSymbol::HandleRadioPut, this, ID_RADIO_PUT );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &DialogPickSymbol::HandleRadioCall, this, ID_RADIO_CALL );
  
  Bind( wxEVT_DATE_CHANGED, &DialogPickSymbol::HandleExpiryChanged, this, ID_DATE_EXPIRY );
  
  Bind( EVT_SetFocus, &DialogPickSymbol::HandleSetFocus, this );

  // examples:
  //Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelManualOrder::OnBtnBuy, this, ID_BtnBuy );
  //Bind( wxEVT_IDLE, &PanelManualOrder::OnInstrumentSymbolTextIdle, this, ID_TxtInstrumentSymbol );
  //Bind( wxEVT_COMMAND_TEXT_ENTER, &PanelManualOrder::OnInstrumentSymbolTextEnter, this, ID_TxtInstrumentSymbol );

}

void DialogPickSymbol::SetBasic( void ) {
  m_bOptionOnly = false;
  m_bFuturesOptionOnly = false;
  m_radioEquity->Enable();
  m_radioFuture->Enable();
  m_radioFOption->Disable();
  m_radioOption->Disable();
  m_radioOption->SetValue( false );
  m_radioFOption->SetValue( false );
  //SetRadioOption();
}

void DialogPickSymbol::SetAll( void ) {
  m_bOptionOnly = false;
  m_bFuturesOptionOnly = false;
  m_radioEquity->Enable();
  m_radioFuture->Enable();
  m_radioFOption->Enable();
  m_radioOption->Enable();
  //m_radioOption->SetValue( false );
  //m_radioFOption->SetValue( false );
  //SetRadioOption();
}

void DialogPickSymbol::SetOptionOnly( void ) {
  m_bOptionOnly = true;
  m_radioEquity->Disable();
  m_radioFuture->Disable();
  m_radioFOption->Disable();
  m_radioOption->Enable();
  m_radioOption->SetValue( true );
  SetRadioOption();
}

void DialogPickSymbol::SetFuturesOptionOnly( void ) {
  m_bFuturesOptionOnly = true;
  m_radioEquity->Disable();
  m_radioOption->Disable();
  m_radioFuture->Disable();
  m_radioFOption->Enable();
  m_radioFOption->SetValue( true );
  SetRadioFuturesOption();
}

void DialogPickSymbol::HandleIQFSymbolChanged( wxCommandEvent& event ) {
  
  //std::cout << "DialogPickSymbol::HandleIQFSymbolChanged entered" << std::endl;
  
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  
  m_txtSymbolDescription->SetLabel( "" );
  m_btnOk->Enable( false );
  pde->nContractId = 0;
  
  wxString text( m_txtIQFRootName->GetValue() );
  std::string sText( text.c_str() );

  m_bIBSymbolChanging = true;
  m_txtIBName->SetValue( text ); // this triggers HandleIBSymbolChanged, so need m_bIBSymbolChanging
  m_bIBSymbolChanging = false;
  pde->sIBSymbolName = text;
  
  std::string sDescription;
  //pde->signalLookupIQFeedDescription( sText, sDescription );
  pde->fLookupIQFeedDescription( sText, sDescription );
  if ( 0 != sDescription.length() ) {
    m_txtSymbolDescription->SetLabel( sDescription );
  }
  UpdateComposite();
}

void DialogPickSymbol::HandleIBSymbolChanged( wxCommandEvent& event ) {
  if ( !m_bIBSymbolChanging ) {
    DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
    pde->sIBSymbolName = m_txtIBName->GetValue();
    UpdateComposite();
  }
}

void DialogPickSymbol::HandleStrikeChanged(wxCommandEvent& event) {
  m_btnOk->Enable( false );
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  std::string sStrike( m_txtStrike->GetValue() );
  switch ( pde->it ) {
    case InstrumentType::Option:
    case InstrumentType::FuturesOption:
      if ( 0 != sStrike.length() ) {
        pde->dblStrike = boost::lexical_cast<double>( sStrike );
      }
      else {
        pde->dblStrike = 0.0;
      }
      break;
  }
  UpdateComposite();
}

void DialogPickSymbol::HandleRadioEquity( wxCommandEvent& event ) {
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  DisableOptionFields();
  pde->it = InstrumentType::Stock;
  UpdateComposite();
  QueueEvent( new SetFocusEvent( EVT_SetFocus, m_txtIQFRootName ) );
}

void DialogPickSymbol::HandleRadioOption( wxCommandEvent& event ) {
  SetRadioOption();
}

void DialogPickSymbol::SetRadioOption( void ) {
  m_btnOk->Enable( false );
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  pde->it = InstrumentType::Option;
  m_radioOptionPut->Enable();
  m_radioOptionCall->Enable();
  m_dateExpiry->Enable();
  m_txtStrike->Enable();
  UpdateComposite();
  QueueEvent( new SetFocusEvent( EVT_SetFocus, m_txtStrike ) );
}

void DialogPickSymbol::HandleRadioFuture( wxCommandEvent& event ) {
  m_btnOk->Enable( false );
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  pde->it = InstrumentType::Future;
  DisableOptionFields();
  m_dateExpiry->Enable();
  UpdateComposite();
  QueueEvent( new SetFocusEvent( EVT_SetFocus, m_dateExpiry ) );
}
 
void DialogPickSymbol::HandleRadioFOption( wxCommandEvent& event ) {
  SetRadioFuturesOption();
}
  
void DialogPickSymbol::SetRadioFuturesOption( void ) {
  m_btnOk->Enable( false );
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  pde->it = InstrumentType::FuturesOption;
  m_radioOptionPut->Enable();
  m_radioOptionCall->Enable();
  m_dateExpiry->Enable();
  m_txtStrike->Enable();
  UpdateComposite();
  QueueEvent( new SetFocusEvent( EVT_SetFocus, m_txtStrike ) );
}
  
void DialogPickSymbol::HandleRadioPut( wxCommandEvent& event ) {
  m_btnOk->Enable( false );
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  pde->os = OptionSide::Put;
  UpdateComposite();
}

void DialogPickSymbol::HandleRadioCall( wxCommandEvent& event ) {
  m_btnOk->Enable( false );
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  pde->os = OptionSide::Call;
  UpdateComposite();
}
  
void DialogPickSymbol::HandleExpiryChanged( wxDateEvent& event ) {
  m_btnOk->Enable( false );
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  const wxDateTime& dt( event.GetDate() );
  pde->year = dt.GetYear();
  pde->month = dt.GetMonth();
  pde->day = dt.GetDay();
  UpdateComposite();
}

void DialogPickSymbol::UpdateComposite( void ) {
  
  //std::cout << "DialogPickSymbol::UpdateComposite" << std::endl;
  
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  
  UpdateContractId();
  
  pde->sIQFSymbolName = this->m_txtIQFRootName->GetValue();
  pde->sIQFeedDescription = "";  // UpdateContractId set button to ok, this undoes it
  //pde->signalComposeIQFeedFullName( pde );
  pde->fComposeIQFeedFullName( pde );
  m_txtIQFeedFullName->SetValue( pde->sIQFeedFullName );
  m_txtIQFeedDescription->SetLabel( pde->sIQFeedDescription );
  UpdateBtnOk();
}

void DialogPickSymbol::UpdateBtnOk( void ) {
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  bool bOk = ( 0 != pde->sIQFeedDescription.length() ) && ( 0 != pde->nContractId );
  m_btnOk->Enable( bOk );
}

void DialogPickSymbol::UpdateContractId( void ) {
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  if ( 0 == pde->nContractId ) {
    m_txtContractId->SetLabel( "-no contract id-" );
  }
  else {
    m_txtContractId->SetLabel( boost::lexical_cast<std::string>( pde->nContractId ) );
  }
  UpdateBtnOk();
}

void DialogPickSymbol::UpdateContractId( int32_t nContractId ) {
  DataExchange* pde = reinterpret_cast<DialogPickSymbol::DataExchange*>( m_pDataExchange );
  pde->nContractId = nContractId;
  //m_txtContractId->SetLabel( boost::lexical_cast<std::string>( nContractId ) );
  UpdateContractId();
}

void DialogPickSymbol::DisableOptionFields( void ) {
    m_radioOptionPut->Disable();
    m_radioOptionCall->Disable();
    m_dateExpiry->Disable();
    m_txtStrike->Disable();
}

void DialogPickSymbol::HandleSetFocus( SetFocusEvent& event ) {
  event.GetWindow()->SetFocus();
}

void DialogPickSymbol::SetDataExchange( DataExchange* pde ) {
  DialogBase::SetDataExchange( pde );
  if ( 0 != pde ) {
    m_txtIQFRootName->Enable();
    m_txtIQFRootName->SetValidator( ou::tf::InstrumentNameValidator( &pde->sIQFSymbolName ) );   // caps, alpha, numeric, @
    m_txtIBName->SetValidator( ou::tf::InstrumentNameValidator( &pde->sIBSymbolName ) );   // caps, alpha, numeric, @
    //m_textSymbol->SetValidator( wxETKTextValidator( wxFILTER_UPPERCASE, &pde->sUnderlyingSymbolName, m_textSymbol ) ); // wxFILTER_ALPHANUMERIC_STRICT
    m_txtStrike->SetValidator( wxFloatingPointValidator<double>( 2, &pde->dblStrike, wxNUM_VAL_DEFAULT  ) );
    m_radioEquity->Enable();
    m_radioOption->Enable();
    m_radioFuture->Enable();
    m_radioFOption->Enable();
    
    std::vector<wxDateTime::Month> vMonth = { 
      wxDateTime::Month::Jan,
      wxDateTime::Month::Feb,
      wxDateTime::Month::Mar,
      wxDateTime::Month::Apr,
      wxDateTime::Month::May,
      wxDateTime::Month::Jun,
      wxDateTime::Month::Jul,
      wxDateTime::Month::Aug,
      wxDateTime::Month::Sep,
      wxDateTime::Month::Oct,
      wxDateTime::Month::Nov,
      wxDateTime::Month::Dec
    };
    wxDateTime::Month month = vMonth[ pde->month ];
    if ( ( pde->year > 0 ) && ( pde->month > 0 ) && ( pde->day > 0 ) ) {
      const wxDateTime dt( pde->day, month, pde->year, 0, 0 );
      m_dateExpiry->SetValue( dt);
    }
    else {
      wxDateTime dt = m_dateExpiry->GetValue();
      pde->year = dt.GetYear();
      pde->month = dt.GetMonth();
      pde->day = dt.GetDay();
    }
    
    switch ( pde->os ) {
      case OptionSide::enumOptionSide::Call:
        m_radioOptionCall->SetValue( true );
        break;
      case OptionSide::enumOptionSide::Put:
        m_radioOptionPut->SetValue( true );
        break;
    }
    
    //m_textIQFRootName->SetFocus();
  }
  else {
    //m_textSymbol->SetValidator( wxDefaultValidator );
    m_radioEquity->Disable();
    m_radioOption->Disable();
    m_radioFuture->Disable();
    m_radioFOption->Disable();
    m_txtIQFRootName->Disable();
    DisableOptionFields();
  }
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
