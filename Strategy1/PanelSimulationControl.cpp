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

#include "StdAfx.h"

#include "PanelSimulationControl.h"

PanelSimulationControl::PanelSimulationControl(void) {
  Init();
}

PanelSimulationControl::PanelSimulationControl( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelSimulationControl::~PanelSimulationControl(void) {
}

bool PanelSimulationControl::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

  wxPanel::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer())
  {
      GetSizer()->SetSizeHints(this);
  }
  Centre();
  return true;
}
  
void PanelSimulationControl::Init() {
  m_txtInstrumentName = NULL;
  m_txtGroupDirectory = NULL;
  m_staticResult = NULL;
  m_gaugeProgress = NULL;
}

void PanelSimulationControl::CreateControls() {

    PanelSimulationControl* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    m_txtInstrumentName = new wxTextCtrl( itemPanel1, ID_TEXT_INSTRUMENTNAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(m_txtInstrumentName, 0, wxALIGN_LEFT|wxALL, 5);

    m_txtGroupDirectory = new wxTextCtrl( itemPanel1, ID_TEXT_GROUPDIRECTORY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(m_txtGroupDirectory, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 0, 0, 0);

    wxButton* itemButton6 = new wxButton( itemPanel1, ID_BTN_STARTSIM, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemButton6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_staticResult = new wxStaticText( itemPanel1, ID_STATIC_RESULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(m_staticResult, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_gaugeProgress = new wxGauge( itemPanel1, ID_GAUGE_PROGRESS, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
    m_gaugeProgress->SetValue(1);
    itemBoxSizer2->Add(m_gaugeProgress, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelSimulationControl::OnBtnStartSimulationClicked, this, ID_BTN_STARTSIM );
}

wxBitmap PanelSimulationControl::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon PanelSimulationControl::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}

void PanelSimulationControl::OnBtnStartSimulationClicked( wxCommandEvent& event ) {
  if ( 0 != m_OnStartSimulation ) m_OnStartSimulation();
}

