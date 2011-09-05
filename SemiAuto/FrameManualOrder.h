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

#pragma once

#include <string>

#include <wx/wx.h>
#include <wx/string.h>

//#include <OUCommon/Delegate.h>
#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include <TFTrading/TradingEnumerations.h>

// wxFrame (wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE, const wxString &name=wxFrameNameStr)

class FrameManualOrder: public wxFrame {
public:

  struct Order_t {
    wxString sSymbol;
    ou::tf::OrderSide::enumOrderSide eOrderSide;
    ou::tf::OrderType::enumOrderType eOrderType;
    unsigned long nQuantity;
    double dblPrice1;
    double dblPrice2;
    Order_t( void ): eOrderSide( ou::tf::OrderSide::Buy ), eOrderType( ou::tf::OrderType::Limit ), nQuantity( 0 ), dblPrice1( 0.0 ), dblPrice2( 0.0 ) {};
  };

  typedef FastDelegate1<const Order_t&> OnNewOrderHandler_t;
  void SetOnNewOrderHandler( OnNewOrderHandler_t function ) {
    OnNewOrder = function;
  }

  typedef FastDelegate1<const std::string&> OnSymbolTextUpdated_t;
  void SetOnSymbolTextUpdated( OnSymbolTextUpdated_t function ) {
    OnSymbolTextUpdated = function;
  }

  FrameManualOrder( void );
  FrameManualOrder(
    wxWindow* parent,
   const wxString& title = "Manual Orders", 
   const wxPoint& pos=wxDefaultPosition, 
   const wxSize& size=wxSize(500, 120), 
   long style=wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)
   );
  ~FrameManualOrder(void);

  bool Create(
    wxWindow* parent,
   const wxString& title = "Manual Orders", 
   const wxPoint& pos=wxDefaultPosition, 
   const wxSize& size=wxSize(500, 120), 
   long style=wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)
   );

protected:
private:

  enum { ID_Null=wxID_HIGHEST, ID_LblInstrumentSymbol, ID_LblInstrumentName, ID_LblQuantity, ID_LblPrice1, ID_LblPrice2,
    ID_TxtInstrumentSymbol, ID_TxtQuantity, ID_TxtPrice1, ID_TxtPrice2,
    ID_BtnOrderTypeMarket, ID_BtnOrderTypeLimit, ID_BtnOrderTypeStop, ID_BtnBuy, ID_BtnSell
  };

  Order_t m_order;

  OnNewOrderHandler_t OnNewOrder;
  OnSymbolTextUpdated_t OnSymbolTextUpdated;

  wxTextCtrl* m_txtInstrumentSymbol;

  void Init( void );
  void CreateControls( void );
  bool ShowToolTips( void ) { return true; };

  void EmitOrder( void ) const;

  void OnClose( wxCloseEvent& event );
  void OnBtnMarket( wxCommandEvent& event ) { m_order.eOrderType = ou::tf::OrderType::Market; };
  void OnBtnLimit( wxCommandEvent& event ) { m_order.eOrderType = ou::tf::OrderType::Limit; };
  void OnBtnStop( wxCommandEvent& event ) { m_order.eOrderType = ou::tf::OrderType::Stop; };
  void OnBtnBuy( wxCommandEvent& event );
  void OnBtnSell( wxCommandEvent& event );

  void OnInstrumentSymbolTextIdle( wxIdleEvent& event );
  void OnInstrumentSymbolTextUpdated( wxCommandEvent& event );
  void OnInstrumentSymbolTextEnter( wxCommandEvent& event );
};

