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

#include "stdafx.h"

#include "PanelBasketTradingMain.h"


PanelBasketTradingMain::PanelBasketTradingMain(void) {
  Init();
}

PanelBasketTradingMain::PanelBasketTradingMain(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelBasketTradingMain::~PanelBasketTradingMain(void) {
}

bool PanelBasketTradingMain::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  wxPanel::Create( parent, id, pos, size, style );
  CreateControls();
  if (GetSizer())
  {
      GetSizer()->SetSizeHints(this);
  }
//  Centre();
  return true;
}

void PanelBasketTradingMain::Init( void ) {
    m_btnStart = NULL;
    m_btnExitPositions = NULL;
    m_btnStop = NULL;
}

void PanelBasketTradingMain::CreateControls() {    
////@begin PanelBasketTradingMain content construction
    PanelBasketTradingMain* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnStart = new wxButton( itemPanel1, ID_BtnStart, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(m_btnStart, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnExitPositions = new wxButton( itemPanel1, ID_BtnExitPositions, _("Exit Positions"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnExitPositions->Enable(false);
    itemBoxSizer5->Add(m_btnExitPositions, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnStop = new wxButton( itemPanel1, ID_BtnStop, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnStop->Enable(false);
    itemBoxSizer7->Add(m_btnStop, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnSave = new wxButton( itemPanel1, ID_BtnSave, _("Save Series"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnSave->Enable(false);
    itemBoxSizer9->Add(m_btnSave, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);
////@end PanelBasketTradingMain content construction

  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelBasketTradingMain::OnBtnStart, this, ID_BtnStart );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelBasketTradingMain::OnBtnExitPositions, this, ID_BtnExitPositions );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelBasketTradingMain::OnBtnStop, this, ID_BtnStop );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelBasketTradingMain::OnBtnSave, this, ID_BtnSave );
}

void PanelBasketTradingMain::OnBtnStart( wxCommandEvent& event ) {
  m_btnStart->Enable( false );
  m_btnExitPositions->Enable( true );
  m_btnStop->Enable( true );
  if ( 0 != m_OnBtnStart ) m_OnBtnStart();
}

void PanelBasketTradingMain::OnBtnExitPositions( wxCommandEvent& event ) {
  m_btnExitPositions->Enable( false );
  m_btnStop->Enable( true );
  if ( 0 != m_OnBtnExitPositions ) m_OnBtnExitPositions();
}

void PanelBasketTradingMain::OnBtnStop( wxCommandEvent& event ) {
  m_btnExitPositions->Enable( false );
  m_btnStop->Enable( false );
  m_btnSave->Enable( true );
  if ( 0 != m_OnBtnStop ) m_OnBtnStop();
}

void PanelBasketTradingMain::OnBtnSave( wxCommandEvent& event ) {
  if ( 0 != m_OnBtnSave ) m_OnBtnSave();
}

