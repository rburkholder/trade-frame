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

#include <OUCommon/Delegate.h>

#include <TFTrading/TradingEnumerations.h>

// wxFrame (wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE, const wxString &name=wxFrameNameStr)

class FrameManualOrder: public wxFrame {
public:
  FrameManualOrder(wxWindow* parent, const wxString& title = "Manual Orders", const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxSize(500, 120), long style=wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)
);
  ~FrameManualOrder(void);
protected:
private:

  enum { ID_Null=wxID_HIGHEST, ID_LblInstrument, ID_LblQuantity, ID_LblPrice1, ID_LblPrice2,
    ID_TxtInstrument, ID_TxtQuantity, ID_TxtPrice1, ID_TxtPrice2,
    ID_BtnOrderTypeMarket, ID_BtnOrderTypeLimit, ID_BtnOrderTypeStop, ID_BtnBuy, ID_BtnSell, ID_BtnCancel
  };

  ou::tf::OrderType::enumOrderType m_eOrderType;
  wxString m_sInstrument;
  unsigned long m_nQuantity;
  double m_dblPrice1;
  double m_dblPrice2;

  void CreateControls( void );
  bool ShowToolTips( void ) { return true; };

  void OnClose( wxCloseEvent& event );
  void OnBtnMarket( wxCommandEvent& event ) { m_eOrderType = ou::tf::OrderType::Market; };
  void OnBtnLimit( wxCommandEvent& event ) { m_eOrderType = ou::tf::OrderType::Limit; };
  void OnBtnStop( wxCommandEvent& event ) { m_eOrderType = ou::tf::OrderType::Stop; };
  void OnBtnBuy( wxCommandEvent& event );
  void OnBtnSell( wxCommandEvent& event );
  void OnBtnCancel( wxCommandEvent& event );
};

