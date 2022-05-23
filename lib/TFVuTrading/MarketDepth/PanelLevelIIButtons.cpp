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
 * File:    PanelLevelIIButtons.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: May 23 2022, 11:27
 */

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/checkbox.h>


#include "PanelLevelIIButtons.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

PanelLevelIIButtons::PanelLevelIIButtons(): wxPanel()
{
  Init();
};

PanelLevelIIButtons::PanelLevelIIButtons( /*wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style*/
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style
) {
  Init();
  Create( parent, id, pos, size, style );
}

PanelLevelIIButtons::~PanelLevelIIButtons() {}

void PanelLevelIIButtons::Init() {
  m_cbArm = nullptr;
  m_btnCancel = nullptr;
  m_btnClose = nullptr;
  m_btnReCenter = nullptr;

  m_fButtonArmed = nullptr;
  m_fButtonReCenter = nullptr;
  m_fButtonCancel = nullptr;
  m_fButtonClose = nullptr;
}

bool PanelLevelIIButtons::Create( /*wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style*/
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style
) {

  SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
  wxPanel::Create( parent, id, pos, size, style );

  CreateControls();
  if ( GetSizer() ) {
    GetSizer()->SetSizeHints( this );
  }

  return true;
}

void PanelLevelIIButtons::CreateControls( void ) {

    PanelLevelIIButtons* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer1);

    m_cbArm = new wxCheckBox( itemPanel1, ID_CB_Arm, _("Arm"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbArm->SetValue(false);
    if (PanelLevelIIButtons::ShowToolTips())
        m_cbArm->SetToolTip(_("Arm For Trading"));
    m_cbArm->Enable(false);
    itemBoxSizer1->Add(m_cbArm, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnCancel = new wxButton( itemPanel1, ID_BTN_Cancel, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    if (PanelLevelIIButtons::ShowToolTips())
        m_btnCancel->SetToolTip(_("Cancel All Orders"));
    m_btnCancel->Enable(false);
    itemBoxSizer1->Add(m_btnCancel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnClose = new wxButton( itemPanel1, ID_BTN_Close, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
    if (PanelLevelIIButtons::ShowToolTips())
        m_btnClose->SetToolTip(_("Close All Orders"));
    m_btnClose->Enable(false);
    itemBoxSizer1->Add(m_btnClose, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    m_btnReCenter = new wxButton( itemPanel1, ID_BTN_ReCenter, _("Re-Center"), wxDefaultPosition, wxDefaultSize, 0 );
    if (PanelLevelIIButtons::ShowToolTips())
        m_btnReCenter->SetToolTip(_("Force recentering of ladder"));
    m_btnReCenter->Enable(false);
    itemBoxSizer1->Add(m_btnReCenter, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

  //Bind( wxEVT_SIZE, &PanelTrade::OnResize, this, GetId() );
  //Bind( wxEVT_SIZING, &PanelTrade::OnResizing, this, GetId() );
  //Bind( wxEVT_CLOSE, &PanelLevelIIButtons::OnDestroy, this, GetId() );
  Bind( wxEVT_DESTROY, &PanelLevelIIButtons::OnDestroy, this, GetId() );
  Bind( wxEVT_CHECKBOX, &PanelLevelIIButtons::OnCBArmClick, this, GetId() );
  Bind( wxEVT_BUTTON, &PanelLevelIIButtons::OnBTNCancelClick, this, GetId() );
  Bind( wxEVT_BUTTON, &PanelLevelIIButtons::OnBTNCloseClick, this, GetId() );
  Bind( wxEVT_BUTTON, &PanelLevelIIButtons::OnBTNReCenterClick, this, GetId() );

}

void PanelLevelIIButtons::Set( fButtonArmed_t&& fArmed, fButtonClick_t&& fRecenter, fButtonClick_t&& fCancel, fButtonClick_t&& fClose ) {
  m_fButtonArmed = std::move( fArmed );
  m_fButtonReCenter = std::move( fRecenter );
  m_fButtonCancel = std::move( fCancel );
  m_fButtonClose = std::move( fClose );
}

bool PanelLevelIIButtons::ShowToolTips() {
  return true;
}

void PanelLevelIIButtons::OnCBArmClick( wxCommandEvent& event ) {
  if ( m_fButtonArmed ) {
    m_fButtonArmed( m_cbArm->GetValue() );
  }
  event.Skip();
}

void PanelLevelIIButtons::OnBTNCancelClick( wxCommandEvent& event ) {
  if ( m_fButtonCancel ) {
    m_fButtonCancel();
  }
  event.Skip();
}

void PanelLevelIIButtons::OnBTNCloseClick( wxCommandEvent& event ) {
  if ( m_fButtonClose ) {
    m_fButtonClose();
  }
  event.Skip();
}

void PanelLevelIIButtons::OnBTNReCenterClick( wxCommandEvent& event ) {
  if ( m_fButtonReCenter ) {
    m_fButtonReCenter();
  }
  event.Skip();
}

//void PanelLevelIIButtons::OnCloseWindow( wxCloseEvent& event ) {
//  event.Skip();
//}

void PanelLevelIIButtons::OnDestroy( wxWindowDestroyEvent& event ) {
  event.Skip();
}



} // market depth
} // namespace tf
} // namespace ou
