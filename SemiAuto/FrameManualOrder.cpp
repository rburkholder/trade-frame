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
  itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 4);

  wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizer3->Add(itemBoxSizer4, 0, wxALIGN_TOP|wxRIGHT, 2);

  wxStaticText* itemStaticText5 = new wxStaticText( itemFrame1, ID_LblInstrument, _("Instrument"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
  itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

  wxTextCtrl* itemTextCtrl6 = new wxTextCtrl( itemFrame1, ID_TxtInstrument, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 
    InstrumentNameValidator( &m_order.sInstrument) );
  itemTextCtrl6->SetMaxLength(20);
  if (FrameManualOrder::ShowToolTips())
      itemTextCtrl6->SetToolTip(_("Instrument Symbol"));
  itemBoxSizer4->Add(itemTextCtrl6, 0, wxALIGN_CENTER_HORIZONTAL, 2);

  wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizer3->Add(itemBoxSizer7, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 2);

  wxStaticText* itemStaticText8 = new wxStaticText( itemFrame1, ID_LblQuantity, _("Quantity"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
  itemBoxSizer7->Add(itemStaticText8, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

  wxTextCtrl* itemTextCtrl9 = new wxTextCtrl( itemFrame1, ID_TxtQuantity, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,
    wxIntegerValidator<unsigned long>( &m_order.nQuantity, wxNUM_VAL_ZERO_AS_BLANK ) );
  itemTextCtrl9->SetMaxLength(20);
  if (FrameManualOrder::ShowToolTips())
      itemTextCtrl9->SetToolTip(_("Quantity"));
  itemBoxSizer7->Add(itemTextCtrl9, 0, wxALIGN_CENTER_HORIZONTAL, 2);

  wxBoxSizer* itemBoxSizer10 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizer3->Add(itemBoxSizer10, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 2);

  wxStaticText* itemStaticText11 = new wxStaticText( itemFrame1, ID_LblPrice1, _("Price 1"), wxDefaultPosition, wxDefaultSize, 0);
  itemBoxSizer10->Add(itemStaticText11, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

  wxTextCtrl* itemTextCtrl12 = new wxTextCtrl( itemFrame1, ID_TxtPrice1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,
    wxFloatingPointValidator<double>( 4, &m_order.dblPrice1, wxNUM_VAL_ZERO_AS_BLANK )  );
  itemTextCtrl12->SetMaxLength(20);
  if (FrameManualOrder::ShowToolTips())
      itemTextCtrl12->SetToolTip(_("Limit Price"));
  itemBoxSizer10->Add(itemTextCtrl12, 0, wxALIGN_CENTER_HORIZONTAL, 2);

  wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizer3->Add(itemBoxSizer13, 0, wxALIGN_CENTER_VERTICAL, 2);

  wxStaticText* itemStaticText14 = new wxStaticText( itemFrame1, ID_LblPrice2, _("Price 2"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer13->Add(itemStaticText14, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 3);

  wxTextCtrl* itemTextCtrl15 = new wxTextCtrl( itemFrame1, ID_TxtPrice2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,
    wxFloatingPointValidator<double>( 4, &m_order.dblPrice2, wxNUM_VAL_ZERO_AS_BLANK ) );
  itemTextCtrl15->SetMaxLength(20);
  itemBoxSizer13->Add(itemTextCtrl15, 0, wxALIGN_CENTER_HORIZONTAL, 5);

  wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer2->Add(itemBoxSizer16, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

  wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer16->Add(itemBoxSizer17, 0, wxALIGN_CENTER_VERTICAL, 5);

  wxRadioButton* itemRadioButton18 = new wxRadioButton( itemFrame1, ID_BtnOrderTypeMarket, _("Market"), wxDefaultPosition, wxDefaultSize, 0 );
  itemRadioButton18->SetValue(false);
  itemBoxSizer17->Add(itemRadioButton18, 0, wxALIGN_CENTER_VERTICAL, 5);

  wxRadioButton* itemRadioButton19 = new wxRadioButton( itemFrame1, ID_BtnOrderTypeLimit, _("Limit"), wxDefaultPosition, wxDefaultSize, 0 );
  itemRadioButton19->SetValue(true);
  itemBoxSizer17->Add(itemRadioButton19, 0, wxALIGN_CENTER_VERTICAL, 5);

  wxRadioButton* itemRadioButton20 = new wxRadioButton( itemFrame1, ID_BtnOrderTypeStop, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
  itemRadioButton20->SetValue(false);
  itemBoxSizer17->Add(itemRadioButton20, 0, wxALIGN_CENTER_VERTICAL, 5);

  itemBoxSizer16->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  wxBoxSizer* itemBoxSizer22 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer16->Add(itemBoxSizer22, 0, wxALIGN_CENTER_VERTICAL, 5);

  wxButton* itemButton23 = new wxButton( itemFrame1, ID_BtnBuy, _("Buy"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer22->Add(itemButton23, 0, wxALIGN_CENTER_VERTICAL, 3);

  wxButton* itemButton24 = new wxButton( itemFrame1, ID_BtnSell, _("Sell"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer22->Add(itemButton24, 0, wxALIGN_CENTER_VERTICAL, 3);

  // Connect events and objects

  Bind( wxEVT_CLOSE_WINDOW, &FrameManualOrder::OnClose, this );

  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameManualOrder::OnBtnMarket, this, ID_BtnOrderTypeMarket );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameManualOrder::OnBtnLimit, this, ID_BtnOrderTypeLimit );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &FrameManualOrder::OnBtnStop, this, ID_BtnOrderTypeStop );

  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &FrameManualOrder::OnBtnBuy, this, ID_BtnBuy );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &FrameManualOrder::OnBtnSell, this, ID_BtnSell );
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

// need to set state on buttons sometime to make validations below unneeded

void FrameManualOrder::EmitOrder( void ) const {
  bool bOk = true;
  if ( 0 == m_order.nQuantity ) bOk = false;
  if ( 0 == m_order.sInstrument.length() ) bOk = false;
  if ( ou::tf::OrderType::Limit == m_order.eOrderType ) {
    if ( 0.0 == m_order.dblPrice1 ) bOk = false;
  }
  if ( bOk ) {
    if ( 0 != OnNewOrder ) 
      OnNewOrder( m_order );
  }
}
