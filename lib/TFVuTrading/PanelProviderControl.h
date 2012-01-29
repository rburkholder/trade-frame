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

#pragma once

#include <wx/panel.h>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

// wxFrame (wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE, const wxString &name=wxFrameNameStr)

// D1: data provider 1, could be iqfeed
// D2: data provider 2, could be ib, 
// X: execution provider

namespace ou { // One Unified
namespace tf { // TradeFrame

enum eProviderState_t { ProviderOff, ProviderGoingOn, ProviderOn, ProviderGoingOff };

class UpdateProviderStatusEvent: public wxEvent {
public:
  UpdateProviderStatusEvent( wxEventType eventType, eProviderState_t state )
    : wxEvent( 0, eventType ), m_state( state ) {
  }
  UpdateProviderStatusEvent( const UpdateProviderStatusEvent& event ): wxEvent( *this ), m_state( event.m_state ) {};
  ~UpdateProviderStatusEvent( void ) {};
  UpdateProviderStatusEvent* Clone( void ) const { return new UpdateProviderStatusEvent( *this ); }
  eProviderState_t GetState( void ) const { return m_state; };
protected:
private:
  eProviderState_t m_state;
};

wxDECLARE_EVENT( EVT_ProviderIB, UpdateProviderStatusEvent );
wxDECLARE_EVENT( EVT_ProviderIQFeed, UpdateProviderStatusEvent );
wxDECLARE_EVENT( EVT_ProviderSimulator, UpdateProviderStatusEvent );

// ===================================

#define SYMBOL_PANELPROVIDERCONTROL_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELPROVIDERCONTROL_TITLE _("Provider Control")
#define SYMBOL_PANELPROVIDERCONTROL_IDNAME ID_PANELPROVIDERCONTROL
#define SYMBOL_PANELPROVIDERCONTROL_POSITION wxDefaultPosition
//#define SYMBOL_PANELPROVIDERCONTROL_SIZE wxSize(400, 300)
#define SYMBOL_PANELPROVIDERCONTROL_SIZE wxDefaultSize

class PanelProviderControl:  public wxPanel {
public:

  enum Provider_t {
    EIQFeed, EIB, ESim
  };

  PanelProviderControl(void);
  PanelProviderControl( 
    wxWindow* parent, 
    wxWindowID id = SYMBOL_PANELPROVIDERCONTROL_IDNAME, 
    const wxPoint& pos = SYMBOL_PANELPROVIDERCONTROL_POSITION, 
    const wxSize& size = SYMBOL_PANELPROVIDERCONTROL_SIZE, 
    long style = SYMBOL_PANELPROVIDERCONTROL_STYLE );
  ~PanelProviderControl(void);

  bool Create( 
    wxWindow* parent, 
    wxWindowID id = SYMBOL_PANELPROVIDERCONTROL_IDNAME, 
    const wxPoint& pos = SYMBOL_PANELPROVIDERCONTROL_POSITION, 
    const wxSize& size = SYMBOL_PANELPROVIDERCONTROL_SIZE, 
    long style = SYMBOL_PANELPROVIDERCONTROL_STYLE );
  
//  wxBitmap GetBitmapResource( const wxString& name );
//  wxIcon GetIconResource( const wxString& name );

  void SetIQFeedState( eProviderState_t state );
  void SetIBState( eProviderState_t state );
  void SetSimulatorState( eProviderState_t state );

  void QueueEvent( wxEvent* event ) { wxEvtHandler::QueueEvent( event ); };

  typedef FastDelegate1<eProviderState_t> OnProviderStateChange_t;

  void SetOnIBStateChangeHandler( OnProviderStateChange_t function ) {
    OnIBStateChange = function;
  }
  void SetOnIQFeedStateChangeHandler( OnProviderStateChange_t function ) {
    OnIQFeedStateChange = function;
  }
  void SetOnSimulatorStateChangeHandler( OnProviderStateChange_t function ) {
    OnSimulatorStateChange = function;
  }

  typedef FastDelegate1<Provider_t> OnProviderSelect_t;

  void SetOnProviderSelectD1Handler( OnProviderSelect_t function ) {
    OnProviderSelectD1 = function;
  }

  void SetOnProviderSelectD2Handler( OnProviderSelect_t function ) {
    OnProviderSelectD2 = function;
  }

  void SetOnProviderSelectXHandler( OnProviderSelect_t function ) {
    OnProviderSelectX = function;
  }

protected:
private:

  enum { ID_NULL=wxID_HIGHEST, SYMBOL_PANELPROVIDERCONTROL_IDNAME,
    wxID_BitmapIQfeed, ID_BtnIQFeed, wxID_LblIQFeed,
    wxID_BitmapInteractiveBrokers,ID_BtnInteractiveBrokers, wxID_LblInteractiveBrokers,
    ID_RBTN_D1_IQF, ID_RBTN_D2_IQF, ID_RBTN_X_IQF, 
    ID_RBTN_D1_IB, ID_RBTN_D2_IB, ID_RBTN_X_IB, 
    ID_RBTN_D1_SIM, ID_RBTN_D2_SIM, ID_RBTN_X_SIM,
    wxID_BitmapSimulation, ID_BtnSimulation, wxID_LblSimulation
  };

  eProviderState_t m_stateIQFeed;
  eProviderState_t m_stateIB;
  eProviderState_t m_stateSimulator;

  bool m_bDisabling;

//  int m_nProvidersOn;  // # number of providers not off

    wxRadioButton* m_btnD1IQFeed;
    wxRadioButton* m_btnD2IQFeed;
    wxRadioButton* m_btnXIQFeed;
    wxButton* m_btnIQFeed;
    wxRadioButton* m_btnD1IB;
    wxRadioButton* m_btnD2IB;
    wxRadioButton* m_btnXIB;
    wxButton* m_btnIB;
    wxRadioButton* m_btnD1Simulator;
    wxRadioButton* m_btnD2Simulator;
    wxRadioButton* m_btnXSimulator;
    wxButton* m_btnSimulator;

  OnProviderStateChange_t OnIQFeedStateChange;
  OnProviderStateChange_t OnIBStateChange;
  OnProviderStateChange_t OnSimulatorStateChange;

  OnProviderSelect_t OnProviderSelectD1;
  OnProviderSelect_t OnProviderSelectD2;
  OnProviderSelect_t OnProviderSelectX;

  void SetState( wxButton* btn, eProviderState_t state );

  void Init( void );
  void CreateControls( void );
  bool ShowToolTips( void ) { return true; };

  void ResetAllRadioData1( void );
  void ResetAllRadioData2( void );
  void ResetAllRadioExec( void );

  void EnableAllRadio( void );
  void DisableAllRadio( void );

  void OnClose( wxCloseEvent& event );

  void OnBtnCommon( wxCommandEvent& event, eProviderState_t& state );

  void OnBtnIQFeed( wxCommandEvent& event );
  void OnBtnIB( wxCommandEvent& event );
  void OnBtnSimulation( wxCommandEvent& event );

  void OnIQFeedState( UpdateProviderStatusEvent& event );
  void OnIBState( UpdateProviderStatusEvent& event );
  void OnSimulatorState( UpdateProviderStatusEvent& event );
  
  void OnRadioD1IQFeed( wxCommandEvent& event );
  void OnRadioD2IQFeed( wxCommandEvent& event );
  void OnRadioXIQFeed( wxCommandEvent& event );
  void OnRadioD1IB( wxCommandEvent& event );
  void OnRadioD2IB( wxCommandEvent& event );
  void OnRadioXIB( wxCommandEvent& event );
  void OnRadioD1Sim( wxCommandEvent& event );
  void OnRadioD2Sim( wxCommandEvent& event );
  void OnRadioXSim( wxCommandEvent& event );
};

} // namespace tf
} // namespace ou
