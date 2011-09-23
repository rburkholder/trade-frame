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

#include <wx/wx.h>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

// wxFrame (wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE, const wxString &name=wxFrameNameStr)

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

class FrameProviderControl: public wxFrame {
public:

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

  FrameProviderControl( void );
  FrameProviderControl(
    wxWindow* parent,
   const wxString& title = "Provider Control", 
   const wxPoint& pos=wxDefaultPosition, 
   const wxSize& size=wxDefaultSize, //wxSize(500, 120), 
   long style=wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));
  ~FrameProviderControl(void);

  bool Create( 
    wxWindow* parent,
   const wxString& title = "Provider Control", 
   const wxPoint& pos=wxDefaultPosition, 
   const wxSize& size=wxDefaultSize, //wxSize(500, 120), 
   long style=wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));

  void SetIQFeedState( eProviderState_t state );
  void SetIBState( eProviderState_t state );
  void SetSimulatorState( eProviderState_t state );

  void QueueEvent( wxEvent* event ) { wxEvtHandler::QueueEvent( event ); };

protected:
private:

  enum { ID_NULL=wxID_HIGHEST, 
    wxID_BitmapIQfeed, ID_BtnIQFeed, wxID_LblIQFeed,
    wxID_BitmapInteractiveBrokers,ID_BtnInteractiveBrokers, wxID_LblInteractiveBrokers,
    wxID_BitmapSimulation, ID_BtnSimulation, wxID_LblSimulation
  };

  eProviderState_t m_stateIQFeed;
  eProviderState_t m_stateIB;
  eProviderState_t m_stateSimulator;

  wxButton* m_btnIQFeed;
  wxButton* m_btnIB;
  wxButton* m_btnSimulator;

  OnProviderStateChange_t OnIQFeedStateChange;
  OnProviderStateChange_t OnIBStateChange;
  OnProviderStateChange_t OnSimulatorStateChange;

  void SetState( wxButton* btn, eProviderState_t state );

  void Init( void );
  void CreateControls( void );
  bool ShowToolTips( void ) { return true; };

  void OnClose( wxCloseEvent& event );

  void OnBtnCommon( wxCommandEvent& event, eProviderState_t& state );

  void OnBtnIQFeed( wxCommandEvent& event );
  void OnBtnIB( wxCommandEvent& event );
  void OnBtnSimulation( wxCommandEvent& event );

  void OnIQFeedState( UpdateProviderStatusEvent& event );
  void OnIBState( UpdateProviderStatusEvent& event );
  void OnSimulatorState( UpdateProviderStatusEvent& event );


};

