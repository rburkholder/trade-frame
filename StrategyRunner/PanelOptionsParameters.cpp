/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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
#include "PanelOptionsParameters.h"

PanelOptionsParameters::PanelOptionsParameters(void) {
  Init();
}

PanelOptionsParameters::PanelOptionsParameters( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelOptionsParameters::~PanelOptionsParameters(void) {
}

bool PanelOptionsParameters::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

  wxPanel::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer())   {
      GetSizer()->SetSizeHints(this);
  }
  Centre();
  return true;
}

void PanelOptionsParameters::Init() {
  m_txtUnderlying = NULL;
  m_ctrlNearDate = NULL;
  m_ctrlFarDate = NULL;
}

void PanelOptionsParameters::CreateControls() {    

    PanelOptionsParameters* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_RIGHT|wxALL, 5);

    wxStaticText* itemStaticText4 = new wxStaticText( itemPanel1, ID_LBL_Underlying, _("Underlying:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    itemBoxSizer3->Add(itemStaticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txtUnderlying = new wxTextCtrl( itemPanel1, ID_TXT_Underlying, _("GLD"), wxDefaultPosition, wxDefaultSize, 0 );
    m_txtUnderlying->SetMaxLength(10);
    itemBoxSizer3->Add(m_txtUnderlying, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer6, 0, wxALIGN_RIGHT|wxALL, 5);

    wxStaticText* itemStaticText7 = new wxStaticText( itemPanel1, ID_LBL_OPTIONNEARDATE, _("Near Date:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    itemBoxSizer6->Add(itemStaticText7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ctrlNearDate = new wxDatePickerCtrl( itemPanel1, ID_DATE_NearDate, wxDateTime(), wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT );
    itemBoxSizer6->Add(m_ctrlNearDate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_RIGHT|wxALL, 5);

    wxStaticText* itemStaticText10 = new wxStaticText( itemPanel1, ID_LBL_OPTIONFARDATE, _("Far Date:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    itemBoxSizer9->Add(itemStaticText10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ctrlFarDate = new wxDatePickerCtrl( itemPanel1, ID_DATE_FarDate, wxDateTime(), wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT );
    itemBoxSizer9->Add(m_ctrlFarDate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnStart = new wxButton( itemPanel1, ID_BTN_START, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(m_btnStart, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOptionsParameters::OnBtnStartClicked, this, ID_BTN_START );

}

void PanelOptionsParameters::OnBtnStartClicked( wxCommandEvent& event ) {
  if ( 0 != m_OnStart ) m_OnStart();
}

void PanelOptionsParameters::SetBtnStartEnable( bool b ) {
  m_btnStart->Enable( b );
}

std::string PanelOptionsParameters::GetUnderlying( void ) {
  wxString s( m_txtUnderlying->GetLineText( 0 ) );
  return s.ToStdString();
}

boost::gregorian::date PanelOptionsParameters::GetOptionFarDate( void ) {
  wxDateTime d1 = m_ctrlFarDate->GetValue();
  boost::gregorian::date d2( d1.GetYear(), d1.GetMonth() + 1, d1.GetDay() );
  return d2;
}

boost::gregorian::date PanelOptionsParameters::GetOptionNearDate( void ) {
  wxDateTime d1 = m_ctrlNearDate->GetValue();
  boost::gregorian::date d2( d1.GetYear(), d1.GetMonth() + 1, d1.GetDay() );
  return d2;
}

void PanelOptionsParameters::SetOptionFarDate( boost::gregorian::date date ) {
  wxDateTime dt( date.day(), static_cast<wxDateTime::Month>( date.month() - 1 ), date.year(), 0, 0, 0 );
  m_ctrlFarDate->SetValue( dt );
}

void PanelOptionsParameters::SetOptionNearDate( boost::gregorian::date date ) {
  wxDateTime dt( date.day(), static_cast<wxDateTime::Month>( date.month() - 1 ), date.year(), 0, 0, 0 );
  m_ctrlNearDate->SetValue( dt );
}


wxBitmap PanelOptionsParameters::GetBitmapResource( const wxString& name ) {
    // Bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelOptionsParameters::GetIconResource( const wxString& name ) {
    // Icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
}
