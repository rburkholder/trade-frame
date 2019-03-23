/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#pragma once

// Started 2012/12/22

#include <wx/panel.h>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#define SYMBOL_PANELBASKETTRADINGMAIN_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELBASKETTRADINGMAIN_TITLE _("PanelBasketTradingMain")
#define SYMBOL_PANELBASKETTRADINGMAIN_IDNAME ID_PANELBASKETTRADINGMAIN
//#define SYMBOL_PANELBASKETTRADINGMAIN_SIZE wxSize(400, 300)
#define SYMBOL_PANELBASKETTRADINGMAIN_SIZE wxDefaultSize
#define SYMBOL_PANELBASKETTRADINGMAIN_POSITION wxDefaultPosition

class PanelBasketTradingMain: public wxPanel {
public:
  PanelBasketTradingMain(void);
  PanelBasketTradingMain(
    wxWindow* parent,
    wxWindowID id = SYMBOL_PANELBASKETTRADINGMAIN_IDNAME,
    const wxPoint& pos = SYMBOL_PANELBASKETTRADINGMAIN_POSITION,
    const wxSize& size = SYMBOL_PANELBASKETTRADINGMAIN_SIZE,
    long style = SYMBOL_PANELBASKETTRADINGMAIN_STYLE );
  bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PANELBASKETTRADINGMAIN_IDNAME,
    const wxPoint& pos = SYMBOL_PANELBASKETTRADINGMAIN_POSITION,
    const wxSize& size = SYMBOL_PANELBASKETTRADINGMAIN_SIZE,
    long style = SYMBOL_PANELBASKETTRADINGMAIN_STYLE );
  ~PanelBasketTradingMain(void);

  typedef FastDelegate0<> OnBtn_t;
  OnBtn_t m_OnBtnLoad;
  OnBtn_t m_OnBtnStart;
  OnBtn_t m_OnBtnExitPositions;
  OnBtn_t m_OnBtnStop;
  OnBtn_t m_OnBtnSave;

protected:
private:

  enum { ID_NULL=wxID_HIGHEST, SYMBOL_PANELBASKETTRADINGMAIN_IDNAME,
    ID_BtnLoad, ID_BtnStart, ID_BtnExitPositions, ID_BtnStop, ID_BtnSave
  };

    wxButton* m_btnLoad;
    wxButton* m_btnStart;
    wxButton* m_btnExitPositions;
    wxButton* m_btnStop;
    wxButton* m_btnSave;

  void Init();
  void CreateControls();
  bool ShowToolTips( void ) { return true; };

  void OnBtnLoad( wxCommandEvent& event );
  void OnBtnStart( wxCommandEvent& event );
  void OnBtnExitPositions( wxCommandEvent& event );
  void OnBtnStop( wxCommandEvent& event );
  void OnBtnSave( wxCommandEvent& event );
};

