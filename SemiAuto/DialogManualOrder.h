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

class UpdateInstrumentNameEvent: public wxEvent {
public:
  UpdateInstrumentNameEvent( wxEventType eventType, const std::string& s ): wxEvent( 0, eventType ), m_sName( s ) {};
  UpdateInstrumentNameEvent( const UpdateInstrumentNameEvent& event ): wxEvent( *this ), m_sName( event.m_sName ) {};
  ~UpdateInstrumentNameEvent( void ) {};
  UpdateInstrumentNameEvent* Clone( void ) const { return new UpdateInstrumentNameEvent( *this ); }
  const std::string& InstrumentName( void ) { return m_sName; };
protected:
private:
  std::string m_sName;
};

wxDECLARE_EVENT( EVT_UpdateInstrumentName, UpdateInstrumentNameEvent );

// wxFrame (wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE, const wxString &name=wxFrameNameStr)

#define ID_DIALOGMANUALORDER 10028

#define SYMBOL_DIALOGMANUALORDER_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_DIALOGMANUALORDER_TITLE _("Manual Order")
#define SYMBOL_DIALOGMANUALORDER_IDNAME ID_DIALOGMANUALORDER
#define SYMBOL_DIALOGMANUALORDER_SIZE wxSize(400, 300)
#define SYMBOL_DIALOGMANUALORDER_POSITION wxDefaultPosition

class DialogManualOrder: public wxDialog {
public:

  struct Order_t {
    wxString sSymbol;
    ou::tf::OrderSide::enumOrderSide eOrderSide;
    ou::tf::OrderType::enumOrderType eOrderType;
    unsigned long nQuantity;
    double dblPrice1;
    double dblPrice2;
    Order_t( void ): eOrderSide( ou::tf::OrderSide::Buy ), eOrderType( ou::tf::OrderType::Limit ), nQuantity( 100 ), dblPrice1( 0.0 ), dblPrice2( 0.0 ) {};
  };

  typedef FastDelegate1<const Order_t&> OnNewOrderHandler_t;
  void SetOnNewOrderHandler( OnNewOrderHandler_t function ) {
    OnNewOrder = function;
  }

  typedef FastDelegate1<const std::string&> OnSymbolTextUpdated_t;
  void SetOnSymbolTextUpdated( OnSymbolTextUpdated_t function ) {
    OnSymbolTextUpdated = function;
  }

  typedef FastDelegate1<unsigned int> OnFocusPropogate_t;
  void SetOnFocusPropogate( OnFocusPropogate_t function ) {
    OnFocusPropogate = function;
  }

  DialogManualOrder( void );
  DialogManualOrder(/*
    wxWindow* parent,
   const wxString& title = "Manual Orders", 
   const wxPoint& pos=wxDefaultPosition, 
   const wxSize& size=wxSize(500, 120), 
   long style=wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)*/
   wxWindow* parent, 
   wxWindowID id = SYMBOL_DIALOGMANUALORDER_IDNAME, 
   const wxString& caption = SYMBOL_DIALOGMANUALORDER_TITLE, 
   const wxPoint& pos = SYMBOL_DIALOGMANUALORDER_POSITION, 
   const wxSize& size = SYMBOL_DIALOGMANUALORDER_SIZE, 
   long style = SYMBOL_DIALOGMANUALORDER_STYLE 
   );
  ~DialogManualOrder(void);

  bool Create(/*
    wxWindow* parent,
   const wxString& title = "Manual Orders", 
   const wxPoint& pos=wxDefaultPosition, 
   const wxSize& size=wxSize(500, 120), 
   long style=wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)*/
   wxWindow* parent, 
   wxWindowID id = SYMBOL_DIALOGMANUALORDER_IDNAME, 
   const wxString& caption = SYMBOL_DIALOGMANUALORDER_TITLE, 
   const wxPoint& pos = SYMBOL_DIALOGMANUALORDER_POSITION, 
   const wxSize& size = SYMBOL_DIALOGMANUALORDER_SIZE, 
   long style = SYMBOL_DIALOGMANUALORDER_STYLE
   );

  //void SetInstrumentName( const std::string& sName );
  void SetIxStruct( unsigned int ix ) { m_ixStruct = ix; };

  void QueueEvent( wxEvent* event ) { wxEvtHandler::QueueEvent( event ); };

protected:
private:

  enum { ID_Null=wxID_HIGHEST, ID_LblInstrumentSymbol, ID_LblInstrumentName, ID_LblQuantity, ID_LblPrice1, ID_LblPrice2,
    ID_TxtInstrumentSymbol, ID_TxtQuantity, ID_TxtPrice1, ID_TxtPrice2,
    ID_BtnOrderTypeMarket, ID_BtnOrderTypeLimit, ID_BtnOrderTypeStop, ID_BtnBuy, ID_BtnSell, ID_BtnCancel
  };

  Order_t m_order;

  unsigned int m_ixStruct;  // for use by SemiAuto

  OnNewOrderHandler_t OnNewOrder;
  OnSymbolTextUpdated_t OnSymbolTextUpdated;
  OnFocusPropogate_t OnFocusPropogate;

  wxTextCtrl* m_txtInstrumentSymbol;
  wxStaticText* m_txtInstrumentName;
  wxString m_sSymbol;
  wxButton* m_btnBuy;
  wxButton* m_btnSell;
  wxButton* m_btnCancel;
  wxStaticText* m_txtStatus;

  void Init( void );
  void CreateControls( void );
  bool ShowToolTips( void ) { return true; };

  void EnableButtons( void );
  void DisableButtons( void );

  void OnFocusChange( wxFocusEvent& event );

  void EmitOrder( void );

  void OnClose( wxCloseEvent& event );
  void OnBtnMarket( wxCommandEvent& event ) { m_order.eOrderType = ou::tf::OrderType::Market; };
  void OnBtnLimit( wxCommandEvent& event ) { m_order.eOrderType = ou::tf::OrderType::Limit; };
  void OnBtnStop( wxCommandEvent& event ) { m_order.eOrderType = ou::tf::OrderType::Stop; };
  void OnBtnBuy( wxCommandEvent& event );
  void OnBtnSell( wxCommandEvent& event );

  void OnInstrumentSymbolTextIdle( wxIdleEvent& event );
  void OnInstrumentSymbolTextUpdated( wxCommandEvent& event );
  void OnInstrumentSymbolTextEnter( wxCommandEvent& event );

  void OnUpdateInstrumentName( UpdateInstrumentNameEvent& event );
};

