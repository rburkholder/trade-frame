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
 * File:    PanelProviderControlv2.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFVuTrading
 * Created: June 19, 2022 12:03
 */

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/tglbtn.h>
#include <wx/checkbox.h>

#include <TFTrading/ProviderManager.h>

#include "PanelProviderControlv2.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace v2 { // v2

struct ProviderWidgets {

  enum ProviderState { Off, GoingOn, On, GoingOff } m_state;

  using fCallBack_t = PanelProviderControl::fCallBack_t;
  using fSetExclusive_t = std::function<void(ProviderWidgets&)>;
  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;

  pProvider_t m_pProvider;

  wxWindow* m_owner;

  wxBoxSizer* m_sizer;
  wxCheckBox* m_cbD1;
  wxCheckBox* m_cbD2;
  wxCheckBox* m_cbX1;
  wxCheckBox* m_cbX2;
  wxToggleButton* m_btnState;
  wxStaticText* m_textProvider;

  fSetExclusive_t m_fSetD1;
  fSetExclusive_t m_fSetD2;
  fSetExclusive_t m_fSetX1;
  fSetExclusive_t m_fSetX2;

  fCallBack_t m_fConnecting;
  fCallBack_t m_fConnected;
  fCallBack_t m_fDisconnecting;
  fCallBack_t m_fDisconnected;

  ProviderWidgets(): m_state( ProviderState::Off ) {}
  ~ProviderWidgets(){
    ou::tf::ProviderManager::GlobalInstance().Release( m_pProvider->GetName() );
    m_pProvider->OnConnecting.Remove( MakeDelegate( this, &ProviderWidgets::Connecting ) );
    m_pProvider->OnConnected.Remove( MakeDelegate( this, &ProviderWidgets::Connected ) );
    m_pProvider->OnDisconnecting.Remove( MakeDelegate( this, &ProviderWidgets::Disconnecting ) );
    m_pProvider->OnDisconnected.Remove( MakeDelegate( this, &ProviderWidgets::Disonnected ) );
    m_pProvider.reset();
  }

  void Set(
    wxWindow* owner
  , pProvider_t pProvider
  , fCallBack_t&& fConnecting, fCallBack_t&& fConnected
  , fCallBack_t&& fDisconnecting, fCallBack_t&& fDisconnected
  ) {
    m_owner = owner;

    m_fConnecting = std::move( fConnecting );
    m_fConnected = std::move( fConnected );
    m_fDisconnecting = std::move( fDisconnecting );
    m_fDisconnected = std::move( fDisconnected );

    m_pProvider = pProvider;
    m_pProvider->OnConnecting.Add( MakeDelegate( this, &ProviderWidgets::Connecting ) );
    m_pProvider->OnConnected.Add( MakeDelegate( this, &ProviderWidgets::Connected ) );
    m_pProvider->OnDisconnecting.Add( MakeDelegate( this, &ProviderWidgets::Disconnecting ) );
    m_pProvider->OnDisconnected.Add( MakeDelegate( this, &ProviderWidgets::Disonnected ) );
    ou::tf::ProviderManager::GlobalInstance().Register( m_pProvider );
    Enable();
  }

  void Disable() {
    m_cbD1->Enable( false );
    m_cbD2->Enable( false );
    m_cbX1->Enable( false );
    m_cbX2->Enable( false );
    //UpdateProviderButton();
  }

  void Enable() {
    if ( m_pProvider->ProvidesBrokerInterface() ) {
      m_cbX1->Enable( true );
      m_cbX2->Enable( true );
    }
    if ( m_pProvider->ProvidesTrades() ) {
      m_cbD1->Enable( true );
      m_cbD2->Enable( true );
    }
    UpdateProviderButton();
  }

  void Set(
    fSetExclusive_t&& fSetD1,
    fSetExclusive_t&& fSetD2,
    fSetExclusive_t&& fSetX1,
    fSetExclusive_t&& fSetX2
  ) {
    m_fSetD1 = std::move( fSetD1 );
    m_fSetD2 = std::move( fSetD2 );
    m_fSetX1 = std::move( fSetX1 );
    m_fSetX2 = std::move( fSetX2 );
  }

  void SetState( ProviderState state ) {
    m_state = state;
    switch ( m_state ) {
    case ProviderState::Off:
      m_btnState->SetLabelText( "Off->On" );
      m_btnState->Enable();
      //if ( ( ProviderOff == m_stateIQFeed ) && ( ProviderOff == m_stateIB ) && ( ProviderOff == m_stateSimulator ) ) {
      //  EnableAllRadio();
      //}
      break;
    case ProviderState::GoingOn:
      m_btnState->SetLabelText( "Going On" );
      m_btnState->Disable();
      //if ( ( ProviderOff != m_stateIQFeed ) || ( ProviderOff != m_stateIB ) || ( ProviderOff != m_stateSimulator ) ) {
      //  DisableAllRadio();
      //}
      break;
    case ProviderState::On:
      m_btnState->SetLabelText( "On->Off" );
      m_btnState->Enable();
      break;
    case ProviderState::GoingOff:
      m_btnState->SetLabelText( "Going Off" );
      m_btnState->Disable();
      break;
    }
  }

  void Connecting( int ) { // handle event
    m_owner->CallAfter( [this](){ SetState( ProviderState::GoingOn ); });
    if ( m_fConnecting ) m_fConnecting();
  }

  void Connected( int ) { // handle event
    m_owner->CallAfter( [this](){ SetState( ProviderState::On ); });
    if ( m_fConnected ) m_fConnected();
  }

  void Disconnecting( int ) { // handle event
    m_owner->CallAfter( [this](){ SetState( ProviderState::GoingOff ); });
    if ( m_fDisconnecting ) m_fDisconnecting();
  }

  void Disonnected( int ) { // handle event
    m_owner->CallAfter( [this](){ SetState( ProviderState::Off ); });
    if ( m_fDisconnected ) m_fDisconnected();
  }

  void OnBtn( wxCommandEvent& event ) {
    if ( m_btnState->GetValue() ) {
      m_pProvider->Connect();
    }
    else {
      m_pProvider->Disconnect();
    }
  }

  void UpdateProviderButton() { // TODO: disable if connected
    bool bAnyEnabled = m_cbD1->IsEnabled() || m_cbD2->IsEnabled() || m_cbX1->IsEnabled() || m_cbX2->IsEnabled();
    if ( bAnyEnabled ) {
      bool bAnyChecked = m_cbD1->IsChecked() || m_cbD2->IsChecked() || m_cbX1->IsChecked() || m_cbX2->IsChecked();
      m_btnState->Enable( bAnyChecked );
      //std::cout << m_pProvider->GetName() << " " << bAnyChecked << std::endl;
    }
  }
};

// =======  PanelProviderControl

PanelProviderControl::PanelProviderControl() {
  Init();
}

PanelProviderControl::PanelProviderControl( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelProviderControl::~PanelProviderControl() {
}

bool PanelProviderControl::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  wxPanel::Create( parent, id, pos, size, style );
  CreateControls();
  if ( GetSizer() ) {
    GetSizer()->SetSizeHints(this);
  }
  return true;
}

void PanelProviderControl::Init() {
}

void PanelProviderControl::CreateControls() {

  PanelProviderControl* itemFrame1 = this;

  m_sizerPanel = new wxBoxSizer(wxVERTICAL);
  itemFrame1->SetSizer(m_sizerPanel);

}

void PanelProviderControl::Add(
  pProvider_t pProvider
, bool bD1, bool bD2, bool bX1, bool bX2
, fCallBack_t&& fConnecting, fCallBack_t&& fConnected
, fCallBack_t&& fDisconnecting, fCallBack_t&& fDisconnected
) {

  auto result = m_mapProviderWidgets.emplace( pProvider->ID(), std::make_shared<ProviderWidgets>() );
  assert( result.second );
  ProviderWidgets& widgets( *result.first->second );

  widgets.Set(
    [this]( ProviderWidgets& pw ){ // m_fD1
      for ( mapProviderWidgets_t::value_type& vt : m_mapProviderWidgets ) {
        vt.second->m_cbD1->SetValue( false );
        vt.second->UpdateProviderButton();
      }
      pw.m_cbD1->SetValue( true );
      pw.UpdateProviderButton();
      // call events ?
    },
    [this]( ProviderWidgets& pw ){ // m_fD2
      for ( mapProviderWidgets_t::value_type& vt : m_mapProviderWidgets ) {
        vt.second->m_cbD2->SetValue( false );
        vt.second->UpdateProviderButton();
      }
      pw.m_cbD2->SetValue( true );
      pw.UpdateProviderButton();
      // call events ?
    },
    [this]( ProviderWidgets& pw ){ // m_fX1
      for ( mapProviderWidgets_t::value_type& vt : m_mapProviderWidgets ) {
        vt.second->m_cbX1->SetValue( false );
        vt.second->UpdateProviderButton();
      }
      pw.m_cbX1->SetValue( true );
      pw.UpdateProviderButton();
      // call events ?
    },
    [this]( ProviderWidgets& pw ){ // m_fX2
      for ( mapProviderWidgets_t::value_type& vt : m_mapProviderWidgets ) {
        vt.second->m_cbX2->SetValue( false );
        vt.second->UpdateProviderButton();
      }
      pw.m_cbX2->SetValue( true );
      pw.UpdateProviderButton();
      // call events ?
    }
  );

  widgets.m_sizer = new wxBoxSizer(wxHORIZONTAL);
  m_sizerPanel->Add(widgets.m_sizer, 0, wxLEFT|wxTOP|wxBOTTOM, 2);

  widgets.m_cbD1 = new wxCheckBox( this, wxID_ANY, _("D1"), wxDefaultPosition, wxDefaultSize, 0 );
  widgets.m_sizer->Add(widgets.m_cbD1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 2);
  widgets.m_cbD1->Enable( false );
  Bind( wxEVT_CHECKBOX, [&widgets](wxCommandEvent& event){ widgets.m_fSetD1( widgets ); }, widgets.m_cbD1->GetId() );

  widgets.m_cbD2 = new wxCheckBox( this, wxID_ANY, _("D2"), wxDefaultPosition, wxDefaultSize, 0 );
  widgets.m_sizer->Add(widgets.m_cbD2, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 2);
  widgets.m_cbD2->Enable( false );
  Bind( wxEVT_CHECKBOX, [&widgets](wxCommandEvent& event){ widgets.m_fSetD2( widgets ); }, widgets.m_cbD2->GetId() );

  widgets.m_cbX1 = new wxCheckBox( this, wxID_ANY, _("X1"), wxDefaultPosition, wxDefaultSize, 0 );
  widgets.m_sizer->Add(widgets.m_cbX1, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 2);
  widgets.m_cbX1->Enable( false );
  Bind( wxEVT_CHECKBOX, [&widgets](wxCommandEvent& event){ widgets.m_fSetX1( widgets ); }, widgets.m_cbX1->GetId() );

  widgets.m_cbX2 = new wxCheckBox( this, wxID_ANY, _("X2"), wxDefaultPosition, wxDefaultSize, 0 );
  widgets.m_sizer->Add(widgets.m_cbX2, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 2);
  widgets.m_cbX2->Enable( false );
  Bind( wxEVT_CHECKBOX, [&widgets](wxCommandEvent& event){ widgets.m_fSetX2( widgets ); }, widgets.m_cbX2->GetId() );

  widgets.m_btnState = new wxToggleButton( this, wxID_ANY, _("Turn On"), wxDefaultPosition, wxDefaultSize, 0 );
  widgets.m_sizer->Add(widgets.m_btnState, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 2 );
  widgets.m_btnState->Enable( false );
  Bind(
    wxEVT_TOGGLEBUTTON,
    [&widgets](wxCommandEvent& event){
      widgets.OnBtn( event );
      event.Skip();
    },
    widgets.m_btnState->GetId()
  );

  widgets.m_textProvider = new wxStaticText( this, wxID_ANY, _(pProvider->GetName()), wxDefaultPosition, wxDefaultSize, 0 );
  widgets.m_sizer->Add(widgets.m_textProvider, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  if ( GetSizer() ) {
    GetSizer()->SetSizeHints(this);
  }

  widgets.Set(
    this
  , pProvider
  , std::move( fConnecting ), std::move( fConnected )
  , std::move( fDisconnecting ), std::move( fDisconnected )
  );

  if ( bD1 ) widgets.m_fSetD1( widgets );
  if ( bD2 ) widgets.m_fSetD2( widgets );
  if ( bX1 ) widgets.m_fSetX1( widgets );
  if ( bX2 ) widgets.m_fSetX2( widgets );

  // TODO: if any provider button enabled, disable all radio buttons

}

void PanelProviderControl::UpdateProviderButtons() {
  for ( mapProviderWidgets_t::value_type& vt : m_mapProviderWidgets ) {
    vt.second->UpdateProviderButton();
  }
}

} // namespace v2
} // namespace tf
} // namespace ou
