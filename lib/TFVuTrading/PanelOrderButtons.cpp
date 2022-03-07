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

#include <wx/listctrl.h>

#include "PanelOrderButtons.h"

enum class EOrderType { Market=0, Limit=1, Bracket=2, Stop=3 };

namespace {
  ou::tf::OrderType::enumOrderType LUOrderType( int id ) {
    EOrderType ot = (EOrderType) id;
    switch ( ot ) {
      case EOrderType::Bracket:
        return ou::tf::OrderType::Scale;
        break;
      case EOrderType::Limit:
        return ou::tf::OrderType::Limit;
        break;
      case EOrderType::Market:
        return ou::tf::OrderType::Market;
        break;
      case EOrderType::Stop:
        return ou::tf::OrderType::Stop;
        break;
      default:
        return ou::tf::OrderType::Unknown;
        break;
    }
  }

}

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
    m_radioOrderType = nullptr;
    m_radioInstrument = nullptr;
    m_btnBuy = nullptr;
    m_btnSell = nullptr;
    m_btnCancelAll = nullptr;
    m_listPositions = nullptr;
    m_listOrders = nullptr;
}

void PanelOrderButtons::CreateControls() {

    wxBoxSizer* sizerMain = nullptr;
    wxBoxSizer* sizerButtons = nullptr;
    wxBoxSizer* sizerRadioButtons = nullptr;
    wxBoxSizer* sizerOrderButtons = nullptr;
    wxBoxSizer* sizerPositions = nullptr;
    wxBoxSizer* sizerOrders = nullptr;

    PanelOrderButtons* itemPanel1 = this;

    sizerMain = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(sizerMain);

    sizerButtons = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerButtons, 0, 0, 0);

    sizerRadioButtons = new wxBoxSizer(wxVERTICAL);
    sizerButtons->Add(sizerRadioButtons, 0, wxALIGN_CENTER_VERTICAL, 2);

    wxArrayString m_radioOrderTypeStrings;
    m_radioOrderTypeStrings.Add(_("&Market"));
    m_radioOrderTypeStrings.Add(_("&Limit"));
    m_radioOrderTypeStrings.Add(_("&Bracket"));
    m_radioOrderTypeStrings.Add(_("&Stop"));
    m_radioOrderType = new wxRadioBox( itemPanel1, ID_RadioOrderType, _("Order Type"), wxDefaultPosition, wxDefaultSize, m_radioOrderTypeStrings, 1, wxRA_SPECIFY_ROWS );
    m_radioOrderType->SetSelection(0);
    sizerRadioButtons->Add(m_radioOrderType, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    wxArrayString m_radioInstrumentStrings;
    m_radioInstrumentStrings.Add(_("&Underlying"));
    m_radioInstrumentStrings.Add(_("&Call"));
    m_radioInstrumentStrings.Add(_("&Put"));
    m_radioInstrument = new wxRadioBox( itemPanel1, ID_RadioInstrument, _("Instrument"), wxDefaultPosition, wxDefaultSize, m_radioInstrumentStrings, 1, wxRA_SPECIFY_ROWS );
    m_radioInstrument->SetSelection(0);
    sizerRadioButtons->Add(m_radioInstrument, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    sizerOrderButtons = new wxBoxSizer(wxVERTICAL);
    sizerButtons->Add(sizerOrderButtons, 0, wxALIGN_CENTER_VERTICAL, 2);

    m_btnBuy = new wxButton( itemPanel1, ID_BtnBuy, _("Buy"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerOrderButtons->Add(m_btnBuy, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnSell = new wxButton( itemPanel1, ID_BtnSell, _("Sell"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerOrderButtons->Add(m_btnSell, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnCancelAll = new wxButton( itemPanel1, ID_BtnCancelAll, _("Cancel All"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerOrderButtons->Add(m_btnCancelAll, 0, wxGROW|wxALL, 2);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    sizerButtons->Add(itemBoxSizer4, 1, wxGROW, 2);

    sizerPositions = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerPositions, 1, wxGROW, 2);

    m_listPositions = new wxListCtrl( itemPanel1, ID_ListPositions, wxDefaultPosition, wxDefaultSize, wxLC_LIST |wxLC_SINGLE_SEL|wxSIMPLE_BORDER );
    m_listPositions->SetName(wxT("Positions"));
    sizerPositions->Add(m_listPositions, 1, wxGROW|wxALL, 2);

    sizerOrders = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerOrders, 1, wxGROW, 2);

    m_listOrders = new wxListCtrl( itemPanel1, ID_ListOrders, wxDefaultPosition, wxDefaultSize, wxLC_LIST |wxLC_SINGLE_SEL|wxSIMPLE_BORDER );
    m_listOrders->SetName(wxT("Orders"));
    sizerOrders->Add(m_listOrders, 1, wxGROW|wxALL, 2);

    // Connect events and objects
    //m_listPositions->Connect(ID_ListPositions, wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(PanelOrderButtons::OnContextMenu), NULL, this);
    //m_listOrders->Connect(ID_ListOrders, wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(PanelOrderButtons::OnContextMenu), NULL, this);

  Bind( wxEVT_DESTROY, &PanelOrderButtons::OnDestroy, this );

  Bind( wxEVT_SET_FOCUS, &PanelOrderButtons::OnFocusChange, this );
  Bind( wxEVT_KILL_FOCUS, &PanelOrderButtons::OnFocusChange, this );

  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOrderButtons::OnBtnBuyClick, this, ID_BtnBuy );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOrderButtons::OnBtnSellClick, this, ID_BtnSell );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOrderButtons::OnBtnCancelAllClick, this, ID_BtnCancelAll );

  Bind( wxEVT_RADIOBOX, &PanelOrderButtons::OnRadioOrderTypeClick, this, ID_RadioOrderType );
  Bind( wxEVT_RADIOBOX, &PanelOrderButtons::OnRadioInstrumentClick, this, ID_RadioInstrument );

}

void PanelOrderButtons::Set(
  fBtnOrder_t&& fBtnOrderBuy, // Buy
  fBtnOrder_t&& fBtnOrderSell, // Sell
  fBtnOrder_t&& fBtnOrderCancelAll  // CancelAll
) {
  m_fBtnOrderBuy = std::move( fBtnOrderBuy );
  m_fBtnOrderSell = std::move( fBtnOrderSell );
  m_fBtnOrderCancelAll = std::move( fBtnOrderCancelAll );
}

void PanelOrderButtons::OnRadioOrderTypeClick( wxCommandEvent& event ) {
  std::string s( event.GetString() );
  std::cout << "order: " << s << "," << event.GetSelection() << std::endl;
}

void PanelOrderButtons::OnRadioInstrumentClick( wxCommandEvent& event ) {
  std::string s( event.GetString() );
  std::cout << "instrument: " << s << "," << event.GetSelection() << std::endl;
}


void PanelOrderButtons::OnBtnBuyClick( wxCommandEvent& event ) {
  if ( m_fBtnOrderBuy ) {
    wxColour colour = m_btnBuy->GetForegroundColour();
    m_btnBuy->SetForegroundColour( *wxGREEN );
    m_fBtnOrderBuy(
      LUOrderType( m_radioOrderType->GetSelection() ),
      (EInstrumentType)m_radioInstrument->GetSelection(),
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
      LUOrderType( m_radioOrderType->GetSelection() ),
      (EInstrumentType)m_radioInstrument->GetSelection(),
      [this,colour](){ // fBtnDone_t
        m_btnSell->SetForegroundColour( colour );
      } );
  }
  event.Skip();
}

void PanelOrderButtons::OnBtnCancelAllClick( wxCommandEvent& event ) {
  if ( m_fBtnOrderCancelAll ) {
    wxColour colour = m_btnCancelAll->GetForegroundColour();
    m_btnCancelAll->SetForegroundColour( *wxGREEN );
    m_fBtnOrderCancelAll(
      LUOrderType( m_radioOrderType->GetSelection() ),
      (EInstrumentType)m_radioInstrument->GetSelection(),
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
