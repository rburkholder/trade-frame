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

#include "stdafx.h"

#include <wx/textentry.h>
////#include <wx/event.h>

#include <iostream>

#include "ValidatorInstrumentName.h"

// if something more complicated, then look at:
//  http://trac.wxwidgets.org/ticket/14535
//  http://trac.wxwidgets.org/attachment/ticket/14535/wxMaskedEdit_v1r2.patch

namespace ou { // One Unified
namespace tf { // TradeFrame

wxDEFINE_EVENT( EVT_SetCursorEvent, SetCursorEvent );

void InstrumentNameValidator::Init( void ) {
  //m_bSetInsertionPoint = false;
  //m_lInsertionPoint = 0;
  Bind( wxEVT_CHAR, &InstrumentNameValidator::OnChar, this );
  //Bind( wxEVT_IDLE, &InstrumentNameValidator::OnIdle, this );
  Bind( EVT_SetCursorEvent, &InstrumentNameValidator::OnSetCursor, this );
}

void InstrumentNameValidator::OnChar( wxKeyEvent& event ) {
  
  if ( m_bInProcess ) {
    event.Skip();  // need to allow the return and continuation
    return;
  }
  
  wxASSERT( m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl) ) );
  
  m_bInProcess = true;

  // 2015/12/23 it doesn't appear as though an event.skip() processors will accept the changed codes
//      event.m_keyCode = event.m_uniChar = event.m_rawCode = ch - 'a' + 'A';
  // therefore, we have to do the editing ourselves
  long kc( 0 );
#if wxUSE_UNICODE
  kc = (long) event.GetUnicodeKey();
#else // !wxUSE_UNICODE
  kc = event.GetKeyCode();
#endif // wxUSE_UNICODE/!wxUSE_UNICODE  

  event.Skip( false );  // default ignore characters unless allowed
  bool bAllowChar( false ); // whether to display by caller
  bool bWriteText( false ); // whether to display by WriteText
  wxTextEntry* winText( dynamic_cast<wxTextEntry*>( GetWindow() ) );
  
  if ( ( 32 > kc ) || ( 127 < kc ) ) { // allow stuff outside of ascii range
    bAllowChar = true;
  }
  else {
    // characters are ascii and process as such
    if ( 0 != ( eAlpha & m_vt ) ) { // allow alpha
      if ( ( _T('A') <= kc ) && ( _T('Z') >= kc ) ) {
        bAllowChar = true;
      }
      if ( ( _T('a') <= kc ) && ( _T('z') >= kc ) ) {
        if ( 0 != ( eCaps & m_vt ) ) {
          kc += 'A' - 'a';  // always capitalize
          bWriteText = true;
        }
        else {
          bAllowChar = true;
        }
      }
    }
    if ( ( 0 != ( eNumeric & m_vt ) ) && ( _T('0') <= kc ) && ( _T('9') >= kc ) ) {
      if ( 0 < winText->GetValue().size() ) {
        bAllowChar = true;
      }
    }
    if ( 0 != ( eAt & m_vt ) ) {
      if ( _T('@') == kc ) {  // for iqfeed symbols, may do a test for position 0
        bAllowChar = true;
      }
    }
  }
  
  assert ( ! ( bAllowChar && bWriteText ) );
  if ( bWriteText ) {
    int ip = winText->GetInsertionPoint();
    winText->WriteText( wxChar( kc ) );
    QueueEvent( new SetCursorEvent( EVT_SetCursorEvent, ip + 1 ) );
    event.Skip(false);  // end of the line for processing
  }
  if ( bAllowChar ) {
    event.Skip(true);   // allow the character
  }
  
  m_bInProcess = false;
}

// https://groups.google.com/forum/#!msg/wx-users/uTshhOwLfwY/lL7FC0YvqJMJ
// SetInsertionPoint doesn't happen within wxGTK handler
void InstrumentNameValidator::OnIdle( wxIdleEvent& event ) {
//  if ( m_bSetInsertionPoint ) {
//    m_bSetInsertionPoint = false;
//    wxTextEntry* winText( dynamic_cast<wxTextEntry*>( GetWindow() ) );
//    winText->SetInsertionPoint( m_lInsertionPoint );
//  }
}

void InstrumentNameValidator::OnSetCursor( SetCursorEvent& event ) {
  wxTextEntry* winText( dynamic_cast<wxTextEntry*>( GetWindow() ) );
  winText->SetInsertionPoint( event.GetInsertionPoint() );
}

bool InstrumentNameValidator::TransferFromWindow( void ) {
//  wxWindow* pWin = GetWindow();
//  if ( pWin->IsKindOf(CLASSINFO(wxTextCtrl))) {
//  if ( m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl))) {
    wxTextEntry& winText( dynamic_cast<wxTextEntry&>( *m_validatorWindow ) );
    *m_pString = winText.GetValue();
//  }
  return true;
}

bool InstrumentNameValidator::TransferToWindow( void ) {
  wxWindow* pWin = GetWindow();
  wxTextEntry& winText( dynamic_cast<wxTextEntry&>( *pWin ) );
  winText.SetValue( *m_pString );
  return true;
}

bool InstrumentNameValidator::Validate( wxWindow* parent ) {
  wxTextEntry& winText( dynamic_cast<wxTextEntry&>( *m_validatorWindow ) );
  return ( 0 != winText.GetValue().size() );
}

} // namespace tf
} // namespace ou
