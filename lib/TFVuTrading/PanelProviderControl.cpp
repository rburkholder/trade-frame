/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include "PanelProviderControl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

wxDEFINE_EVENT( EVT_ProviderIB, UpdateProviderStatusEvent );
wxDEFINE_EVENT( EVT_ProviderIQFeed, UpdateProviderStatusEvent );
wxDEFINE_EVENT( EVT_ProviderSimulator, UpdateProviderStatusEvent );

PanelProviderControl::PanelProviderControl(void) {
  Init();
}

PanelProviderControl::PanelProviderControl( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelProviderControl::~PanelProviderControl(void) {
}

bool PanelProviderControl::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  wxPanel::Create( parent, id, pos, size, style );
  CreateControls();
  SetIQFeedState( m_stateIQFeed );
  SetIBState( m_stateIB );
  SetSimulatorState( m_stateSimulator );
  if (GetSizer())
  {
      GetSizer()->SetSizeHints(this);
  }
//  Centre();
  return true;
}

void PanelProviderControl::Init( void ) {
//  m_nProvidersOn = 0;
//  m_bDisabling = false;
    m_cbIQFeedD2 = NULL;
    m_cbIQFeedX = NULL;
    m_btnIQFeed = NULL;
    m_cbIBD1 = NULL;
    m_cbIBD2 = NULL;
    m_cbIBX = NULL;
    m_btnIB = NULL;
    m_cbSimD1 = NULL;
    m_cbSimD2 = NULL;
    m_cbSimX = NULL;
    m_btnSimulator = NULL;
  m_stateIQFeed = ProviderOff;
  m_stateIB = ProviderOff;
  m_stateSimulator = ProviderOff;
}

void PanelProviderControl::CreateControls( void ) {

  // radio buttons could be auto-enabled based upon how provider indicates data, execution capability flags

  PanelProviderControl* itemFrame1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_LEFT|wxALL, 5);

    m_cbIQFeedD1 = new wxCheckBox( itemFrame1, ID_CB_IQF_D1, _("D1"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIQFeedD1->SetValue(true);
    itemBoxSizer3->Add(m_cbIQFeedD1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_cbIQFeedD2 = new wxCheckBox( itemFrame1, ID_CB_IQF_D2, _("D2"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIQFeedD2->SetValue(false);
    itemBoxSizer3->Add(m_cbIQFeedD2, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_cbIQFeedX = new wxCheckBox( itemFrame1, ID_CB_IQF_X, _("X"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIQFeedX->SetValue(false);
    m_cbIQFeedX->Enable(false);
    itemBoxSizer3->Add(m_cbIQFeedX, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnIQFeed = new wxButton( itemFrame1, ID_BtnIQFeed, _("Turn On"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(m_btnIQFeed, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( itemFrame1, wxID_LblIQFeed, _("IQF"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_LEFT|wxALL, 5);

    m_cbIBD1 = new wxCheckBox( itemFrame1, ID_CB_IB_D1, _("D1"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIBD1->SetValue(false);
    itemBoxSizer9->Add(m_cbIBD1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_cbIBD2 = new wxCheckBox( itemFrame1, ID_CB_IB_D2, _("D2"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIBD2->SetValue(true);
    itemBoxSizer9->Add(m_cbIBD2, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_cbIBX = new wxCheckBox( itemFrame1, ID_CB_IB_X, _("X"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIBX->SetValue(true);
    itemBoxSizer9->Add(m_cbIBX, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnIB = new wxButton( itemFrame1, ID_BtnInteractiveBrokers, _("Turn On"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(m_btnIB, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText14 = new wxStaticText( itemFrame1, wxID_LblInteractiveBrokers, _("IB"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemStaticText14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer15, 0, wxALIGN_LEFT|wxALL, 5);

    m_cbSimD1 = new wxCheckBox( itemFrame1, ID_CB_SIM_D1, _("D1"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbSimD1->SetValue(false);
    itemBoxSizer15->Add(m_cbSimD1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_cbSimD2 = new wxCheckBox( itemFrame1, ID_CB_SIM_D2, _("D2"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbSimD2->SetValue(false);
    m_cbSimD2->Enable(false);
    itemBoxSizer15->Add(m_cbSimD2, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_cbSimX = new wxCheckBox( itemFrame1, ID_CB_SIM_X, _("X"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbSimX->SetValue(false);
    itemBoxSizer15->Add(m_cbSimX, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnSimulator = new wxButton( itemFrame1, ID_BtnSimulation, _("Turn On"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer15->Add(m_btnSimulator, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText20 = new wxStaticText( itemFrame1, wxID_LblSimulation, _("Sim"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer15->Add(itemStaticText20, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  // Connect events and objects

//  Bind( wxEVT_CLOSE_WINDOW, &PanelProviderControl::OnClose, this );  // doesn't get called, as is not frame, need to do in frame

  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelProviderControl::OnBtnIQFeed, this, ID_BtnIQFeed );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelProviderControl::OnBtnIB, this, ID_BtnInteractiveBrokers );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelProviderControl::OnBtnSimulation, this, ID_BtnSimulation );

  Bind( EVT_ProviderIB, &PanelProviderControl::OnIBState, this );
  Bind( EVT_ProviderIQFeed, &PanelProviderControl::OnIQFeedState, this );
  Bind( EVT_ProviderSimulator, &PanelProviderControl::OnSimulatorState, this );

  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelProviderControl::OnBtnD1IQFeed, this, ID_CB_IQF_D1 );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelProviderControl::OnBtnD2IQFeed, this, ID_CB_IQF_D2 );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelProviderControl::OnBtnXIQFeed, this, ID_CB_IQF_X );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelProviderControl::OnBtnD1IB, this, ID_CB_IB_D1 );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelProviderControl::OnBtnD2IB, this, ID_CB_IB_D2 );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelProviderControl::OnBtnXIB, this, ID_CB_IB_X );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelProviderControl::OnBtnD1Sim, this, ID_CB_SIM_D1 );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelProviderControl::OnBtnD2Sim, this, ID_CB_SIM_D2 );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelProviderControl::OnBtnXSim, this, ID_CB_SIM_X );

}

void PanelProviderControl::OnClose( wxCloseEvent& event ) {
  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

void PanelProviderControl::UpdateProviderButtons( void ) {
  m_btnIQFeed->Enable( m_cbIQFeedD1->IsChecked() || m_cbIQFeedD2->IsChecked() || m_cbIQFeedX->IsChecked() );
  m_btnIB->Enable( m_cbIBD1->IsChecked() || m_cbIBD2->IsChecked() || m_cbIBX->IsChecked() );
  m_btnSimulator->Enable( m_cbSimD1->IsChecked() || m_cbSimD2->IsChecked() || m_cbSimX->IsChecked() );
}

void PanelProviderControl::SyncInitialState( void ) {
  if ( m_cbIQFeedD1->IsChecked() ) if ( 0 != OnProviderSelectD1 ) OnProviderSelectD1( EIQFeed );
  if ( m_cbIQFeedD2->IsChecked() ) if ( 0 != OnProviderSelectD2 ) OnProviderSelectD2( EIQFeed );
  if ( m_cbIQFeedX->IsChecked() ) if ( 0 != OnProviderSelectX ) OnProviderSelectX( EIQFeed );

  if ( m_cbIBD1->IsChecked() ) if ( 0 != OnProviderSelectD1 ) OnProviderSelectD1( EIB );
  if ( m_cbIBD2->IsChecked() ) if ( 0 != OnProviderSelectD2 ) OnProviderSelectD2( EIB );
  if ( m_cbIBX->IsChecked() ) if ( 0 != OnProviderSelectX ) OnProviderSelectX( EIB );

  if ( m_cbSimD1->IsChecked() ) if ( 0 != OnProviderSelectD1 ) OnProviderSelectD1( ESim );
  if ( m_cbSimD2->IsChecked() ) if ( 0 != OnProviderSelectD2 ) OnProviderSelectD2( ESim );
  if ( m_cbSimX->IsChecked() ) if ( 0 != OnProviderSelectX ) OnProviderSelectX( ESim );

  UpdateProviderButtons();
}

void PanelProviderControl::OnBtnCommon( wxCommandEvent& event, eProviderState_t& state ) {
  switch ( state ) {
  case ProviderOff:
    state = ProviderGoingOn;
    break;
  case ProviderOn:
    state = ProviderGoingOff;
    break;
  default:;
    // other states non-functional till change confirmed
  }
}

void PanelProviderControl::OnBtnIQFeed( wxCommandEvent& event ) {
  OnBtnCommon( event, m_stateIQFeed );
  SetIQFeedState( m_stateIQFeed );
  if ( 0 != OnIQFeedStateChange ) 
    OnIQFeedStateChange( m_stateIQFeed );
}

void PanelProviderControl::OnBtnIB( wxCommandEvent& event ) {
  OnBtnCommon( event, m_stateIB );
  SetIBState( m_stateIB );
  if ( 0 != OnIBStateChange ) 
    OnIBStateChange( m_stateIB );
}

void PanelProviderControl::OnBtnSimulation( wxCommandEvent& event ) {
  OnBtnCommon( event, m_stateSimulator );
  SetSimulatorState( m_stateSimulator );
  if ( 0 != OnSimulatorStateChange ) 
    OnSimulatorStateChange( m_stateSimulator );
}

void PanelProviderControl::SetState( wxButton* btn, eProviderState_t state ) {
  switch ( state ) {
  case ProviderOff:
    btn->SetLabelText( "Turn On" );
    btn->Enable();
    if ( ( ProviderOff == m_stateIQFeed ) && ( ProviderOff == m_stateIB ) && ( ProviderOff == m_stateSimulator ) ) {
      EnableAllRadio();
    }
    break;
  case ProviderOn:
    btn->SetLabelText( "Turn Off" );
    btn->Enable();
    break;
  case ProviderGoingOn:
    btn->SetLabelText( "Going On" );
    btn->Disable();
    if ( ( ProviderOff != m_stateIQFeed ) || ( ProviderOff != m_stateIB ) || ( ProviderOff != m_stateSimulator ) ) {
      DisableAllRadio();
    }
    break;
  case ProviderGoingOff:
    btn->SetLabelText( "Going Off" );
    btn->Disable();
    break;
  }
}

void PanelProviderControl::SetIQFeedState( eProviderState_t state ) {
  m_stateIQFeed = state;
  SetState( m_btnIQFeed, state );
}

void PanelProviderControl::SetIBState( eProviderState_t state ) {
  m_stateIB = state;
  SetState( m_btnIB, state );
}

void PanelProviderControl::SetSimulatorState( eProviderState_t state ) {
  m_stateSimulator = state;
  SetState( m_btnSimulator, state );
}

void PanelProviderControl::OnIQFeedState( UpdateProviderStatusEvent& event ) {
  SetIQFeedState( event.GetState() );
}

void PanelProviderControl::OnIBState( UpdateProviderStatusEvent& event ) {
  SetIBState( event.GetState() );
}

void PanelProviderControl::OnSimulatorState( UpdateProviderStatusEvent& event ) {
  SetSimulatorState( event.GetState() );
}

void PanelProviderControl::OnBtnD1IQFeed( wxCommandEvent& event ) {
  ResetAllRadioData1();
  m_cbIQFeedD1->SetValue( true );
  UpdateProviderButtons();
  if ( 0 != OnProviderSelectD1 ) OnProviderSelectD1( EIQFeed );
}

void PanelProviderControl::OnBtnD2IQFeed( wxCommandEvent& event ) {
  ResetAllRadioData2();
  m_cbIQFeedD2->SetValue( true );
  UpdateProviderButtons();
  if ( 0 != OnProviderSelectD2 ) OnProviderSelectD2( EIQFeed );
}

void PanelProviderControl::OnBtnXIQFeed( wxCommandEvent& event ) {
  ResetAllRadioExec();
  //m_btnXIQFeed->SetValue( true );
  UpdateProviderButtons();
  //if ( 0 != OnProviderSelectX ) OnProviderSelectX( EIQFeed );
}

void PanelProviderControl::OnBtnD1IB( wxCommandEvent& event ) {
  ResetAllRadioData1();
  m_cbIBD1->SetValue( true );
  UpdateProviderButtons();
  if ( 0 != OnProviderSelectD1 ) OnProviderSelectD1( EIB );
}

void PanelProviderControl::OnBtnD2IB( wxCommandEvent& event ) {
  ResetAllRadioData2();
  m_cbIBD2->SetValue( true );
  UpdateProviderButtons();
  if ( 0 != OnProviderSelectD2 ) OnProviderSelectD2( EIB );
}

void PanelProviderControl::OnBtnXIB( wxCommandEvent& event ) {
  ResetAllRadioExec();
  m_cbIBX->SetValue( true );
  UpdateProviderButtons();
  if ( 0 != OnProviderSelectX ) OnProviderSelectX( EIB );
}

void PanelProviderControl::OnBtnD1Sim( wxCommandEvent& event ) {
  ResetAllRadioData1();
  m_cbSimD1->SetValue( true );
  UpdateProviderButtons();
  if ( 0 != OnProviderSelectD1 ) OnProviderSelectD1( ESim );
}

void PanelProviderControl::OnBtnD2Sim( wxCommandEvent& event ) {
  ResetAllRadioData2();
  //m_cbSimD2->SetValue( true );
  UpdateProviderButtons();
  //if ( 0 != OnProviderSelectD2 ) OnProviderSelectD2( ESim );
}

void PanelProviderControl::OnBtnXSim( wxCommandEvent& event ) {
  ResetAllRadioExec();
  m_cbSimX->SetValue( true );
  UpdateProviderButtons();
  if ( 0 != OnProviderSelectX ) OnProviderSelectX( ESim );
}

void PanelProviderControl::ResetAllRadioData1( void ) {
  m_cbIQFeedD1->SetValue( false );
  m_cbIBD1->SetValue( false );
  m_cbSimD1->SetValue( false );
}

void PanelProviderControl::ResetAllRadioData2( void ) {
  m_cbIQFeedD2->SetValue( false );
  m_cbIBD2->SetValue( false );
  m_cbSimD2->SetValue( false );
}

void PanelProviderControl::ResetAllRadioExec( void ) {
  m_cbIQFeedX->SetValue( false );
  m_cbIBX->SetValue( false );
  m_cbSimX->SetValue( false );
}

void PanelProviderControl::EnableAllRadio( void ) {
  m_cbIQFeedD1->Enable();
  m_cbIQFeedD2->Enable();
  //m_cbIQFeedX->Enable();  // IQFeed does not have execution ability
  m_cbIBD1->Enable();
  m_cbIBD2->Enable();
  m_cbIBX->Enable();
  m_cbSimD1->Enable();
  //m_cbSimD2->Enable();  // special provider specific
  m_cbSimX->Enable();
}

void PanelProviderControl::DisableAllRadio( void ) {
  m_cbIQFeedD1->Disable();
  m_cbIQFeedD2->Disable();
//  m_cbIQFeedX->Disable();
  m_cbIBD1->Disable();
  m_cbIBD2->Disable();
  m_cbIBX->Disable();
  m_cbSimD1->Disable();
//  m_cbSimD2->Disable();
  m_cbSimX->Disable();
}

} // namespace tf
} // namespace ou
