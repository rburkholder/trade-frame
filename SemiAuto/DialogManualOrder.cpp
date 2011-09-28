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

#pragma warning( disable: 4996 4482 )

#include <exception>

#include <wx/wx.h>
#include <wx/valnum.h>
#include <wx/valtext.h>

#include "ValidatorInstrumentName.h"

#include "DialogManualOrder.h"

wxDEFINE_EVENT( EVT_UpdateInstrumentName, UpdateInstrumentNameEvent );

DialogManualOrder::DialogManualOrder( void ) {
  Init();
};
  
  // style: wxCAPTION | wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX
DialogManualOrder::DialogManualOrder( /*wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style*/
  wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style
  )
  : m_ixStruct( 0 )
 {
  Init();
  Create(parent, id, caption, pos, size, style);
}

DialogManualOrder::~DialogManualOrder(void) {
  // test for open and then close?
}

void DialogManualOrder::Init( void ) {
}

bool DialogManualOrder::Create( /*wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style*/
  wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style
  ) {

    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;

}

void DialogManualOrder::CreateControls( void ) {

    DialogManualOrder* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_LEFT|wxALL, 4);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxALIGN_TOP|wxRIGHT, 2);

    wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, ID_LblInstrumentSymbol, _("Instrument:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer4->Add(itemBoxSizer6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_txtInstrumentSymbol = new wxTextCtrl( itemDialog1, ID_TxtInstrumentSymbol, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, InstrumentNameValidator( &m_order.sSymbol) );
    m_txtInstrumentSymbol->SetMaxLength(20);
    if (DialogManualOrder::ShowToolTips())
        m_txtInstrumentSymbol->SetToolTip(_("Instrument Symbol"));
    itemBoxSizer6->Add(m_txtInstrumentSymbol, 0, wxALIGN_CENTER_VERTICAL, 2);

    m_txtInstrumentName = new wxStaticText( itemDialog1, ID_LblInstrumentName, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer6->Add(m_txtInstrumentName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer10 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer9->Add(itemBoxSizer10, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 2);

    wxStaticText* itemStaticText11 = new wxStaticText( itemDialog1, ID_LblQuantity, _("Quantity:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    itemBoxSizer10->Add(itemStaticText11, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

    wxTextCtrl* itemTextCtrl12 = new wxTextCtrl( itemDialog1, ID_TxtQuantity, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxIntegerValidator<unsigned long>( &m_order.nQuantity, wxNUM_VAL_ZERO_AS_BLANK ) );
    itemTextCtrl12->SetMaxLength(20);
    if (DialogManualOrder::ShowToolTips())
        itemTextCtrl12->SetToolTip(_("Quantity"));
    itemBoxSizer10->Add(itemTextCtrl12, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer9->Add(itemBoxSizer13, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 2);

    wxStaticText* itemStaticText14 = new wxStaticText( itemDialog1, ID_LblPrice1, _("Price 1:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer13->Add(itemStaticText14, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

    wxTextCtrl* itemTextCtrl15 = new wxTextCtrl( itemDialog1, ID_TxtPrice1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxFloatingPointValidator<double>( 4, &m_order.dblPrice1, wxNUM_VAL_ZERO_AS_BLANK ) );
    itemTextCtrl15->SetMaxLength(20);
    if (DialogManualOrder::ShowToolTips())
        itemTextCtrl15->SetToolTip(_("Limit Price"));
    itemBoxSizer13->Add(itemTextCtrl15, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer9->Add(itemBoxSizer16, 0, wxALIGN_CENTER_VERTICAL, 2);

    wxStaticText* itemStaticText17 = new wxStaticText( itemDialog1, ID_LblPrice2, _("Price 2:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer16->Add(itemStaticText17, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

    wxTextCtrl* itemTextCtrl18 = new wxTextCtrl( itemDialog1, ID_TxtPrice2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxFloatingPointValidator<double>( 4, &m_order.dblPrice2, wxNUM_VAL_ZERO_AS_BLANK ) );
    itemTextCtrl18->SetMaxLength(20);
    itemBoxSizer16->Add(itemTextCtrl18, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer19, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer19->Add(itemBoxSizer20, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxRadioButton* itemRadioButton21 = new wxRadioButton( itemDialog1, ID_BtnOrderTypeMarket, _("Market"), wxDefaultPosition, wxDefaultSize, 0 );
    itemRadioButton21->SetValue(false);
    itemBoxSizer20->Add(itemRadioButton21, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxRadioButton* itemRadioButton22 = new wxRadioButton( itemDialog1, ID_BtnOrderTypeLimit, _("&Limit"), wxDefaultPosition, wxDefaultSize, 0 );
    itemRadioButton22->SetValue(true);
    itemBoxSizer20->Add(itemRadioButton22, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxRadioButton* itemRadioButton23 = new wxRadioButton( itemDialog1, ID_BtnOrderTypeStop, _("S&top"), wxDefaultPosition, wxDefaultSize, 0 );
    itemRadioButton23->SetValue(false);
    itemBoxSizer20->Add(itemRadioButton23, 0, wxALIGN_CENTER_VERTICAL, 5);

    itemBoxSizer19->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer25 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer19->Add(itemBoxSizer25, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_btnBuy = new wxButton( itemDialog1, ID_BtnBuy, _("&Buy"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnBuy->Enable(false);
    itemBoxSizer25->Add(m_btnBuy, 0, wxALIGN_CENTER_VERTICAL, 3);

    m_btnSell = new wxButton( itemDialog1, ID_BtnSell, _("&Sell"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnSell->Enable(false);
    itemBoxSizer25->Add(m_btnSell, 0, wxALIGN_CENTER_VERTICAL, 3);

    m_btnCancel = new wxButton( itemDialog1, ID_BtnCancel, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnCancel->Show(false);
    m_btnCancel->Enable(false);
    itemBoxSizer25->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_txtStatus = new wxStaticText( itemDialog1, wxID_STATIC, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    itemBoxSizer2->Add(m_txtStatus, 0, wxALIGN_LEFT|wxALL, 5);

  // Connect events and objects

  Bind( wxEVT_CLOSE_WINDOW, &DialogManualOrder::OnClose, this );

  Bind( wxEVT_SET_FOCUS, &DialogManualOrder::OnFocusChange, this );
  Bind( wxEVT_KILL_FOCUS, &DialogManualOrder::OnFocusChange, this );

  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &DialogManualOrder::OnBtnMarket, this, ID_BtnOrderTypeMarket );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &DialogManualOrder::OnBtnLimit, this, ID_BtnOrderTypeLimit );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &DialogManualOrder::OnBtnStop, this, ID_BtnOrderTypeStop );

  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &DialogManualOrder::OnBtnBuy, this, ID_BtnBuy );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &DialogManualOrder::OnBtnSell, this, ID_BtnSell );

  Bind( wxEVT_IDLE, &DialogManualOrder::OnInstrumentSymbolTextIdle, this, ID_TxtInstrumentSymbol );
  Bind( wxEVT_COMMAND_TEXT_UPDATED, &DialogManualOrder::OnInstrumentSymbolTextUpdated, this, ID_TxtInstrumentSymbol );
  Bind( wxEVT_COMMAND_TEXT_ENTER, &DialogManualOrder::OnInstrumentSymbolTextEnter, this, ID_TxtInstrumentSymbol );

  Bind( EVT_UpdateInstrumentName, &DialogManualOrder::OnUpdateInstrumentName, this );
}

void DialogManualOrder::OnClose( wxCloseEvent& event ) {
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

void DialogManualOrder::OnBtnBuy( wxCommandEvent& event ) {
  m_order.eOrderSide = ou::tf::OrderSide::Buy;
  EmitOrder();
};

void DialogManualOrder::OnBtnSell( wxCommandEvent& event ) {
  m_order.eOrderSide = ou::tf::OrderSide::Sell;
  EmitOrder();
};

// doesn't appear to be functional
void DialogManualOrder::OnInstrumentSymbolTextIdle( wxIdleEvent& event ) {
  event.Skip();
}

// one character at a time
// on successful symbol, then set flag to enable buttons
void DialogManualOrder::OnInstrumentSymbolTextUpdated( wxCommandEvent& event ) {
  if ( 0 == m_txtInstrumentSymbol->GetLineLength( 0 ) ) {
    m_txtInstrumentName->SetLabelText( "" );
    DisableButtons();
  }
  else {
    if ( 0 != OnFocusPropogate ) OnFocusPropogate( m_ixStruct );
    if ( 0 != OnSymbolTextUpdated )
      OnSymbolTextUpdated( m_txtInstrumentSymbol->GetLineText( 0 ).ToStdString() );
  }
  //event.Skip();
}

void DialogManualOrder::OnInstrumentSymbolTextEnter( wxCommandEvent& event ) {
  event.Skip();
}

void DialogManualOrder::OnFocusChange( wxFocusEvent& event ) {
  if ( 0 != OnFocusPropogate ) OnFocusPropogate( m_ixStruct );
}

// need to set state on buttons sometime to make validations below unneeded

void DialogManualOrder::EmitOrder( void ) {
  m_txtStatus->SetLabelText( "" );
  if ( Validate() && TransferDataFromWindow() ) {
    bool bOk = true;
    if ( 0 == m_order.nQuantity ) bOk = false;
    if ( 0 == m_order.sSymbol.length() ) bOk = false;
    if ( ou::tf::OrderType::Limit == m_order.eOrderType ) {
      if ( 0.0 == m_order.dblPrice1 ) bOk = false;
    }
    if ( bOk ) {
      if ( 0 != OnNewOrder ) 
        m_txtInstrumentSymbol->Enable( false ); // so we can keep the Instrument fixed.
        OnNewOrder( m_order );
    }
    else {
      m_txtStatus->SetLabelText( "Incomplete Fields" );
    }
  }
  else {
    m_txtStatus->SetLabelText( "Error in Validation" );
  }

}

void DialogManualOrder::OnUpdateInstrumentName( UpdateInstrumentNameEvent& event ) {
  m_txtInstrumentName->SetLabelText( event.InstrumentName() );
  0 == event.InstrumentName().size() ? DisableButtons() : EnableButtons();
}

void DialogManualOrder::EnableButtons( void ) {
  m_btnBuy->Enable(true);
  m_btnSell->Enable(true);
}

void DialogManualOrder::DisableButtons( void ) {
  m_btnBuy->Enable(false);
  m_btnSell->Enable(false);
}