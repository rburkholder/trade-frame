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
 * File:    PanelDividenNotes.cpp
 * Author:  raymond@burkholder.net
 * Project: Dividend
 * Created: April 27, 2025 12:47:19
 */

#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "PanelDividenNotes.hpp"

PanelDividenNotes::PanelDividenNotes() {
  Init();
}

PanelDividenNotes::PanelDividenNotes( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create( parent, id, pos, size, style );
}

PanelDividenNotes::~PanelDividenNotes() {
}

void PanelDividenNotes::Init() {
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
  m_lbTags = nullptr;
  m_fBtnSave = nullptr;
  m_fBtnUndo = nullptr;

  m_bByFields = false;

}

bool PanelDividenNotes::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

  SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  wxPanel::Create( parent, id, pos, size, style );

  CreateControls();
  //if (GetSizer()) {
  //    GetSizer()->SetSizeHints(this);
  //}
  //Centre();
  return true;
}

void PanelDividenNotes::CreateControls() {

  PanelDividenNotes* itemPanel1 = this;

  wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
  itemPanel1->SetSizer(itemBoxSizer2);

  wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer2->Add(itemBoxSizer3, 0, wxGROW|wxLEFT|wxRIGHT, 2);

  wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizer3->Add(itemBoxSizer1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer1->Add(itemBoxSizer4, 0, wxGROW|wxALL, 2);

  wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxStaticText* itemStaticText6 = new wxStaticText( itemPanel1, ID_STATIC_Yield, _("Yield:"), wxDefaultPosition, wxSize(60, -1), wxALIGN_RIGHT );
  itemBoxSizer5->Add(itemStaticText6, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

  m_txtYield = new wxTextCtrl( itemPanel1, ID_TEXT_Yield, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
  itemBoxSizer5->Add(m_txtYield, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer4->Add(itemBoxSizer8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxStaticText* itemStaticText9 = new wxStaticText( itemPanel1, ID_STATIC_Last, _("Last:"), wxDefaultPosition, wxSize(60, -1), wxALIGN_RIGHT );
  itemBoxSizer8->Add(itemStaticText9, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

  m_txtLast = new wxTextCtrl( itemPanel1, ID_TEXT_Last, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
  itemBoxSizer8->Add(m_txtLast, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer1->Add(itemBoxSizer11, 0, wxGROW|wxALL, 3);

  wxBoxSizer* itemBoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer11->Add(itemBoxSizer12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxStaticText* itemStaticText13 = new wxStaticText( itemPanel1, ID_STATIC_Amount, _("Amount:"), wxDefaultPosition, wxSize(60, -1), wxALIGN_RIGHT );
  itemBoxSizer12->Add(itemStaticText13, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

  m_txtAmount = new wxTextCtrl( itemPanel1, ID_TEXT_Amount, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
  itemBoxSizer12->Add(m_txtAmount, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer11->Add(itemBoxSizer15, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxStaticText* itemStaticText16 = new wxStaticText( itemPanel1, ID_STATIC_Rate, _("Rate:"), wxDefaultPosition, wxSize(60, -1), wxALIGN_RIGHT );
  itemBoxSizer15->Add(itemStaticText16, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

  m_txtRate = new wxTextCtrl( itemPanel1, ID_TEXT_Rate, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
  itemBoxSizer15->Add(m_txtRate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer1->Add(itemBoxSizer18, 0, wxGROW|wxALL, 3);

  wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer18->Add(itemBoxSizer19, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxStaticText* itemStaticText20 = new wxStaticText( itemPanel1, ID_STATIC_ExDiv, _("ExDiv:"), wxDefaultPosition, wxSize(60, -1), wxALIGN_RIGHT );
  itemBoxSizer19->Add(itemStaticText20, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

  m_txtDateExDiv = new wxTextCtrl( itemPanel1, ID_TEXT_ExDiv, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
  itemBoxSizer19->Add(m_txtDateExDiv, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxBoxSizer* itemBoxSizer22 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer18->Add(itemBoxSizer22, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxStaticText* itemStaticText23 = new wxStaticText( itemPanel1, ID_STATIC_Payed, _("Payed:"), wxDefaultPosition, wxSize(60, -1), wxALIGN_RIGHT );
  itemBoxSizer22->Add(itemStaticText23, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

  m_txtDatePayed = new wxTextCtrl( itemPanel1, ID_TEXT_Payed, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
  itemBoxSizer22->Add(m_txtDatePayed, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

  wxBoxSizer* itemBoxSizer26 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizer3->Add(itemBoxSizer26, 1, wxGROW|wxALL, 2);

  wxArrayString m_lbTagsStrings;
  m_lbTags = new wxListBox( itemPanel1, ID_LB_TAGS, wxDefaultPosition, wxDefaultSize, m_lbTagsStrings, wxLB_SINGLE );
  itemBoxSizer26->Add(m_lbTags, 1, wxGROW|wxALL, 0);

  wxBoxSizer* itemBoxSizer25 = new wxBoxSizer(wxVERTICAL);
  itemBoxSizer2->Add(itemBoxSizer25, 1, wxGROW|wxLEFT|wxRIGHT, 3);

  m_txtNotes = new wxTextCtrl( itemPanel1, ID_TEXT_Notes, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
  itemBoxSizer25->Add(m_txtNotes, 1, wxGROW|wxALL, 1);

  wxBoxSizer* itemBoxSizer27 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer25->Add(itemBoxSizer27, 0, wxGROW|wxALL, 1);

  wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer27->Add(itemBoxSizer28, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  m_txtName = new wxStaticText( itemPanel1, ID_STATIC_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer28->Add(m_txtName, 1, wxGROW|wxLEFT|wxRIGHT, 2);

  wxBoxSizer* itemBoxSizer30 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer27->Add(itemBoxSizer30, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  m_btnUndo = new wxButton( itemPanel1, ID_BTN_Undo, _("Undo"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer30->Add(m_btnUndo, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

  m_btnSave = new wxButton( itemPanel1, ID_BTN_Save, _("Save"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer30->Add(m_btnSave, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);
  m_btnDefaultBackground = m_btnSave->GetBackgroundColour();

  m_btnUndo->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelDividenNotes::OnBTNUndoClick, this );
  m_btnSave->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelDividenNotes::OnBTNSaveClick, this );
  m_txtNotes->Bind( wxEVT_COMMAND_TEXT_UPDATED, &PanelDividenNotes::OnTextUpdated, this );
  m_txtNotes->Bind( wxEVT_KILL_FOCUS, &PanelDividenNotes::OnTextFocusKilled, this );

  Bind( wxEVT_CLOSE_WINDOW, &PanelDividenNotes::OnClose, this );

}

void PanelDividenNotes::SetFields( const Fields& fields ) {

  m_bByFields = true;

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

  m_bByFields = false;
}

void PanelDividenNotes::SetTags( const wxArrayString& rTags ) {
  m_lbTags->Set( rTags );
}

void PanelDividenNotes::OnBTNUndoClick( wxCommandEvent& event ) { // wxEVT_COMMAND_BUTTON_CLICKED
  if ( m_fBtnUndo ) {
    m_txtNotes->SetValue( m_fBtnUndo() );
    m_txtNotes->SetBackgroundColour( m_btnDefaultBackground );
  }
  event.Skip();
}

void PanelDividenNotes::OnBTNSaveClick( wxCommandEvent& event ) { // wxEVT_COMMAND_BUTTON_CLICKED
  const std::string sNotes( m_txtNotes->GetValue() );
  if ( m_fBtnSave ) m_fBtnSave( sNotes );
  m_btnSave->SetBackgroundColour( m_btnDefaultBackground );
  event.Skip();
}

void PanelDividenNotes::OnTextUpdated( wxCommandEvent& event ) { // wxEVT_COMMAND_TEXT_UPDATED
  if ( m_bByFields ) {
    m_btnSave->SetBackgroundColour( m_btnDefaultBackground );
  }
  else {
    m_btnSave->SetBackgroundColour( *wxYELLOW );
  }
  event.Skip();
}

void PanelDividenNotes::OnTextFocusKilled( wxFocusEvent& event ) { // wxEVT_KILL_FOCUS
  // save the text
  event.Skip();
}

void PanelDividenNotes::OnClose( wxCloseEvent& event ) {
  m_btnUndo->Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelDividenNotes::OnBTNUndoClick, this );
  m_btnSave->Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelDividenNotes::OnBTNSaveClick, this );
  m_txtNotes->Unbind( wxEVT_COMMAND_TEXT_UPDATED, &PanelDividenNotes::OnTextUpdated, this );
  m_txtNotes->Unbind( wxEVT_KILL_FOCUS, &PanelDividenNotes::OnTextFocusKilled, this );
  Unbind( wxEVT_CLOSE_WINDOW, &PanelDividenNotes::OnClose, this );
}

wxBitmap PanelDividenNotes::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon PanelDividenNotes::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}

