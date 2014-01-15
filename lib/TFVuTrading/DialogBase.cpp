/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include "DialogBase.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

DialogBase::DialogBase( void ) {
  Init();
}
/*
DialogBase::DialogBase( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {
    Init();
    Create(parent, id, caption, pos, size, style);
}
*/
DialogBase::~DialogBase( void ) {
}

void DialogBase::Init( void ) {
    m_pDataExchange = 0;
}

bool DialogBase::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) {

  wxDialog::Create( parent, id, caption, pos, size, style );

  // http://docs.wxwidgets.org/2.8/wx_wxdialog.html#wxdialogsetescapeid
  Bind( wxEVT_CLOSE_WINDOW, &DialogBase::OnCloseWindow, this );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &DialogBase::OnOk, this, wxID_OK );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &DialogBase::OnCancel, this, wxID_CANCEL );
  //Bind( wxEVT_COMMAND_BUTTON_CLICKED, &DialogBase::OnCloseWindow, this, wxID_CANCEL );

  return true;

}

void DialogBase::SetDataExchange( DataExchange* pde ) {
  m_pDataExchange = pde;
}

// http://docs.wxwidgets.org/trunk/overview_validator.html
void DialogBase::OnOk( wxCommandEvent& event ) {
  // ok button
  if ( Validate() && TransferDataFromWindow() ) {
    if ( IsModal() )
      EndModal(wxID_OK);
    else {
      SetReturnCode(wxID_OK);
      m_pDataExchange->bOk = true;
      this->Show(false);
      if ( 0 != m_OnDoneHandler ) m_OnDoneHandler( m_pDataExchange );
    }
  }
}

void DialogBase::OnCancel( wxCommandEvent& event ) {
  // cancel or escape
  // could be merged with OnClose
  OnCancelOrClose();
}

void DialogBase::OnCloseWindow( wxCloseEvent& event ) {
  // close button
  OnCancelOrClose();
  event.Skip();
}

void DialogBase::OnCancelOrClose( void ) {
  if ( IsModal() ) // this condition may not be correct modal endings.
    EndModal(wxID_CANCEL);
  else {
    SetReturnCode(wxID_CANCEL);
    m_pDataExchange->bOk = false;
    this->Show(false);
    if ( 0 != m_OnDoneHandler ) m_OnDoneHandler( m_pDataExchange );
  }
}

wxBitmap DialogBase::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon DialogBase::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}

} // namespace tf
} // namespace ou
