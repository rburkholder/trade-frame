/************************************************************************
 * Copyright(c) 2020, One Unified. All rights reserved.                 *
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
 * File:    FrameOrderEntry.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading
 * Created: January 7, 2020, 20:30
 */

#include <wx/wx.h>
#include <wx/window.h>
#include <wx/sizer.h>

#include "FrameOrderEntry.h"

FrameOrderEntry::FrameOrderEntry() {
  Init();
}

FrameOrderEntry::FrameOrderEntry( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create( parent, id, caption, pos, size, style );
}

FrameOrderEntry::~FrameOrderEntry(void) {
  std::cout << "FrameMain::~FrameMain" << this->GetName() << std::endl;
}

bool FrameOrderEntry::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {

  wxFrame::Create( parent, id, caption, pos, size, style );

  CreateControls();
  if (GetSizer())
  {
      GetSizer()->SetSizeHints(this);
  }
//  Centre();
  return true;
}

void FrameOrderEntry::Init() {
  m_radioExpiryDay = NULL;
  m_radioExpiryGTC = NULL;
  m_radioOrderMarket = NULL;
  m_radioOrderLimit = NULL;
  m_radioOrderStop = NULL;
  m_txtProfitPrice = NULL;
  m_txtLimitPrice = NULL;
  m_txtStopPrice = NULL;
  m_txtQuantity = NULL;
  m_radioLong = NULL;
  m_radioShort = NULL;
  m_btnUpdate = NULL;
  m_btnSend = NULL;
  m_btnCancel = NULL;
}

void FrameOrderEntry::CreateControls( void ) {

    FrameOrderEntry* itemFrame1 = this;

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    itemFrame1->SetSizer(itemBoxSizer1);

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer1->Add(itemBoxSizer2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_radioExpiryDay = new wxRadioButton( itemFrame1, ID_radioDay, _("Day"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_radioExpiryDay->SetValue(false);
    itemBoxSizer2->Add(m_radioExpiryDay, 0, wxALIGN_LEFT|wxALL, 2);

    m_radioExpiryGTC = new wxRadioButton( itemFrame1, ID_radioGTC, _("GTC"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioExpiryGTC->SetValue(false);
    itemBoxSizer2->Add(m_radioExpiryGTC, 0, wxALIGN_LEFT|wxALL, 2);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer1->Add(itemBoxSizer5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_radioOrderMarket = new wxRadioButton( itemFrame1, ID_radioMarket, _("Market"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_radioOrderMarket->SetValue(false);
    itemBoxSizer5->Add(m_radioOrderMarket, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_radioOrderLimit = new wxRadioButton( itemFrame1, ID_radioLimit, _("Limit"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioOrderLimit->SetValue(false);
    itemBoxSizer5->Add(m_radioOrderLimit, 0, wxALIGN_LEFT|wxALL, 2);

    m_radioOrderStop = new wxRadioButton( itemFrame1, ID_radioStop, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioOrderStop->SetValue(false);
    itemBoxSizer5->Add(m_radioOrderStop, 0, wxALIGN_LEFT|wxALL, 2);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer1->Add(itemBoxSizer9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer9->Add(itemBoxSizer3, 0, wxALIGN_RIGHT|wxALL, 2);

    wxStaticText* itemStaticText4 = new wxStaticText( itemFrame1, wxID_STATIC, _("Profit Price"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    itemBoxSizer3->Add(itemStaticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtProfitPrice = new wxTextCtrl( itemFrame1, ID_txtProfitPrice, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_txtProfitPrice->SetMaxLength(10);
    itemBoxSizer3->Add(m_txtProfitPrice, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer9->Add(itemBoxSizer6, 0, wxALIGN_RIGHT|wxALL, 2);

    wxStaticText* itemStaticText7 = new wxStaticText( itemFrame1, wxID_STATIC, _("Limit Price"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    itemBoxSizer6->Add(itemStaticText7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtLimitPrice = new wxTextCtrl( itemFrame1, ID_txtLimitPrice, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_txtLimitPrice->SetMaxLength(10);
    itemBoxSizer6->Add(m_txtLimitPrice, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer9->Add(itemBoxSizer10, 0, wxALIGN_RIGHT|wxALL, 2);

    wxStaticText* itemStaticText11 = new wxStaticText( itemFrame1, wxID_STATIC, _("Stop Price"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    itemBoxSizer10->Add(itemStaticText11, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtStopPrice = new wxTextCtrl( itemFrame1, ID_txtStopPrice, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_txtStopPrice->SetMaxLength(10);
    itemBoxSizer10->Add(m_txtStopPrice, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer1->Add(itemBoxSizer13, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer14 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer1->Add(itemBoxSizer14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer14->Add(itemBoxSizer15, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    wxStaticBox* itemStaticBox16 = new wxStaticBox( itemFrame1, wxID_STATIC, _("Order Quan."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer15->Add(itemStaticBox16, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtQuantity = new wxTextCtrl( itemFrame1, ID_txtQuantity, _("1"), wxDefaultPosition, wxDefaultSize, 0 );
    m_txtQuantity->SetMaxLength(10);
    itemBoxSizer15->Add(m_txtQuantity, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer14->Add(itemBoxSizer18, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_radioLong = new wxRadioButton( itemFrame1, ID_radioLong, _("Long"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_radioLong->SetValue(false);
    itemBoxSizer18->Add(m_radioLong, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_radioShort = new wxRadioButton( itemFrame1, ID_radioShort, _("Short"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioShort->SetValue(false);
    itemBoxSizer18->Add(m_radioShort, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer21 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer14->Add(itemBoxSizer21, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnUpdate = new wxButton( itemFrame1, ID_btnUpdate, _("Update"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer21->Add(m_btnUpdate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_btnSend = new wxButton( itemFrame1, ID_btnSend, _("Send"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer21->Add(m_btnSend, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_btnCancel = new wxButton( itemFrame1, ID_btnCancel, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer21->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    Bind( wxEVT_CLOSE_WINDOW, &FrameOrderEntry::OnClose, this );
}

void FrameOrderEntry::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #3 -> Appxxx::OnExit
  Unbind( wxEVT_CLOSE_WINDOW, &FrameOrderEntry::OnClose, this );
//  Unbind( wxEVT_COMMAND_MENU_SELECTED, &FrameMain::OnMenuExitClick, this, ID_MENUEXIT );  // causes crash
  // http://docs.wxwidgets.org/trunk/classwx_close_event.html
  event.Skip();  // continue with base class stuff
}

wxBitmap FrameOrderEntry::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon FrameOrderEntry::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}
