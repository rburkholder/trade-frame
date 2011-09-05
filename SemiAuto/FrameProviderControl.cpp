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

#include <wx/wx.h>

#include "FrameProviderControl.h"

FrameProviderControl::FrameProviderControl( void ) {
  Init();
}

// style: wxCAPTION | wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX
FrameProviderControl::FrameProviderControl(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
{
  Init();
  Create(parent, title, pos, size, style);
}

FrameProviderControl::~FrameProviderControl(void) {
}

void FrameProviderControl::Init( void ) {
  m_stateIQFeed = ProviderOff;
  m_stateIB = ProviderOff;
  m_stateSimulator = ProviderOff;
  m_btnIQFeed = m_btnIB = m_btnSimulator = 0;
}

bool FrameProviderControl::Create( wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style) {
  wxFrame::Create( parent, wxID_ANY, title, pos, size, style );
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

void FrameProviderControl::CreateControls( void ) {

  FrameProviderControl* itemFrame1 = this;

  wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
  itemFrame1->SetSizer(itemBoxSizer2);

  wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_LEFT|wxALL, 5);

  wxStaticBitmap* itemStaticBitmap4 = new wxStaticBitmap( itemFrame1, wxID_BitmapIQfeed, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer3->Add(itemStaticBitmap4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  m_btnIQFeed = new wxButton( itemFrame1, ID_BtnIQFeed, _("Turn On"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer3->Add(m_btnIQFeed, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  wxStaticText* itemStaticText6 = new wxStaticText( itemFrame1, wxID_LblIQFeed, _("IQ Feed"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer3->Add(itemStaticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer2->Add(itemBoxSizer7, 0, wxALIGN_LEFT|wxALL, 5);

  wxStaticBitmap* itemStaticBitmap8 = new wxStaticBitmap( itemFrame1, wxID_BitmapInteractiveBrokers, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer7->Add(itemStaticBitmap8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  m_btnIB = new wxButton( itemFrame1, ID_BtnInteractiveBrokers, _("Turn On"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer7->Add(m_btnIB, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  wxStaticText* itemStaticText10 = new wxStaticText( itemFrame1, wxID_LblInteractiveBrokers, _("Interactive Brokers"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer7->Add(itemStaticText10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
  itemBoxSizer2->Add(itemBoxSizer11, 0, wxALIGN_LEFT|wxALL, 5);

  wxStaticBitmap* itemStaticBitmap12 = new wxStaticBitmap( itemFrame1, wxID_BitmapSimulation, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer11->Add(itemStaticBitmap12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  m_btnSimulator = new wxButton( itemFrame1, ID_BtnSimulation, _("Turn On"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer11->Add(m_btnSimulator, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  wxStaticText* itemStaticText14 = new wxStaticText( itemFrame1, wxID_LblSimulation, _("Simulation"), wxDefaultPosition, wxDefaultSize, 0 );
  itemBoxSizer11->Add(itemStaticText14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  // Connect events and objects

  Bind( wxEVT_CLOSE_WINDOW, &FrameProviderControl::OnClose, this );

  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &FrameProviderControl::OnBtnIQFeed, this, ID_BtnIQFeed );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &FrameProviderControl::OnBtnIB, this, ID_BtnInteractiveBrokers );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &FrameProviderControl::OnBtnSimulation, this, ID_BtnSimulation );

}

void FrameProviderControl::OnClose( wxCloseEvent& event ) {
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

void FrameProviderControl::OnBtnCommon( wxCommandEvent& event, eProviderState_t& state ) {
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

void FrameProviderControl::OnBtnIQFeed( wxCommandEvent& event ) {
  OnBtnCommon( event, m_stateIQFeed );
  SetIQFeedState( m_stateIQFeed );
  if ( 0 != OnIQFeedStateChange ) 
    OnIQFeedStateChange( m_stateIQFeed );
}

void FrameProviderControl::OnBtnIB( wxCommandEvent& event ) {
  OnBtnCommon( event, m_stateIB );
  SetIBState( m_stateIB );
  if ( 0 != OnIBStateChange ) 
    OnIBStateChange( m_stateIB );
}

void FrameProviderControl::OnBtnSimulation( wxCommandEvent& event ) {
  OnBtnCommon( event, m_stateSimulator );
  SetSimulatorState( m_stateSimulator );
  if ( 0 != OnSimulatorStateChange ) 
    OnSimulatorStateChange( m_stateSimulator );
}

void FrameProviderControl::SetState( wxButton* btn, eProviderState_t state ) {
  switch ( state ) {
  case ProviderOff:
    btn->SetLabelText( "Turn On" );
    btn->Enable();
    break;
  case ProviderOn:
    btn->SetLabelText( "Turn Off" );
    btn->Enable();
    break;
  case ProviderGoingOn:
    btn->SetLabelText( "Going On" );
    btn->Disable();
    break;
  case ProviderGoingOff:
    btn->SetLabelText( "Going Off" );
    btn->Disable();
    break;
  }
}

void FrameProviderControl::SetIQFeedState( eProviderState_t state ) {
  SetState( m_btnIQFeed, state );
  m_stateIQFeed = state;
}

void FrameProviderControl::SetIBState( eProviderState_t state ) {
  SetState( m_btnIB, state );
  m_stateIB = state;
}

void FrameProviderControl::SetSimulatorState( eProviderState_t state ) {
  SetState( m_btnSimulator, state );
  m_stateSimulator = state;
}
