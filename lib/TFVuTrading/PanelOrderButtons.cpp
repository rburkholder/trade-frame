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

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/radiobox.h>
#include <wx/stattext.h>

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
}

void PanelOrderButtons::CreateControls() {

    wxBoxSizer* sizerMain = nullptr;
    wxBoxSizer* sizerCockForCursor = nullptr;
    wxBoxSizer* sizerQuantity = nullptr;
    wxBoxSizer* sizerQuanStock = nullptr;
    wxBoxSizer* sizerQuanFuture = nullptr;
    wxBoxSizer* sizerQuanOption = nullptr;
    wxBoxSizer* sizerPositionEntry = nullptr;
    wxBoxSizer* sizerPositionExitProfit = nullptr;
    wxBoxSizer* sizerPositionExitStop = nullptr;
    wxBoxSizer* sizerBtnOrderTypes = nullptr;
    wxBoxSizer* sizerMarketData = nullptr;
    wxBoxSizer* sizerMDBase = nullptr;
    wxBoxSizer* sizerMDCall1 = nullptr;
    wxBoxSizer* sizerMDPut1 = nullptr;
    wxBoxSizer* sizerMDCall2 = nullptr;
    wxBoxSizer* sizerMDPut2 = nullptr;
    wxBoxSizer* sizerStochastic = nullptr;

    wxStaticText* labelQuantity;

    PanelOrderButtons* itemPanel1 = this;

    sizerMain = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(sizerMain);

    sizerCockForCursor = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerCockForCursor, 0, wxGROW, 2);

    m_cbCockForCursor = new wxCheckBox( itemPanel1, ID_CB_CockForCursor, _("Cock for Cursor"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbCockForCursor->SetValue(false);
    if (PanelOrderButtons::ShowToolTips())
        m_cbCockForCursor->SetToolTip(_("Price point by mouse"));
    sizerCockForCursor->Add(m_cbCockForCursor, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    sizerQuantity = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerQuantity, 0, wxGROW|wxLEFT|wxRIGHT, 2);

    labelQuantity = new wxStaticText( itemPanel1, wxID_STATIC, _("Quantity:"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerQuantity->Add(labelQuantity, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    sizerQuanStock = new wxBoxSizer(wxHORIZONTAL);
    sizerQuantity->Add(sizerQuanStock, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_txtQuanStock = new wxTextCtrl( itemPanel1, ID_TXT_QuanStock, _("100"), wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT );
    sizerQuanStock->Add(m_txtQuanStock, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    wxStaticText* itemStaticText7 = new wxStaticText( itemPanel1, wxID_STATIC, _("Stock"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerQuanStock->Add(itemStaticText7, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    wxStaticLine* itemStaticLine8 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL|wxNO_BORDER );
    sizerQuantity->Add(itemStaticLine8, 0, wxGROW|wxLEFT|wxRIGHT, 2);

    sizerQuanFuture = new wxBoxSizer(wxHORIZONTAL);
    sizerQuantity->Add(sizerQuanFuture, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_txtQuanFuture = new wxTextCtrl( itemPanel1, ID_TXT_QuanFuture, _("1"), wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT );
    sizerQuanFuture->Add(m_txtQuanFuture, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel1, wxID_STATIC, _("Future"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerQuanFuture->Add(itemStaticText11, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    wxStaticLine* itemStaticLine12 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL|wxNO_BORDER );
    sizerQuantity->Add(itemStaticLine12, 0, wxGROW|wxLEFT|wxRIGHT, 2);

    sizerQuanOption = new wxBoxSizer(wxHORIZONTAL);
    sizerQuantity->Add(sizerQuanOption, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_txtQuanOption = new wxTextCtrl( itemPanel1, ID_TXT_QuanOption, _("1"), wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT );
    sizerQuanOption->Add(m_txtQuanOption, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    wxStaticText* itemStaticText15 = new wxStaticText( itemPanel1, wxID_STATIC, _("Option"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerQuanOption->Add(itemStaticText15, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    sizerPositionEntry = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerPositionEntry, 1, wxGROW, 2);

    m_cbEnablePositionEntry = new wxCheckBox( itemPanel1, ID_CB_PositionEntry, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_cbEnablePositionEntry->SetValue(true);
    m_cbEnablePositionEntry->Enable(false);
    sizerPositionEntry->Add(m_cbEnablePositionEntry, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 2);

    m_txtPricePositionEntry = new wxTextCtrl( itemPanel1, ID_TXT_PositionEntry, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    sizerPositionEntry->Add(m_txtPricePositionEntry, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 2);

    wxArrayString m_radioPositionEntryStrings;
    m_radioPositionEntryStrings.Add(_("&Market"));
    m_radioPositionEntryStrings.Add(_("&Limit"));
    m_radioPositionEntryStrings.Add(_("&LmtTO"));
    m_radioPositionEntryStrings.Add(_("&Stoch"));
    m_radioPositionEntry = new wxRadioBox( itemPanel1, ID_RADIO_PositionEntry, _("Position Entry"), wxDefaultPosition, wxDefaultSize, m_radioPositionEntryStrings, 1, wxRA_SPECIFY_ROWS );
    m_radioPositionEntry->SetSelection(0);
    if (PanelOrderButtons::ShowToolTips())
        m_radioPositionEntry->SetToolTip(_("Market\nLimit\nLimit with Timeout\nStochastic"));
    sizerPositionEntry->Add(m_radioPositionEntry, 1, wxGROW|wxLEFT, 2);

    sizerPositionExitProfit = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerPositionExitProfit, 1, wxGROW, 2);

    m_cbEnableProfitExit = new wxCheckBox( itemPanel1, ID_CB_PositionExitProfit, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_cbEnableProfitExit->SetValue(false);
    sizerPositionExitProfit->Add(m_cbEnableProfitExit, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 2);

    m_txtPriceProfitExit = new wxTextCtrl( itemPanel1, ID_TXT_PositionExitProfit, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    sizerPositionExitProfit->Add(m_txtPriceProfitExit, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 2);

    wxArrayString m_radioExitProfitStrings;
    m_radioExitProfitStrings.Add(_("&Rel"));
    m_radioExitProfitStrings.Add(_("&Abs"));
    m_radioExitProfitStrings.Add(_("St&och"));
    m_radioExitProfit = new wxRadioBox( itemPanel1, ID_RADIO_PositionExitProfit, _("Profit Exit"), wxDefaultPosition, wxDefaultSize, m_radioExitProfitStrings, 1, wxRA_SPECIFY_ROWS );
    m_radioExitProfit->SetSelection(0);
    if (PanelOrderButtons::ShowToolTips())
        m_radioExitProfit->SetToolTip(_("Relative\nAbsolute\nStochastic"));
    sizerPositionExitProfit->Add(m_radioExitProfit, 1, wxGROW|wxLEFT, 2);

    sizerPositionExitStop = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerPositionExitStop, 1, wxGROW|wxBOTTOM, 2);

    m_cbEnableStopExit = new wxCheckBox( itemPanel1, ID_CB_PositionExitStop, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_cbEnableStopExit->SetValue(false);
    sizerPositionExitStop->Add(m_cbEnableStopExit, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 2);

    m_txtPriceStopExit = new wxTextCtrl( itemPanel1, ID_TXT_PositionExitStop, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    sizerPositionExitStop->Add(m_txtPriceStopExit, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 2);

    wxArrayString m_radioExitStopStrings;
    m_radioExitStopStrings.Add(_("Tr&l"));
    m_radioExitStopStrings.Add(_("Trl &Pct"));
    m_radioExitStopStrings.Add(_("St&op"));
    m_radioExitStop = new wxRadioBox( itemPanel1, ID_RADIO_PositionExitTop, _("Stop Exit"), wxDefaultPosition, wxDefaultSize, m_radioExitStopStrings, 1, wxRA_SPECIFY_ROWS );
    m_radioExitStop->SetSelection(0);
    if (PanelOrderButtons::ShowToolTips())
        m_radioExitStop->SetToolTip(_("Trailing Absolute\nTrailing Percent\nStop"));
    sizerPositionExitStop->Add(m_radioExitStop, 1, wxGROW|wxLEFT, 2);

    wxStaticLine* itemStaticLine1 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    sizerMain->Add(itemStaticLine1, 0, wxGROW|wxTOP|wxBOTTOM, 2);

    sizerBtnOrderTypes = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerBtnOrderTypes, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    m_btnBuy = new wxButton( itemPanel1, ID_BtnBuy, _("&Buy"), wxDefaultPosition, wxSize(60, -1), 0 );
    sizerBtnOrderTypes->Add(m_btnBuy, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

    m_btnSell = new wxButton( itemPanel1, ID_BtnSell, _("&Sell"), wxDefaultPosition, wxSize(60, -1), 0 );
    sizerBtnOrderTypes->Add(m_btnSell, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

    m_btnCancel = new wxButton( itemPanel1, ID_BtnCancel, _("C&ancel"), wxDefaultPosition, wxSize(60, -1), 0 );
    m_btnCancel->Enable(false);
    sizerBtnOrderTypes->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

    m_btnClose = new wxButton( itemPanel1, ID_BtnClose, _("&Close"), wxDefaultPosition, wxSize(60, -1), 0 );
    m_btnClose->Enable(false);
    sizerBtnOrderTypes->Add(m_btnClose, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

    wxStaticLine* itemStaticLine9 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    sizerMain->Add(itemStaticLine9, 0, wxGROW|wxTOP|wxBOTTOM, 2);

    wxArrayString m_radioInstrumentStrings;
    m_radioInstrumentStrings.Add(_("&Base"));
    m_radioInstrumentStrings.Add(_("&Call1"));
    m_radioInstrumentStrings.Add(_("&Put1"));
    m_radioInstrumentStrings.Add(_("&Call2"));
    m_radioInstrumentStrings.Add(_("&Put2"));
    m_radioInstrument = new wxRadioBox( itemPanel1, ID_RADIO_Instrument, _("Instrument"), wxDefaultPosition, wxDefaultSize, m_radioInstrumentStrings, 1, wxRA_SPECIFY_ROWS );
    m_radioInstrument->SetSelection(0);
    sizerMain->Add(m_radioInstrument, 1, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT, 2);

    wxStaticLine* itemStaticLine11 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    sizerMain->Add(itemStaticLine11, 0, wxGROW|wxTOP|wxBOTTOM, 2);

    sizerMarketData = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerMarketData, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 2);

    sizerMDBase = new wxBoxSizer(wxVERTICAL);
    sizerMarketData->Add(sizerMDBase, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtBase = new wxStaticText( itemPanel1, ID_TXT_Base, _("Base"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerMDBase->Add(m_txtBase, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    m_txtBaseAsk = new wxStaticText( itemPanel1, ID_TXT_BaseAsk, _("ask"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerMDBase->Add(m_txtBaseAsk, 0, wxALIGN_RIGHT|wxRIGHT, 2);

    m_txtBaseBid = new wxStaticText( itemPanel1, ID_TXT_BaseBid, _("bid"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerMDBase->Add(m_txtBaseBid, 0, wxALIGN_RIGHT|wxRIGHT, 2);

    sizerMDCall1 = new wxBoxSizer(wxVERTICAL);
    sizerMarketData->Add(sizerMDCall1, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtCall1 = new wxStaticText( itemPanel1, ID_TXT_Call1, _("Call1"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerMDCall1->Add(m_txtCall1, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    m_txtCall1Ask = new wxStaticText( itemPanel1, ID_TXT_Call1Ask, _("ask"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerMDCall1->Add(m_txtCall1Ask, 0, wxALIGN_RIGHT|wxRIGHT, 2);

    m_txtCall1Bid = new wxStaticText( itemPanel1, ID_TXT_Call1Bid, _("bid"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerMDCall1->Add(m_txtCall1Bid, 0, wxALIGN_RIGHT|wxRIGHT, 2);

    sizerMDPut1 = new wxBoxSizer(wxVERTICAL);
    sizerMarketData->Add(sizerMDPut1, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtPut1 = new wxStaticText( itemPanel1, ID_TXT_Put1, _("Put1"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerMDPut1->Add(m_txtPut1, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    m_txtPut1Ask = new wxStaticText( itemPanel1, ID_TXT_Put1Ask, _("ask"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerMDPut1->Add(m_txtPut1Ask, 0, wxALIGN_RIGHT|wxRIGHT, 2);

    m_txtPut1Bid = new wxStaticText( itemPanel1, ID_TXT_Put1Bid, _("bid"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerMDPut1->Add(m_txtPut1Bid, 0, wxALIGN_RIGHT|wxRIGHT, 2);

    sizerMDCall2 = new wxBoxSizer(wxVERTICAL);
    sizerMarketData->Add(sizerMDCall2, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtCall2 = new wxStaticText( itemPanel1, ID_TXT_Call2, _("Call2"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerMDCall2->Add(m_txtCall2, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    m_txtCall2Ask = new wxStaticText( itemPanel1, ID_TXT_Call2Ask, _("ask"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerMDCall2->Add(m_txtCall2Ask, 0, wxALIGN_RIGHT|wxRIGHT, 2);

    m_txtCall2Bid = new wxStaticText( itemPanel1, ID_TXT_Call2Bid, _("bid"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerMDCall2->Add(m_txtCall2Bid, 0, wxALIGN_RIGHT|wxRIGHT, 2);

    sizerMDPut2 = new wxBoxSizer(wxVERTICAL);
    sizerMarketData->Add(sizerMDPut2, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtPut2 = new wxStaticText( itemPanel1, ID_TXT_Put2, _("Put2"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerMDPut2->Add(m_txtPut2, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    m_txtPut2Ask = new wxStaticText( itemPanel1, ID_TXT_Put2Ask, _("ask"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerMDPut2->Add(m_txtPut2Ask, 0, wxALIGN_RIGHT|wxRIGHT, 2);

    m_txtPut2Bid = new wxStaticText( itemPanel1, ID_TXT_Put2Bid, _("bid"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerMDPut2->Add(m_txtPut2Bid, 0, wxALIGN_RIGHT|wxRIGHT, 2);

    wxStaticLine* itemStaticLine35 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    sizerMain->Add(itemStaticLine35, 0, wxGROW|wxTOP|wxBOTTOM, 2);

    sizerStochastic = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerStochastic, 1, wxALIGN_CENTER_HORIZONTAL, 2);

    m_cbEnableStoch1 = new wxCheckBox( itemPanel1, ID_CB_Stoch1, _("Stoch 1"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbEnableStoch1->SetValue(false);
    sizerStochastic->Add(m_cbEnableStoch1, 1, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

    m_cbEnableStoch2 = new wxCheckBox( itemPanel1, ID_CB_Stoch2, _("Stoch 2"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbEnableStoch2->SetValue(false);
    sizerStochastic->Add(m_cbEnableStoch2, 1, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_cbEnableStoch3 = new wxCheckBox( itemPanel1, ID_CB_Stoch3, _("Stoch 3"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbEnableStoch3->SetValue(false);
    sizerStochastic->Add(m_cbEnableStoch3, 1, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

  Bind( wxEVT_DESTROY, &PanelOrderButtons::OnDestroy, this );

  Bind( wxEVT_SET_FOCUS, &PanelOrderButtons::OnFocusChange, this );
  Bind( wxEVT_KILL_FOCUS, &PanelOrderButtons::OnFocusChange, this );

  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOrderButtons::OnBtnBuyClick, this, ID_BtnBuy );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOrderButtons::OnBtnSellClick, this, ID_BtnSell );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOrderButtons::OnBtnCloseClick, this, ID_BtnClose );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOrderButtons::OnBtnCancelClick, this, ID_BtnCancel );

  Bind( wxEVT_RADIOBOX, &PanelOrderButtons::OnRADIOPositionEntrySelected, this, ID_RADIO_PositionEntry );
  Bind( wxEVT_RADIOBOX, &PanelOrderButtons::OnRADIOInstrumentSelected, this, ID_RADIO_Instrument );
  Bind( wxEVT_RADIOBOX, &PanelOrderButtons::OnRADIOPositionExitProfitSelected, this, ID_RADIO_PositionExitProfit );
  Bind( wxEVT_RADIOBOX, &PanelOrderButtons::OnRADIOPositionExitTopSelected, this, ID_RADIO_PositionExitTop );

  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelOrderButtons::OnCBCockForCursorClick, this, ID_CB_CockForCursor );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelOrderButtons::OnCBPositionEntryClick, this, ID_CB_PositionEntry );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelOrderButtons::OnCBPositionExitProfitClick, this, ID_CB_PositionExitProfit );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelOrderButtons::OnCBPositionExitStopClick, this, ID_CB_PositionExitStop );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelOrderButtons::OnCBStoch1Click, this, ID_CB_Stoch1 );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelOrderButtons::OnCBStoch2Click, this, ID_CB_Stoch2 );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelOrderButtons::OnCBStoch3Click, this, ID_CB_Stoch3 );

  Bind( wxEVT_COMMAND_TEXT_UPDATED, &PanelOrderButtons::OnTXTPositionEntryTextUpdated, this, ID_TXT_PositionEntry );
  Bind( wxEVT_COMMAND_TEXT_UPDATED, &PanelOrderButtons::OnTXTPositionExitProfitTextUpdated, this, ID_TXT_PositionExitProfit );
  Bind( wxEVT_COMMAND_TEXT_UPDATED, &PanelOrderButtons::OnTXTPositionExitStopTextUpdated, this, ID_TXT_PositionExitStop );

  Bind( wxEVT_COMMAND_TEXT_UPDATED, &PanelOrderButtons::OnTXTQuanStockTextUpdated, this, ID_TXT_QuanStock );
  Bind( wxEVT_COMMAND_TEXT_UPDATED, &PanelOrderButtons::OnTXTQuanFutureTextUpdated, this, ID_TXT_QuanFuture );
  Bind( wxEVT_COMMAND_TEXT_UPDATED, &PanelOrderButtons::OnTXTQuanOptionTextUpdated, this, ID_TXT_QuanOption );

}

void PanelOrderButtons::Set(
  fBtnOrder_t&& fBtnOrderBuy, // Buy
  fBtnOrder_t&& fBtnOrderSell, // Sell
  fBtnOrder_t&& fBtnOrderClose, // Sell
  fBtnOrder_t&& fBtnOrderCancel  // CancelAll
) {
  m_fBtnOrderBuy = std::move( fBtnOrderBuy );
  m_fBtnOrderSell = std::move( fBtnOrderSell );
  m_fBtnOrderClose = std::move( fBtnOrderClose );
  m_fBtnOrderCancel = std::move( fBtnOrderCancel );
}

void PanelOrderButtons::Update( const PanelOrderButtons_MarketData& data ) {

  m_txtBase->SetLabel( data.m_sBase );
  m_txtBaseAsk->SetLabel( data.m_sBaseAsk );
  m_txtBaseBid->SetLabel( data.m_sBaseBid );

  m_txtCall1->SetLabel( data.m_sCall1 );
  m_txtCall1Ask->SetLabel( data.m_sCall1Ask );
  m_txtCall1Bid->SetLabel( data.m_sCall1Bid );

  m_txtPut1->SetLabel( data.m_sPut1 );
  m_txtPut1Ask->SetLabel( data.m_sPut1Ask );
  m_txtPut1Bid->SetLabel( data.m_sPut1Bid );

  m_txtCall2->SetLabel( data.m_sCall2 );
  m_txtCall2Ask->SetLabel( data.m_sCall2Ask );
  m_txtCall2Bid->SetLabel( data.m_sCall2Bid );

  m_txtPut2->SetLabel( data.m_sPut2 );
  m_txtPut2Ask->SetLabel( data.m_sPut2Ask );
  m_txtPut2Bid->SetLabel( data.m_sPut2Bid );

}

bool PanelOrderButtons::ValidateFields() {
  bool bOk( true );
  if ( m_order.m_bPositionEntryEnable ) {
    if ( PanelOrderButtons_Order::EPositionEntryMethod::LimitOnly == m_order.m_ePositionEntryMethod ) {
      if ( 0 == m_order.m_sPositionEntryValue.size() ) {
        std::cout << "position entry: requires value" << std::endl;
        bOk = false;
      }
    }
  }
  if ( m_order.m_bPositionEntryEnable ) {
    if ( PanelOrderButtons_Order::EPositionEntryMethod::LimitTimeOut == m_order.m_ePositionEntryMethod ) {
      if ( 0 == m_order.m_sPositionEntryValue.size() ) {
        std::cout << "position entry: requires value" << std::endl;
        bOk = false;
      }
    }
  }
  if ( m_order.m_bPositionExitProfitEnable ) {
    if ( PanelOrderButtons_Order::EPositionExitProfitMethod::Absolute == m_order.m_ePositionExitProfitMethod ) {
      if ( 0 == m_order.m_sPositionExitProfitValue.size() ) {
        std::cout << "position exit profit (abs): requires value" << std::endl;
        bOk = false;
      }
    }
    if ( PanelOrderButtons_Order::EPositionExitProfitMethod::Relative == m_order.m_ePositionExitProfitMethod ) {
      if ( 0 == m_order.m_sPositionExitProfitValue.size() ) {
        std::cout << "position exit profit (rel): requires value" << std::endl;
        bOk = false;
      }
    }
  }
  if ( m_order.m_bPositionExitStopEnable ) {
    if ( 0 == m_order.m_sPositionExitStopValue.size() ) {
      std::cout << "position exit stop: requires value" << std::endl;
      bOk = false;
    }
  }
  if ( PanelOrderButtons_Order::EPositionEntryMethod::Stoch == m_order.m_ePositionEntryMethod ) {
    if ( m_order.m_bStochastic1 | m_order.m_bStochastic2 | m_order.m_bStochastic3 ) {}
    else {
      std::cout << "position entry (stochastic): requires at least one" << std::endl;
      bOk = false;
    }
  }
  if ( PanelOrderButtons_Order::EPositionExitProfitMethod::Stoch == m_order.m_ePositionExitProfitMethod ) {
    if ( m_order.m_bStochastic1 | m_order.m_bStochastic2 | m_order.m_bStochastic3 ) {}
    else {
      std::cout << "position exit profit (stochastic): requires at least one" << std::endl;
      bOk = false;
    }
  }
  return bOk;
}

void PanelOrderButtons::OnBtnBuyClick( wxCommandEvent& event ) {
  if ( ValidateFields() ) {
    if ( m_fBtnOrderBuy ) {
      wxColour colour = m_btnBuy->GetForegroundColour();
      m_btnBuy->SetForegroundColour( *wxGREEN );
      m_fBtnOrderBuy(
        m_order,
        [this,colour](){ // fBtnDone_t
          m_btnBuy->SetForegroundColour( colour );
        } );
    }
  }
  event.Skip();
}

void PanelOrderButtons::OnBtnSellClick( wxCommandEvent& event ) {
  if ( ValidateFields() ) {
    if ( m_fBtnOrderSell ) {
      wxColour colour = m_btnSell->GetForegroundColour();
      m_btnSell->SetForegroundColour( *wxGREEN );
      m_fBtnOrderSell(
        m_order,
        [this,colour](){ // fBtnDone_t
          m_btnSell->SetForegroundColour( colour );
        } );
    }
  }
  event.Skip();
}

void PanelOrderButtons::OnBtnCloseClick( wxCommandEvent& event ) {
  //if ( ValidateFields() ) { // need this?
    if ( m_fBtnOrderClose ) {
      wxColour colour = m_btnClose->GetForegroundColour();
      m_btnClose->SetForegroundColour( *wxGREEN );
      m_fBtnOrderClose(
        m_order,
        [this,colour](){ // fBtnDone_t
          m_btnClose->SetForegroundColour( colour );
        } );
    }
  //}
  event.Skip();
}

void PanelOrderButtons::OnBtnCancelClick( wxCommandEvent& event ) {
  if ( m_fBtnOrderCancel ) {
    wxColour colour = m_btnCancel->GetForegroundColour();
    m_btnCancel->SetForegroundColour( *wxGREEN );
    m_fBtnOrderCancel(
      m_order,
      [this,colour](){ // fBtnDone_t
        m_btnCancel->SetForegroundColour( colour );
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

// == Cock For Cursor

void PanelOrderButtons::OnCBCockForCursorClick( wxCommandEvent& event ) {
  m_order.m_bCockForCursor = m_cbCockForCursor->IsChecked();
  event.Skip();
}

// == Position Entry

void PanelOrderButtons::OnCBPositionEntryClick( wxCommandEvent& event ) {
  m_order.m_bPositionEntryEnable = m_cbEnablePositionEntry->IsChecked();
  event.Skip();
}

void PanelOrderButtons::OnTXTPositionEntryTextUpdated( wxCommandEvent& event ) {
  m_order.m_sPositionEntryValue = m_txtPricePositionEntry->GetValue();
  event.Skip();
}

void PanelOrderButtons::OnRADIOPositionEntrySelected( wxCommandEvent& event ) {
  //std::string s( event.GetString() );
  //std::cout << "order: " << s << "," << event.GetSelection() << std::endl;
  switch( m_radioPositionEntry->GetSelection() ) {
    case 0:
      m_order.m_ePositionEntryMethod = PanelOrderButtons_Order::EPositionEntryMethod::Market;
      break;
    case 1:
      m_order.m_ePositionEntryMethod = PanelOrderButtons_Order::EPositionEntryMethod::LimitOnly;
      break;
    case 2:
      m_order.m_ePositionEntryMethod = PanelOrderButtons_Order::EPositionEntryMethod::LimitTimeOut;
      break;
    case 3:
      m_order.m_ePositionEntryMethod = PanelOrderButtons_Order::EPositionEntryMethod::Stoch;
      break;
  }
  event.Skip();
}

// == Position Exit for Profit

void PanelOrderButtons::OnCBPositionExitProfitClick( wxCommandEvent& event ) {
  event.Skip();
  m_order.m_bPositionExitProfitEnable = m_cbEnableProfitExit->IsChecked();
  event.Skip();
}

void PanelOrderButtons::OnTXTPositionExitProfitTextUpdated( wxCommandEvent& event ) {
  m_order.m_sPositionExitProfitValue = m_txtPriceProfitExit->GetValue();
  event.Skip();
}

void PanelOrderButtons::OnRADIOPositionExitProfitSelected( wxCommandEvent& event ) {
  switch ( m_radioExitProfit->GetSelection() ) {
    case 0:
      m_order.m_ePositionExitProfitMethod = PanelOrderButtons_Order::EPositionExitProfitMethod::Relative;
      break;
    case 1:
      m_order.m_ePositionExitProfitMethod = PanelOrderButtons_Order::EPositionExitProfitMethod::Absolute;
      break;
    case 2:
      m_order.m_ePositionExitProfitMethod = PanelOrderButtons_Order::EPositionExitProfitMethod::Stoch;
  }
  event.Skip();
}

// == Position Exit with Stop

void PanelOrderButtons::OnCBPositionExitStopClick( wxCommandEvent& event ) {
  m_order.m_bPositionExitStopEnable = m_cbEnableStopExit->IsChecked();
  event.Skip();
}

void PanelOrderButtons::OnTXTPositionExitStopTextUpdated( wxCommandEvent& event ) {
  m_order.m_sPositionExitStopValue = m_txtPriceStopExit->GetValue();
  event.Skip();
}

void PanelOrderButtons::OnRADIOPositionExitTopSelected( wxCommandEvent& event ) {
  switch( m_radioExitStop->GetSelection() ) {
    case 0:
      m_order.m_ePositionExitStopMethod = PanelOrderButtons_Order::EPositionExitStopMethod::TrailingAbsolute;
      break;
    case 1:
      m_order.m_ePositionExitStopMethod = PanelOrderButtons_Order::EPositionExitStopMethod::TrailingPercent;
      break;
    case 2:
      m_order.m_ePositionExitStopMethod = PanelOrderButtons_Order::EPositionExitStopMethod::Stop;
      break;
  }
  event.Skip();
}

// == Instrument

void PanelOrderButtons::OnRADIOInstrumentSelected( wxCommandEvent& event ) {
  //std::string s( event.GetString() );
  //std::cout << "instrument: " << s << "," << event.GetSelection() << std::endl;
  switch ( m_radioInstrument->GetSelection() ) {
    case 0:
      m_order.m_eInstrument = PanelOrderButtons_Order::EInstrument::Underlying;
      break;
    case 1:
      m_order.m_eInstrument = PanelOrderButtons_Order::EInstrument::Call1;
      break;
    case 2:
      m_order.m_eInstrument = PanelOrderButtons_Order::EInstrument::Put1;
      break;
    case 3:
      m_order.m_eInstrument = PanelOrderButtons_Order::EInstrument::Call2;
      break;
    case 4:
      m_order.m_eInstrument = PanelOrderButtons_Order::EInstrument::Put2;
      break;
  }
  event.Skip();
}

// == Stochastic selection

void PanelOrderButtons::OnCBStoch1Click( wxCommandEvent& event ) {
  m_order.m_bStochastic1 = m_cbEnableStoch1->IsChecked();
  event.Skip();
}

void PanelOrderButtons::OnCBStoch2Click( wxCommandEvent& event ) {
  m_order.m_bStochastic2 = m_cbEnableStoch2->IsChecked();
  event.Skip();
}

void PanelOrderButtons::OnCBStoch3Click( wxCommandEvent& event ) {
  m_order.m_bStochastic3 = m_cbEnableStoch3->IsChecked();
  event.Skip();
}

// == Quantity Selection

void PanelOrderButtons::OnTXTQuanStockTextUpdated( wxCommandEvent& event ) {
  m_order.m_sQuanStock = m_txtQuanStock->GetValue();
  event.Skip();
}

void PanelOrderButtons::OnTXTQuanFutureTextUpdated( wxCommandEvent& event ) {
  m_order.m_sQuanFuture = m_txtQuanFuture->GetValue();
  event.Skip();
}

void PanelOrderButtons::OnTXTQuanOptionTextUpdated( wxCommandEvent& event ) {
  m_order.m_sQuanOption = m_txtQuanOption->GetValue();
  event.Skip();
}

} // namespace tf
} // namespace ou
