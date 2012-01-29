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
  m_bDisabling = false;
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

    m_btnD1IQFeed = new wxRadioButton( itemFrame1, ID_RBTN_D1_IQF, _("D1"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnD1IQFeed->SetValue(false);
    itemBoxSizer3->Add(m_btnD1IQFeed, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_btnD2IQFeed = new wxRadioButton( itemFrame1, ID_RBTN_D2_IQF, _("D2"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnD2IQFeed->SetValue(false);
    itemBoxSizer3->Add(m_btnD2IQFeed, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnXIQFeed = new wxRadioButton( itemFrame1, ID_RBTN_X_IQF, _("X"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnXIQFeed->SetValue(false);
    m_btnXIQFeed->Enable(false);
    itemBoxSizer3->Add(m_btnXIQFeed, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnIQFeed = new wxButton( itemFrame1, ID_BtnIQFeed, _("Turn On"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );  // fixes radio button interference?
    itemBoxSizer3->Add(m_btnIQFeed, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( itemFrame1, wxID_LblIQFeed, _("IQF"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_LEFT|wxALL, 5);

    m_btnD1IB = new wxRadioButton( itemFrame1, ID_RBTN_D1_IB, _("D1"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnD1IB->SetValue(false);
    itemBoxSizer9->Add(m_btnD1IB, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_btnD2IB = new wxRadioButton( itemFrame1, ID_RBTN_D2_IB, _("D2"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnD2IB->SetValue(false);
    itemBoxSizer9->Add(m_btnD2IB, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnXIB = new wxRadioButton( itemFrame1, ID_RBTN_X_IB, _("X"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnXIB->SetValue(false);
    itemBoxSizer9->Add(m_btnXIB, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnIB = new wxButton( itemFrame1, ID_BtnInteractiveBrokers, _("Turn On"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );  // fixes radio button interference?
    itemBoxSizer9->Add(m_btnIB, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText14 = new wxStaticText( itemFrame1, wxID_LblInteractiveBrokers, _("IB"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemStaticText14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer15, 0, wxALIGN_LEFT|wxALL, 5);

    m_btnD1Simulator = new wxRadioButton( itemFrame1, ID_RBTN_D1_SIM, _("D1"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnD1Simulator->SetValue(false);
    itemBoxSizer15->Add(m_btnD1Simulator, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_btnD2Simulator = new wxRadioButton( itemFrame1, ID_RBTN_D2_SIM, _("D2"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnD2Simulator->SetValue(false);
    m_btnD2Simulator->Enable(false);
    itemBoxSizer15->Add(m_btnD2Simulator, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnXSimulator = new wxRadioButton( itemFrame1, ID_RBTN_X_SIM, _("X"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnXSimulator->SetValue(false);
    itemBoxSizer15->Add(m_btnXSimulator, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnSimulator = new wxButton( itemFrame1, ID_BtnSimulation, _("Turn On"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );  // fixes radio button interference?
    itemBoxSizer15->Add(m_btnSimulator, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText20 = new wxStaticText( itemFrame1, wxID_LblSimulation, _("Sim"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer15->Add(itemStaticText20, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  // Connect events and objects

  Bind( wxEVT_CLOSE_WINDOW, &PanelProviderControl::OnClose, this );

  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelProviderControl::OnBtnIQFeed, this, ID_BtnIQFeed );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelProviderControl::OnBtnIB, this, ID_BtnInteractiveBrokers );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelProviderControl::OnBtnSimulation, this, ID_BtnSimulation );

  Bind( EVT_ProviderIB, &PanelProviderControl::OnIBState, this );
  Bind( EVT_ProviderIQFeed, &PanelProviderControl::OnIQFeedState, this );
  Bind( EVT_ProviderSimulator, &PanelProviderControl::OnSimulatorState, this );

  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelProviderControl::OnRadioD1IQFeed, this, ID_RBTN_D1_IQF );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelProviderControl::OnRadioD2IQFeed, this, ID_RBTN_D2_IQF );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelProviderControl::OnRadioXIQFeed, this, ID_RBTN_X_IQF );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelProviderControl::OnRadioD1IB, this, ID_RBTN_D1_IB );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelProviderControl::OnRadioD2IB, this, ID_RBTN_D2_IB );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelProviderControl::OnRadioXIB, this, ID_RBTN_X_IB );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelProviderControl::OnRadioD1Sim, this, ID_RBTN_D1_SIM );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelProviderControl::OnRadioD2Sim, this, ID_RBTN_D2_SIM );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelProviderControl::OnRadioXSim, this, ID_RBTN_X_SIM );

}

void PanelProviderControl::OnClose( wxCloseEvent& event ) {
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
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
    m_bDisabling = true;  // fixes some sort of gui auto update thing I don't like
    btn->Disable();
    m_bDisabling = false;
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

void PanelProviderControl::OnRadioD1IQFeed( wxCommandEvent& event ) {
  if ( !m_bDisabling ) {
    ResetAllRadioData1();
    m_btnD1IQFeed->SetValue( true );
    if ( 0 != OnProviderSelectD1 ) OnProviderSelectD1( EIQFeed );
  }
}

void PanelProviderControl::OnRadioD2IQFeed( wxCommandEvent& event ) {
  if ( !m_bDisabling ) {
    ResetAllRadioData2();
    m_btnD2IQFeed->SetValue( true );
    if ( 0 != OnProviderSelectD2 ) OnProviderSelectD2( EIQFeed );
  }
}

void PanelProviderControl::OnRadioXIQFeed( wxCommandEvent& event ) {
  if ( !m_bDisabling ) {
    ResetAllRadioExec();
    //m_btnXIQFeed->SetValue( true );
    //if ( 0 != OnProviderSelectX ) OnProviderSelectX( EIQFeed );
  }
}

void PanelProviderControl::OnRadioD1IB( wxCommandEvent& event ) {
  if ( !m_bDisabling ) {
    ResetAllRadioData1();
    m_btnD1IB->SetValue( true );
    if ( 0 != OnProviderSelectD1 ) OnProviderSelectD1( EIB );
  }
}

void PanelProviderControl::OnRadioD2IB( wxCommandEvent& event ) {
  if ( !m_bDisabling ) {
    ResetAllRadioData2();
    m_btnD2IB->SetValue( true );
    if ( 0 != OnProviderSelectD2 ) OnProviderSelectD2( EIB );
  }
}

void PanelProviderControl::OnRadioXIB( wxCommandEvent& event ) {
  if ( !m_bDisabling ) {
    ResetAllRadioExec();
    m_btnXIB->SetValue( true );
    if ( 0 != OnProviderSelectX ) OnProviderSelectX( EIB );
  }
}

void PanelProviderControl::OnRadioD1Sim( wxCommandEvent& event ) {
  ResetAllRadioData1();
  m_btnD1Simulator->SetValue( true );
  if ( 0 != OnProviderSelectD1 ) OnProviderSelectD1( ESim );
}

void PanelProviderControl::OnRadioD2Sim( wxCommandEvent& event ) {
  ResetAllRadioData2();
  //m_btnD2Simulator->SetValue( true );
  //if ( 0 != OnProviderSelectD2 ) OnProviderSelectD2( ESim );
}

void PanelProviderControl::OnRadioXSim( wxCommandEvent& event ) {
  if ( !m_bDisabling ) {
    ResetAllRadioExec();
    m_btnXSimulator->SetValue( true );
    if ( 0 != OnProviderSelectX ) OnProviderSelectX( ESim );
  }
}

void PanelProviderControl::ResetAllRadioData1( void ) {
  m_btnD1IQFeed->SetValue( false );
  m_btnD1IB->SetValue( false );
  m_btnD1Simulator->SetValue( false );
}

void PanelProviderControl::ResetAllRadioData2( void ) {
  m_btnD2IQFeed->SetValue( false );
  m_btnD2IB->SetValue( false );
  m_btnD2Simulator->SetValue( false );
}

void PanelProviderControl::ResetAllRadioExec( void ) {
  m_btnXIQFeed->SetValue( false );
  m_btnXIB->SetValue( false );
  m_btnXSimulator->SetValue( false );
}

void PanelProviderControl::EnableAllRadio( void ) {
  m_btnD1IQFeed->Enable();
  m_btnD2IQFeed->Enable();
  //m_btnXIQFeed->Enable();  // IQFeed does not have execution ability
  m_btnD1IB->Enable();
  m_btnD2IB->Enable();
  m_btnXIB->Enable();
  m_btnD1Simulator->Enable();
  //m_btnD2Simulator->Enable();  // special provider specific
  m_btnXSimulator->Enable();
}

void PanelProviderControl::DisableAllRadio( void ) {
  m_bDisabling = true;  // fixes some sort of gui auto update thing I don't like
  m_btnD1IQFeed->Disable();
  m_btnD2IQFeed->Disable();
//  m_btnXIQFeed->Disable();
  m_btnD1IB->Disable();
  m_btnD2IB->Disable();
  m_btnXIB->Disable();
  m_btnD1Simulator->Disable();
//  m_btnD2Simulator->Disable();
  m_btnXSimulator->Disable();
  m_bDisabling = false;
}


} // namespace tf
} // namespace ou
