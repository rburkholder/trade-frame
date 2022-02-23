/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    PanelOrderButtons.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFVuTrading
 * Created: February 11, 2022 13:20
 */

#include "PanelOrderButtons.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelOrderButtons::PanelOrderButtons() {
    Init();
}

PanelOrderButtons::PanelOrderButtons(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style
) {
    Init();
    Create(parent, id, pos, size, style);
}

bool PanelOrderButtons::Create(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style
){

    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer()) {
        GetSizer()->SetSizeHints(this);
    }
    //Centre();
    return true;
}

PanelOrderButtons::~PanelOrderButtons() {
}

void PanelOrderButtons::Init() {
    m_btnMarket = nullptr;
    m_btnLimit = nullptr;
    m_btnBracket = nullptr;
    m_btnBuy = nullptr;
    m_btnSell = nullptr;
    m_btnStopLong = nullptr;
    m_btnStopShort = nullptr;
    m_btnCancelAll = nullptr;
}

void PanelOrderButtons::CreateControls() {

    wxBoxSizer* sizerPanel = nullptr;
    wxBoxSizer* sizerBtnOrderType = nullptr;
    wxBoxSizer* sizerButtons = nullptr;

    PanelOrderButtons* itemPanel1 = this;

    sizerPanel = new wxBoxSizer(wxHORIZONTAL);
    itemPanel1->SetSizer(sizerPanel);

    sizerBtnOrderType = new wxBoxSizer(wxVERTICAL);
    sizerPanel->Add(sizerBtnOrderType, 0, wxALIGN_TOP|wxADJUST_MINSIZE, 2);

    m_btnMarket = new wxRadioButton( itemPanel1, ID_BtnMarket, _("Market"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnMarket->SetValue(false);
    sizerBtnOrderType->Add(m_btnMarket, 0, wxALIGN_LEFT|wxALL, 2);

    m_btnLimit = new wxRadioButton( itemPanel1, ID_BtnLimit, _("Limit"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnLimit->SetValue(true);
    sizerBtnOrderType->Add(m_btnLimit, 0, wxALIGN_LEFT|wxALL, 2);

    m_btnBracket = new wxRadioButton( itemPanel1, ID_BtnBracket, _("Bracket"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnBracket->SetValue(false);
    sizerBtnOrderType->Add(m_btnBracket, 0, wxALIGN_LEFT|wxALL, 2);

    sizerButtons = new wxBoxSizer(wxVERTICAL);
    sizerPanel->Add(sizerButtons, 0, wxALIGN_TOP|wxADJUST_MINSIZE, 1);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    sizerButtons->Add(itemBoxSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE, 2);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxALIGN_TOP|wxADJUST_MINSIZE, 1);

    m_btnBuy = new wxButton( itemPanel1, ID_BtnBuy, _("Buy"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(m_btnBuy, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnSell = new wxButton( itemPanel1, ID_BtnSell, _("Sell"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(m_btnSell, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(itemBoxSizer7, 0, wxALIGN_TOP|wxADJUST_MINSIZE, 1);

    m_btnStopLong = new wxButton( itemPanel1, ID_BtnStopLong, _("Stop Long"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer7->Add(m_btnStopLong, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnStopShort = new wxButton( itemPanel1, ID_BtnStopShort, _("Stop Short"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer7->Add(m_btnStopShort, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnCancelAll = new wxButton( itemPanel1, ID_BtnCancelAll, _("Cancel All"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerButtons->Add(m_btnCancelAll, 0, wxGROW|wxALL, 2);

  Bind( wxEVT_DESTROY, &PanelOrderButtons::OnDestroy, this );

  Bind( wxEVT_SET_FOCUS, &PanelOrderButtons::OnFocusChange, this );
  Bind( wxEVT_KILL_FOCUS, &PanelOrderButtons::OnFocusChange, this );

  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelOrderButtons::OnBtnMarketSelected, this, ID_BtnMarket );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelOrderButtons::OnBtnLimitSelected, this, ID_BtnLimit );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelOrderButtons::OnBtnBracketSelected, this, ID_BtnBracket );

  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOrderButtons::OnBtnBuyClick, this, ID_BtnBuy );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOrderButtons::OnBtnSellClick, this, ID_BtnSell );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOrderButtons::OnBtnStopLongClick, this, ID_BtnStopLong );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOrderButtons::OnBtnStopShortClick, this, ID_BtnStopShort );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOrderButtons::OnBtnCancelAllClick, this, ID_BtnCancelAll );

  m_OrderType = EOrderType::Limit;
}

void PanelOrderButtons::Set(
  fBtnOrder_t&& fBtnOrderBuy, // Buy
  fBtnOrder_t&& fBtnOrderSell, // Sell
  fBtnOrder_t&& fBtnOrderStopLong, // StopLong
  fBtnOrder_t&& fBtnOrderStopShort,  // StopShort
  fBtnOrder_t&& fBtnOrderCancelAll  // CancelAll
) {
  m_fBtnOrderBuy = std::move( fBtnOrderBuy );
  m_fBtnOrderSell = std::move( fBtnOrderSell );
  m_fBtnOrderStopLong = std::move( fBtnOrderStopLong );
  m_fBtnOrderStopShort = std::move( fBtnOrderStopShort );
  m_fBtnOrderCancelAll = std::move( fBtnOrderCancelAll );
}

void PanelOrderButtons::OnBtnMarketSelected( wxCommandEvent& event ) {
  m_btnMarket->SetValue( true );
  m_btnLimit->SetValue( false );
  m_btnBracket->SetValue( false );
  m_OrderType = EOrderType::Market;
  event.Skip();
}

void PanelOrderButtons::OnBtnLimitSelected( wxCommandEvent& event ) {
  m_btnMarket->SetValue( false );
  m_btnLimit->SetValue( true );
  m_btnBracket->SetValue( false );
  m_OrderType = EOrderType::Limit;
  event.Skip();
}

void PanelOrderButtons::OnBtnBracketSelected( wxCommandEvent& event ) {
  m_btnMarket->SetValue( false );
  m_btnLimit->SetValue( false );
  m_btnBracket->SetValue( true );
  m_OrderType = EOrderType::Bracket;
  event.Skip();
}

void PanelOrderButtons::OnBtnBuyClick( wxCommandEvent& event ) {
  if ( m_fBtnOrderBuy ) {
    wxColour colour = m_btnBuy->GetForegroundColour();
    m_btnBuy->SetForegroundColour( *wxGREEN );
    m_fBtnOrderBuy(
      m_OrderType,
      [this,colour](){ // fBtnDone_t
        m_btnBuy->SetForegroundColour( colour );
      } );
  }
  event.Skip();
}

void PanelOrderButtons::OnBtnSellClick( wxCommandEvent& event ) {
  if ( m_fBtnOrderSell ) {
    wxColour colour = m_btnSell->GetForegroundColour();
    m_btnSell->SetForegroundColour( *wxGREEN );
    m_fBtnOrderSell(
      m_OrderType,
      [this,colour](){ // fBtnDone_t
        m_btnSell->SetForegroundColour( colour );
      } );
  }
  event.Skip();
}

void PanelOrderButtons::OnBtnStopLongClick( wxCommandEvent& event ) {
  if ( m_fBtnOrderStopLong ) {
    wxColour colour = m_btnStopLong->GetForegroundColour();
    m_btnStopLong->SetForegroundColour( *wxGREEN );
    m_fBtnOrderStopLong(
      m_OrderType,
      [this,colour](){ // fBtnDone_t
        m_btnStopLong->SetForegroundColour( colour );
      } );
  }
  event.Skip();
}

void PanelOrderButtons::OnBtnStopShortClick( wxCommandEvent& event ) {
  if ( m_fBtnOrderStopShort ) {
    wxColour colour = m_btnStopShort->GetForegroundColour();
    m_btnStopShort->SetForegroundColour( *wxGREEN );
    m_fBtnOrderStopShort(
      m_OrderType,
      [this,colour](){ // fBtnDone_t
        m_btnStopShort->SetForegroundColour( colour );
      } );
  }
  event.Skip();
}

void PanelOrderButtons::OnBtnCancelAllClick( wxCommandEvent& event ) {
  if ( m_fBtnOrderCancelAll ) {
    wxColour colour = m_btnCancelAll->GetForegroundColour();
    m_btnCancelAll->SetForegroundColour( *wxGREEN );
    m_fBtnOrderCancelAll(
      m_OrderType,
      [this,colour](){ // fBtnDone_t
        m_btnCancelAll->SetForegroundColour( colour );
      } );
  }
  event.Skip();
}

void PanelOrderButtons::OnFocusChange( wxFocusEvent& event ) {
  event.Skip();
}

void PanelOrderButtons::OnDestroy( wxWindowDestroyEvent& event ) {
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  event.Skip();  // auto followed by Destroy();
}

} // namespace tf
} // namespace ou
