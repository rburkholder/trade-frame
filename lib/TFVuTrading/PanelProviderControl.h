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

// D1: data provider 1, could be iqfeed, regular data
// D2: data provider 2, could be ib, typically option info
// X: execution provider, typically IB or Sim

class wxButton;
class wxCheckBox;

namespace ou { // One Unified
namespace tf { // TradeFrame

enum eProviderState_t { ProviderOff, ProviderGoingOn, ProviderOn, ProviderGoingOff };

class UpdateProviderStatusEvent: public wxEvent {
public:

  UpdateProviderStatusEvent( wxEventType eventType, eProviderState_t state )
  : wxEvent( 0, eventType ), m_state( state ) {
  }

  UpdateProviderStatusEvent( const UpdateProviderStatusEvent& event )
  : wxEvent( event ), m_state( event.m_state ) {};

  ~UpdateProviderStatusEvent() {};

  UpdateProviderStatusEvent* Clone() const { return new UpdateProviderStatusEvent( *this ); }
  eProviderState_t GetState() const { return m_state; };

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

class PanelProviderControl: public wxPanel {
public:

  enum Provider_t {
    EIQFeed, EIB, ESim
  };

  PanelProviderControl();
  PanelProviderControl(
    wxWindow* parent,
    wxWindowID id = SYMBOL_PANELPROVIDERCONTROL_IDNAME,
    const wxPoint& pos = SYMBOL_PANELPROVIDERCONTROL_POSITION,
    const wxSize& size = SYMBOL_PANELPROVIDERCONTROL_SIZE,
    long style = SYMBOL_PANELPROVIDERCONTROL_STYLE );
  virtual ~PanelProviderControl();

  bool Create(
    wxWindow* parent,
    wxWindowID id = SYMBOL_PANELPROVIDERCONTROL_IDNAME,
    const wxPoint& pos = SYMBOL_PANELPROVIDERCONTROL_POSITION,
    const wxSize& size = SYMBOL_PANELPROVIDERCONTROL_SIZE,
    long style = SYMBOL_PANELPROVIDERCONTROL_STYLE );

  void SetProvider( Provider_t d1, Provider_t d2, Provider_t x );

  void SetIQFeedState( eProviderState_t state );
  void SetIBState( eProviderState_t state );
  void SetSimulatorState( eProviderState_t state );

  void QueueEvent( wxEvent* event ) { wxEvtHandler::QueueEvent( event ); };

  using OnProviderStateChange_t = FastDelegate1<eProviderState_t>;

  void SetOnIBStateChangeHandler( OnProviderStateChange_t function ) {
    OnIBStateChange = function;
  }
  void SetOnIQFeedStateChangeHandler( OnProviderStateChange_t function ) {
    OnIQFeedStateChange = function;
  }
  void SetOnSimulatorStateChangeHandler( OnProviderStateChange_t function ) {
    OnSimulatorStateChange = function;
  }

  using OnProviderSelect_t = FastDelegate1<Provider_t>;

  void SetOnProviderSelectD1Handler( OnProviderSelect_t function ) {
    OnProviderSelectD1 = function;
  }

  void SetOnProviderSelectD2Handler( OnProviderSelect_t function ) {
    OnProviderSelectD2 = function;
  }

  void SetOnProviderSelectXHandler( OnProviderSelect_t function ) {
    OnProviderSelectX = function;
  }

  void SyncInitialState();

protected:
private:

  enum {
    ID_NULL=wxID_HIGHEST, ID_PANELPROVIDERCONTROL,
    wxID_BitmapIQfeed, ID_BtnIQFeed, wxID_LblIQFeed,
    wxID_BitmapInteractiveBrokers,ID_BtnInteractiveBrokers, wxID_LblInteractiveBrokers,
    ID_CB_IQF_D1, ID_CB_IQF_D2, ID_CB_IQF_X,
    ID_CB_IB_D1, ID_CB_IB_D2, ID_CB_IB_X,
    ID_CB_SIM_D1, ID_CB_SIM_D2, ID_CB_SIM_X,
    wxID_BitmapSimulation, ID_BtnSimulation, wxID_LblSimulation
  };

  eProviderState_t m_stateIQFeed;
  eProviderState_t m_stateIB;
  eProviderState_t m_stateSimulator;

//  int m_nProvidersOn;  // # number of providers not off

    wxCheckBox* m_cbIQFeedD1;
    wxCheckBox* m_cbIQFeedD2;
    wxCheckBox* m_cbIQFeedX;
    wxButton* m_btnIQFeed;
    wxCheckBox* m_cbIBD1;
    wxCheckBox* m_cbIBD2;
    wxCheckBox* m_cbIBX;
    wxButton* m_btnIB;
    wxCheckBox* m_cbSimD1;
    wxCheckBox* m_cbSimD2;
    wxCheckBox* m_cbSimX;
    wxButton* m_btnSimulator;

  OnProviderStateChange_t OnIQFeedStateChange;
  OnProviderStateChange_t OnIBStateChange;
  OnProviderStateChange_t OnSimulatorStateChange;

  OnProviderSelect_t OnProviderSelectD1;
  OnProviderSelect_t OnProviderSelectD2;
  OnProviderSelect_t OnProviderSelectX;

  void SetState( wxButton* btn, eProviderState_t state );

  void Init();
  void CreateControls();
  bool ShowToolTips() { return true; };

  void ResetAllRadioData1();
  void ResetAllRadioData2();
  void ResetAllRadioExec();

  void EnableAllRadio();
  void DisableAllRadio();

  void UpdateProviderButtons();

  void OnClose( wxCloseEvent& event );

  void OnBtnCommon( wxCommandEvent& event, eProviderState_t& state );

  void OnBtnIQFeed( wxCommandEvent& event );
  void OnBtnIB( wxCommandEvent& event );
  void OnBtnSimulation( wxCommandEvent& event );

  void OnIQFeedState( UpdateProviderStatusEvent& event );
  void OnIBState( UpdateProviderStatusEvent& event );
  void OnSimulatorState( UpdateProviderStatusEvent& event );

  void OnBtnD1IQFeed( wxCommandEvent& event );
  void OnBtnD2IQFeed( wxCommandEvent& event );
  void OnBtnXIQFeed( wxCommandEvent& event );
  void OnBtnD1IB( wxCommandEvent& event );
  void OnBtnD2IB( wxCommandEvent& event );
  void OnBtnXIB( wxCommandEvent& event );
  void OnBtnD1Sim( wxCommandEvent& event );
  void OnBtnD2Sim( wxCommandEvent& event );
  void OnBtnXSim( wxCommandEvent& event );
};

} // namespace tf
} // namespace ou
