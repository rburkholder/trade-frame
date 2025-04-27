/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    PanelSymbolInfo.cpp
 * Author:  raymond@burkholder.net
 * Project: Dividend
 * Created: April 27, 2025 12:47:19
 */

#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "PanelSymbolInfo.hpp"

PanelSymbolInfo::PanelSymbolInfo() {
  Init();
}

PanelSymbolInfo::PanelSymbolInfo( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create( parent, id, pos, size, style );
}

PanelSymbolInfo::~PanelSymbolInfo() {
}

void PanelSymbolInfo::Init() {
  m_txtYield = nullptr;
  m_txtLast = nullptr;
  m_txtAmount = nullptr;
  m_txtRate = nullptr;
  m_txtDateExDiv = nullptr;
  m_txtDatePayed = nullptr;
  m_txtNotes = nullptr;
  m_txtName = nullptr;
  m_btnUndo = nullptr;
  m_btnSave = nullptr;
  m_fBtnSave = nullptr;
  m_fBtnUndo = nullptr;

}

bool PanelSymbolInfo::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

  SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  wxPanel::Create( parent, id, pos, size, style );

  CreateControls();
  //if (GetSizer()) {
  //    GetSizer()->SetSizeHints(this);
  //}
  //Centre();
  return true;
}

void PanelSymbolInfo::CreateControls() {

  PanelSymbolInfo* itemPanel1 = this;

  wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
  itemPanel1->SetSizer(itemBoxSizer2);

  wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizer2->Add(itemBoxSizer1, 0, wxGROW|wxLEFT|wxRIGHT, 3);

  wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer1->Add(itemBoxSizer3, 0, wxGROW|wxALL, 3);

  wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer3->Add(itemBoxSizer4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxStaticText* itemStaticText5 = new wxStaticText( itemPanel1, ID_STATIC_Yield, _("Yield:"), wxDefaultPosition, wxSize(60, -1), wxALIGN_RIGHT );
  itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

  m_txtYield = new wxTextCtrl( itemPanel1, ID_TEXT_Yield, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
  itemBoxSizer4->Add(m_txtYield, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer3->Add(itemBoxSizer7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxStaticText* itemStaticText8 = new wxStaticText( itemPanel1, ID_STATIC_Last, _("Last:"), wxDefaultPosition, wxSize(60, -1), wxALIGN_RIGHT );
  itemBoxSizer7->Add(itemStaticText8, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

  m_txtLast = new wxTextCtrl( itemPanel1, ID_TEXT_Last, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
  itemBoxSizer7->Add(m_txtLast, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxBoxSizer* itemBoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer1->Add(itemBoxSizer10, 0, wxGROW|wxALL, 3);

  wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer10->Add(itemBoxSizer11, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxStaticText* itemStaticText12 = new wxStaticText( itemPanel1, ID_STATIC_Amount, _("Amount:"), wxDefaultPosition, wxSize(60, -1), wxALIGN_RIGHT );
  itemBoxSizer11->Add(itemStaticText12, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

  m_txtAmount = new wxTextCtrl( itemPanel1, ID_TEXT_Amount, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
  itemBoxSizer11->Add(m_txtAmount, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxBoxSizer* itemBoxSizer14 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer10->Add(itemBoxSizer14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxStaticText* itemStaticText15 = new wxStaticText( itemPanel1, ID_STATIC_Rate, _("Rate:"), wxDefaultPosition, wxSize(60, -1), wxALIGN_RIGHT );
  itemBoxSizer14->Add(itemStaticText15, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

  m_txtRate = new wxTextCtrl( itemPanel1, ID_TEXT_Rate, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
  itemBoxSizer14->Add(m_txtRate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer1->Add(itemBoxSizer5, 0, wxGROW|wxALL, 3);

  wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer5->Add(itemBoxSizer6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxStaticText* itemStaticText7 = new wxStaticText( itemPanel1, ID_STATIC_ExDiv, _("ExDiv:"), wxDefaultPosition, wxSize(60, -1), wxALIGN_RIGHT );
  itemBoxSizer6->Add(itemStaticText7, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

  m_txtDateExDiv = new wxTextCtrl( itemPanel1, ID_TEXT_ExDiv, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
  itemBoxSizer6->Add(m_txtDateExDiv, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer5->Add(itemBoxSizer9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxStaticText* itemStaticText10 = new wxStaticText( itemPanel1, ID_STATIC_Payed, _("Payed:"), wxDefaultPosition, wxSize(60, -1), wxALIGN_RIGHT );
  itemBoxSizer9->Add(itemStaticText10, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

  m_txtDatePayed = new wxTextCtrl( itemPanel1, ID_TEXT_Payed, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
  itemBoxSizer9->Add(m_txtDatePayed, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizer2->Add(itemBoxSizer17, 1, wxGROW|wxLEFT|wxRIGHT, 3);

  m_txtNotes = new wxTextCtrl( itemPanel1, ID_TEXT_Notes, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
  itemBoxSizer17->Add(m_txtNotes, 1, wxGROW|wxALL, 1);

  wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer17->Add(itemBoxSizer19, 0, wxGROW|wxALL, 1);

  wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer19->Add(itemBoxSizer8, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  m_txtName = new wxStaticText( itemPanel1, ID_STATIC_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer8->Add(m_txtName, 1, wxGROW|wxLEFT|wxRIGHT, 2);

  wxBoxSizer* itemBoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer19->Add(itemBoxSizer12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  m_btnUndo = new wxButton( itemPanel1, ID_BTN_Undo, _("Undo"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer12->Add(m_btnUndo, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

  m_btnSave = new wxButton( itemPanel1, ID_BTN_Save, _("Save"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer12->Add(m_btnSave, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

  m_btnUndo->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelSymbolInfo::OnBTNUndoClick, this );
  m_btnSave->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelSymbolInfo::OnBTNSaveClick, this ) ;
  Bind( wxEVT_CLOSE_WINDOW, &PanelSymbolInfo::OnClose, this );

}

void PanelSymbolInfo::SetFields( Fields& fields ) {
  m_txtYield->SetValue( fields.sYield );
  m_txtLast->SetValue( fields.sLast );
  m_txtAmount->SetValue( fields.sAmount );
  m_txtRate->SetValue( fields.sRate );
  m_txtDateExDiv->SetValue( fields.sExDiv );
  m_txtDatePayed->SetValue( fields.sPayed );
  m_txtNotes->SetValue( fields.sNotes );
  m_txtName->SetLabel( fields.sName );
  m_fBtnSave = std::move( fields.fBtnSave );
  m_fBtnUndo = std::move( fields.fBtnUndo );
}

void PanelSymbolInfo::OnBTNUndoClick( wxCommandEvent& event ) { // wxEVT_COMMAND_BUTTON_CLICKED
  if ( m_fBtnUndo ) m_txtNotes->SetValue( m_fBtnUndo() );
  event.Skip();
}
void PanelSymbolInfo::OnBTNSaveClick( wxCommandEvent& event ) { // wxEVT_COMMAND_BUTTON_CLICKED
  const std::string sNotes( m_txtNotes->GetValue() );
  if ( m_fBtnSave ) m_fBtnSave( sNotes );
  event.Skip();
}

void PanelSymbolInfo::OnClose( wxCloseEvent& event ) {
  Unbind( wxEVT_CLOSE_WINDOW, &PanelSymbolInfo::OnClose, this );
}

wxBitmap PanelSymbolInfo::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon PanelSymbolInfo::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}

