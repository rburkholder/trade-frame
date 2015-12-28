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

//#include <wx/wx.h>
//#include <wx/validate.h>
#include <wx/valtext.h>
#include <wx/string.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class SetCursorEvent: public wxEvent {
public:
  SetCursorEvent( wxEventType eventType, long lInsertionPoint )
    : wxEvent( 0, eventType ), m_lInsertionPoint( lInsertionPoint ) {}
  SetCursorEvent( const SetCursorEvent& event ): wxEvent( event ), m_lInsertionPoint( event.m_lInsertionPoint ) {}
  ~SetCursorEvent( void ) {}
  SetCursorEvent* Clone( void ) const { return new SetCursorEvent( *this ); }
  long GetInsertionPoint( void ) const { return m_lInsertionPoint; }
protected:
private:
  long m_lInsertionPoint;
};

wxDECLARE_EVENT( EVT_SetCursorEvent, SetCursorEvent );

class InstrumentNameValidator: public wxTextValidator {
public:
  enum EValidationType{ eAlpha=1, eNumeric=2, eCaps=4, eAt=8 }; 
  InstrumentNameValidator( const InstrumentNameValidator& validator )
    : wxTextValidator( wxFILTER_NONE ), 
      m_vt( validator.m_vt ), m_pString( validator.m_pString ), m_bInProcess( false ) { Init(); };
  InstrumentNameValidator( wxString* p = 0, unsigned long vt = eAlpha | eCaps| eNumeric | eAt )
    : wxTextValidator( wxFILTER_NONE ), 
      m_vt( vt ), m_pString( p ), m_bInProcess( false ) { Init(); };
  InstrumentNameValidator* Clone( void ) const {
    return new InstrumentNameValidator( *this );
  }
  virtual bool TransferFromWindow( void );
  virtual bool TransferToWindow( void );
  virtual bool Validate( wxWindow* parent );
protected:
  void OnChar( wxKeyEvent& event );
  void OnIdle( wxIdleEvent& event );
private:
  unsigned long m_vt;
  wxString* m_pString;
  bool m_bInProcess;  // prevents re-entry via WriteText (todo: convert to event, or not, due to the recursion?)
  void Init( void );
  void OnSetCursor( SetCursorEvent& event );
  
};

} // namespace tf
} // namespace ou

