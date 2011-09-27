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

#include <exception>

#include <wx/wx.h>
#include <wx/valnum.h>
#include <wx/valtext.h>

#include "ValidatorInstrumentName.h"

#include "FrameManualOrder.h"

FrameManualOrder::FrameManualOrder( void ) {
  Init();
};
  
  // style: wxCAPTION | wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX
FrameManualOrder::FrameManualOrder(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
  : m_ixStruct( 0 )
 {
  Init();
  Create(parent, title, pos, size, style);
}

FrameManualOrder::~FrameManualOrder(void) {
  // test for open and then close?
}

void FrameManualOrder::Init( void ) {
}

bool FrameManualOrder::Create( wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style) {
  wxFrame::Create( parent, wxID_ANY, title, pos, size, style );
  CreateControls();
  if (GetSizer())
  {
      GetSizer()->SetSizeHints(this);
  }
//  Centre();
  return true;
}

void FrameManualOrder::CreateControls( void ) {

    FrameManualOrder* itemFrame1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_LEFT|wxALL, 4);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxALIGN_TOP|wxRIGHT, 2);

    wxStaticText* itemStaticText5 = new wxStaticText( itemFrame1, ID_LblInstrumentSymbol, _("Instrument:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

    m_txtInstrumentSymbol = new wxTextCtrl( itemFrame1, ID_TxtInstrumentSymbol, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, InstrumentNameValidator( &m_order.sSymbol) );
    m_txtInstrumentSymbol->SetMaxLength(20);
    if (FrameManualOrder::ShowToolTips())
        m_txtInstrumentSymbol->SetToolTip(_("Instrument Symbol"));
    itemBoxSizer4->Add(m_txtInstrumentSymbol, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    m_txtInstrumentName = new wxStaticText( itemFrame1, ID_LblInstrumentName, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(m_txtInstrumentName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer8, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer8->Add(itemBoxSizer9, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 2);

    wxStaticText* itemStaticText10 = new wxStaticText( itemFrame1, ID_LblQuantity, _("Quantity:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    itemBoxSizer9->Add(itemStaticText10, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

    wxTextCtrl* itemTextCtrl11 = new wxTextCtrl( itemFrame1, ID_TxtQuantity, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxIntegerValidator<unsigned long>( &m_order.nQuantity, wxNUM_VAL_ZERO_AS_BLANK ) );
    itemTextCtrl11->SetMaxLength(20);
    if (FrameManualOrder::ShowToolTips())
        itemTextCtrl11->SetToolTip(_("Quantity"));
    itemBoxSizer9->Add(itemTextCtrl11, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    wxBoxSizer* itemBoxSizer12 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer8->Add(itemBoxSizer12, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 2);

    wxStaticText* itemStaticText13 = new wxStaticText( itemFrame1, ID_LblPrice1, _("Price 1:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer12->Add(itemStaticText13, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

    wxTextCtrl* itemTextCtrl14 = new wxTextCtrl( itemFrame1, ID_TxtPrice1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxFloatingPointValidator<double>( 4, &m_order.dblPrice1, wxNUM_VAL_ZERO_AS_BLANK ) );
    itemTextCtrl14->SetMaxLength(20);
    if (FrameManualOrder::ShowToolTips())
        itemTextCtrl14->SetToolTip(_("Limit Price"));
    itemBoxSizer12->Add(itemTextCtrl14, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer8->Add(itemBoxSizer15, 0, wxALIGN_CENTER_VERTICAL, 2);

    wxStaticText* itemStaticText16 = new wxStaticText( itemFrame1, ID_LblPrice2, _("Price 2:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer15->Add(itemStaticText16, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

    wxTextCtrl* itemTextCtrl17 = new wxTextCtrl( itemFrame1, ID_TxtPrice2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxFloatingPointValidator<double>( 4, &m_order.dblPrice2, wxNUM_VAL_ZERO_AS_BLANK ) );
    itemTextCtrl17->SetMaxLength(20);
    itemBoxSizer15->Add(itemTextCtrl17, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer18, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer18->Add(itemBoxSizer19, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxRadioButton* itemRadioButton20 = new wxRadioButton( itemFrame1, ID_BtnOrderTypeMarket, _("Market"), wxDefaultPosition, wxDefaultSize, 0 );
    itemRadioButton20->SetValue(false);
    itemBoxSizer19->Add(itemRadioButton20, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxRadioButton* itemRadioButton21 = new wxRadioButton( itemFrame1, ID_BtnOrderTypeLimit, _("&Limit"), wxDefaultPosition, wxDefaultSize, 0 );
    itemRadioButton21->SetValue(true);
    itemBoxSizer19->Add(itemRadioButton21, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxRadioButton* itemRadioButton22 = new wxRadioButton( itemFrame1, ID_BtnOrderTypeStop, _("S&top"), wxDefaultPosition, wxDefaultSize, 0 );
    itemRadioButton22->SetValue(false);
    itemBoxSizer19->Add(itemRadioButton22, 0, wxALIGN_CENTER_VERTICAL, 5);

    itemBoxSizer18->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer24 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer18->Add(itemBoxSizer24, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxButton* itemButton25 = new wxButton( itemFrame1, ID_BtnBuy, _("&Buy"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton25->Enable(false);
    itemBoxSizer24->Add(itemButton25, 0, wxALIGN_CENTER_VERTICAL, 3);

    wxButton* itemButton26 = new wxButton( itemFrame1, ID_BtnSell, _("&Sell"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton26->Enable(false);
    itemBoxSizer24->Add(itemButton26, 0, wxALIGN_CENTER_VERTICAL, 3);

  // Connect events and objects

  Bind( wxEVT_CLOSE_WINDOW, &FrameManualOrder::OnClose, this );

  Bind( wxEVT_SET_FOCUS, &FrameManualOrder::OnFocusChange, this );
  Bind( wxEVT_KILL_FOCUS, &FrameManualOrder::OnFocusChange, this );

  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameManualOrder::OnBtnMarket, this, ID_BtnOrderTypeMarket );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameManualOrder::OnBtnLimit, this, ID_BtnOrderTypeLimit );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameManualOrder::OnBtnStop, this, ID_BtnOrderTypeStop );

  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &FrameManualOrder::OnBtnBuy, this, ID_BtnBuy );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &FrameManualOrder::OnBtnSell, this, ID_BtnSell );

  Bind( wxEVT_IDLE, &FrameManualOrder::OnInstrumentSymbolTextIdle, this, ID_TxtInstrumentSymbol );
  Bind( wxEVT_COMMAND_TEXT_UPDATED, &FrameManualOrder::OnInstrumentSymbolTextUpdated, this, ID_TxtInstrumentSymbol );
  Bind( wxEVT_COMMAND_TEXT_ENTER, &FrameManualOrder::OnInstrumentSymbolTextEnter, this, ID_TxtInstrumentSymbol );
}

void FrameManualOrder::OnClose( wxCloseEvent& event ) {
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

void FrameManualOrder::OnBtnBuy( wxCommandEvent& event ) {
  m_order.eOrderSide = ou::tf::OrderSide::Buy;
  EmitOrder();
};

void FrameManualOrder::OnBtnSell( wxCommandEvent& event ) {
  m_order.eOrderSide = ou::tf::OrderSide::Sell;
  EmitOrder();
};

// doesn't appear to be functional
void FrameManualOrder::OnInstrumentSymbolTextIdle( wxIdleEvent& event ) {
  event.Skip();
}

// one character at a time
// on successful symbol, then set flag to enable buttons
void FrameManualOrder::OnInstrumentSymbolTextUpdated( wxCommandEvent& event ) {
  if ( 0 < m_txtInstrumentSymbol->GetLineLength( 0 ) ) {
    if ( 0 != OnFocusPropogate ) OnFocusPropogate( m_ixStruct );
    if ( 0 != OnSymbolTextUpdated )
      OnSymbolTextUpdated( m_txtInstrumentSymbol->GetLineText( 0 ).ToStdString() );
  }
  //event.Skip();
}

void FrameManualOrder::OnInstrumentSymbolTextEnter( wxCommandEvent& event ) {
  event.Skip();
}

void FrameManualOrder::OnFocusChange( wxFocusEvent& event ) {
  if ( 0 != OnFocusPropogate ) OnFocusPropogate( m_ixStruct );
}

// need to set state on buttons sometime to make validations below unneeded

void FrameManualOrder::EmitOrder( void ) const {
  bool bOk = true;
  if ( 0 == m_order.nQuantity ) bOk = false;
  if ( 0 == m_order.sSymbol.length() ) bOk = false;
  if ( ou::tf::OrderType::Limit == m_order.eOrderType ) {
    if ( 0.0 == m_order.dblPrice1 ) bOk = false;
  }
  if ( bOk ) {
    if ( 0 != OnNewOrder ) 
      OnNewOrder( m_order );
  }
}

void FrameManualOrder::SetInstrumentName( const std::string& sName ) {
  m_txtInstrumentName->SetLabelText( sName );
}