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

#include <TFTrading/TradingEnumerations.h>

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

   enum class EInstrumentType { Underlying=0, Call=1, Put=2 };

   using fBtnDone_t = std::function<void()>; // undo state set for the button while 'latched'
   using fBtnOrder_t = std::function<void( ou::tf::OrderType::enumOrderType, EInstrumentType, fBtnDone_t&& )>;

   void Set(
     fBtnOrder_t&&, // Buy
     fBtnOrder_t&&, // Sell
     fBtnOrder_t&&  // CancelAll
   );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST,
    ID_PanelOrderButtons,
    ID_RadioOrderType, ID_RadioInstrument,
    ID_BtnBuy, ID_BtnSell, ID_BtnCancelAll,
    ID_ListPositions, ID_ListOrders
  };

    wxRadioBox* m_radioOrderType;
    wxRadioBox* m_radioInstrument;
    wxButton* m_btnBuy;
    wxButton* m_btnSell;
    wxButton* m_btnCancelAll;
    wxListCtrl* m_listPositions;
    wxListCtrl* m_listOrders;

  fBtnOrder_t m_fBtnOrderBuy;
  fBtnOrder_t m_fBtnOrderSell;
  fBtnOrder_t m_fBtnOrderStopLong;
  fBtnOrder_t m_fBtnOrderStopShort;
  fBtnOrder_t m_fBtnOrderCancelAll;

  void Init();
  void CreateControls();
  bool ShowToolTips() { return true; };

  void EnableButtons();
  void DisableButtons();

  void OnFocusChange( wxFocusEvent& event );

  void OnDestroy( wxWindowDestroyEvent& event );

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RadioOrderType
    void OnRadioOrderTypeSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RadioInstrument
    void OnRadioInstrumentSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BtnBuy
    void OnBtnBuyClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BtnSell
    void OnBtnSellClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BtnCancelAll
    void OnBtnCancelAllClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_ListPositions
    void OnListPositionsSelected( wxListEvent& event );

    /// wxEVT_COMMAND_LIST_ITEM_DESELECTED event handler for ID_ListPositions
    void OnListPositionsDeselected( wxListEvent& event );

    /// wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK event handler for ID_ListPositions
    void OnListPositionsRightClick( wxListEvent& event );

    /// wxEVT_CONTEXT_MENU event handler for ID_ListPositions
    void OnContextMenu( wxContextMenuEvent& event );

    /// wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_ListOrders
    void OnListOrdersSelected( wxListEvent& event );

    /// wxEVT_COMMAND_LIST_ITEM_DESELECTED event handler for ID_ListOrders
    void OnListOrdersDeselected( wxListEvent& event );

    /// wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK event handler for ID_ListOrders
    void OnListOrdersRightClick( wxListEvent& event );

    void OnRadioOrderTypeClick( wxCommandEvent& event );
    void OnRadioInstrumentClick( wxCommandEvent& event );

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
