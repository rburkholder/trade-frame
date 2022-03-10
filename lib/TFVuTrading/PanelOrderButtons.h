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
 * File:    PanelOrderButtons.h
 * Author:  raymond@burkholder.net
 * Project: lib/TFVuTrading
 * Created: February 11, 2022 13:20
 */

#pragma once

#include <functional>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/wx.h>

#include "PanelOrderButtons_structs.h"

class wxButton;
class wxListCtrl;
class wxListEvent;

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANELORDERBUTTONS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELORDERBUTTONS_TITLE _("Order Buttons")
#define SYMBOL_PANELORDERBUTTONS_IDNAME ID_PanelOrderButtons
#define SYMBOL_PANELORDERBUTTONS_SIZE wxSize(400, 300)
#define SYMBOL_PANELORDERBUTTONS_POSITION wxDefaultPosition

class PanelOrderButtons: public wxPanel {
public:

  PanelOrderButtons();
  PanelOrderButtons(
   wxWindow* parent,
   wxWindowID id = SYMBOL_PANELORDERBUTTONS_IDNAME,
   const wxPoint& pos = SYMBOL_PANELORDERBUTTONS_POSITION,
   const wxSize& size = SYMBOL_PANELORDERBUTTONS_SIZE,
   long style = SYMBOL_PANELORDERBUTTONS_STYLE
   );
  virtual ~PanelOrderButtons(void);

  bool Create(
   wxWindow* parent,
   wxWindowID id = SYMBOL_PANELORDERBUTTONS_IDNAME,
   const wxPoint& pos = SYMBOL_PANELORDERBUTTONS_POSITION,
   const wxSize& size = SYMBOL_PANELORDERBUTTONS_SIZE,
   long style = SYMBOL_PANELORDERBUTTONS_STYLE
   );

   enum class EInstrumentType { Underlying=0, Call1=1, Put1=2, Call2=3, Put3=4 };

   using fBtnDone_t = std::function<void()>; // undo state set for the button while 'latched'
   using fBtnOrder_t = std::function<void( const PanelOrderButtons_Order&, fBtnDone_t&& )>;

   void Set(
     fBtnOrder_t&&, // Buy
     fBtnOrder_t&&, // Sell
     fBtnOrder_t&&, // Close
     fBtnOrder_t&&  // Cancel
   );

   void Update( const PanelOrderButtons_MarketData& );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST
  , ID_PanelOrderButtons
  , ID_BtnBuy, ID_BtnSell, ID_BtnClose, ID_BtnCancel
  , ID_CB_CockForCursor
  , ID_CB_PositionEntry
  , ID_CB_PositionExitProfit
  , ID_CB_PositionExitStop
  , ID_TXT_PositionEntry
  , ID_TXT_PositionExitProfit
  , ID_TXT_PositionExitStop
  , ID_RADIO_PositionEntry
  , ID_RADIO_PositionExitProfit
  , ID_RADIO_PositionExitTop
  , ID_RADIO_Instrument
  , ID_TXT_Base, ID_TXT_BaseAsk, ID_TXT_BaseBid
  , ID_TXT_Call1, ID_TXT_Call1Ask, ID_TXT_Call1Bid
  , ID_TXT_Put1, ID_TXT_Put1Ask, ID_TXT_Put1Bid
  , ID_TXT_Call2, ID_TXT_Call2Ask, ID_TXT_Call2Bid
  , ID_TXT_Put2, ID_TXT_Put2Ask, ID_TXT_Put2Bid
  , ID_CB_Stoch1, ID_CB_Stoch2, ID_CB_Stoch3
  };

  PanelOrderButtons_Order m_order;
  PanelOrderButtons_MarketData m_data;

    wxCheckBox* m_cbCockForCursor;
    wxCheckBox* m_cbEnablePositionEntry;
    wxTextCtrl* m_txtPricePositionEntry;
    wxRadioBox* m_radioPositionEntry;
    wxCheckBox* m_cbEnableProfitExit;
    wxTextCtrl* m_txtPriceProfitExit;
    wxRadioBox* m_radioExitProfit;
    wxCheckBox* m_cbEnableStopExit;
    wxTextCtrl* m_txtPriceStopExit;
    wxRadioBox* m_radioExitStop;
    wxButton* m_btnBuy;
    wxButton* m_btnSell;
    wxButton* m_btnClose;
    wxButton* m_btnCancel;
    wxRadioBox* m_radioInstrument;
    wxStaticText* m_txtBase;
    wxStaticText* m_txtBaseAsk;
    wxStaticText* m_txtBaseBid;
    wxStaticText* m_txtCall1;
    wxStaticText* m_txtCall1Ask;
    wxStaticText* m_txtCall1Bid;
    wxStaticText* m_txtPut1;
    wxStaticText* m_txtPut1Ask;
    wxStaticText* m_txtPut1Bid;
    wxStaticText* m_txtCall2;
    wxStaticText* m_txtCall2Ask;
    wxStaticText* m_txtCall2Bid;
    wxStaticText* m_txtPut2;
    wxStaticText* m_txtPut2Ask;
    wxStaticText* m_txtPut2Bid;
    wxCheckBox* m_cbEnableStoch1;
    wxCheckBox* m_cbEnableStoch2;
    wxCheckBox* m_cbEnableStoch3;

  fBtnOrder_t m_fBtnOrderBuy;
  fBtnOrder_t m_fBtnOrderSell;
  fBtnOrder_t m_fBtnOrderClose;
  fBtnOrder_t m_fBtnOrderCancel;

  void Init();
  void CreateControls();
  bool ShowToolTips() { return true; };

  bool ValidateFields();

  void OnFocusChange( wxFocusEvent& event );

  void OnDestroy( wxWindowDestroyEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CB_CockForCursor
    void OnCBCockForCursorClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CB_PositionEntry
    void OnCBPositionEntryClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TXT_PositionEntry
    void OnTXTPositionEntryTextUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RADIO_PositionEntry
    void OnRADIOPositionEntrySelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CB_PositionExitProfit
    void OnCBPositionExitProfitClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TXT_PositionExitProfit
    void OnTXTPositionExitProfitTextUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RADIO_PositionExitProfit
    void OnRADIOPositionExitProfitSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CB_PositionExitStop
    void OnCBPositionExitStopClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TXT_PositionExitStop
    void OnTXTPositionExitStopTextUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RADIO_PositionExitTop
    void OnRADIOPositionExitTopSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BtnBuy
    void OnBtnBuyClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BtnSell
    void OnBtnSellClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BtnClose
    void OnBtnCloseClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BtnCancel
    void OnBtnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RADIO_Instrument
    void OnRADIOInstrumentSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CB_Stoch1
    void OnCBStoch1Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CB_Stoch2
    void OnCBStoch2Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CB_Stoch3
    void OnCBStoch3Click( wxCommandEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::PanelOrderButtons, 1)
