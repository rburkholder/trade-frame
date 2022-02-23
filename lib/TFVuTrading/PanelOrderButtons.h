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

class wxRadioButton;
class wxButton;

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

   enum class EOrderType { Market, Limit, Bracket };

   using fBtnDone_t = std::function<void()>; // undo state set for the button while 'latched'
   using fBtnOrder_t = std::function<void(EOrderType, fBtnDone_t&&)>;

   void Set(
     fBtnOrder_t&&, // Buy
     fBtnOrder_t&&, // Sell
     fBtnOrder_t&&, // StopLong
     fBtnOrder_t&&, // StopShort
     fBtnOrder_t&&  // CancelAll
   );

protected:
private:

  enum { ID_Null=wxID_HIGHEST,
    ID_PanelOrderButtons,
    ID_BtnMarket, ID_BtnLimit, ID_BtnBracket,
    ID_BtnBuy, ID_BtnSell,
    ID_BtnStopLong, ID_BtnStopShort,
    ID_BtnCancelAll
  };

    wxRadioButton* m_btnMarket;
    wxRadioButton* m_btnLimit;
    wxRadioButton* m_btnBracket;
    wxButton* m_btnBuy;
    wxButton* m_btnSell;
    wxButton* m_btnStopLong;
    wxButton* m_btnStopShort;
    wxButton* m_btnCancelAll;

  EOrderType m_OrderType;

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

    void OnBtnMarketSelected( wxCommandEvent& event );
    void OnBtnLimitSelected( wxCommandEvent& event );
    void OnBtnBracketSelected( wxCommandEvent& event );
    void OnBtnBuyClick( wxCommandEvent& event );
    void OnBtnSellClick( wxCommandEvent& event );
    void OnBtnStopLongClick( wxCommandEvent& event );
    void OnBtnStopShortClick( wxCommandEvent& event );
    void OnBtnCancelAllClick( wxCommandEvent& event );

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
