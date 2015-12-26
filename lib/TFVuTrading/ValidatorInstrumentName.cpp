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
#include <wx/event.h>

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
  //  this is all wrong, will need to change, see wxETKBaseValidator for proper mechansim, or just use that one

  // 2015/12/23 it doesn't appear as though an event.skip() processors will accept the changed codes
//      event.m_keyCode = event.m_uniChar = event.m_rawCode = ch - 'a' + 'A';
  // therefore, we have to do the editing ourselves
  long kc( 0 );
#if wxUSE_UNICODE
  kc = (long) event.GetUnicodeKey();
#else // !wxUSE_UNICODE
  kc = event.GetKeyCode();
#endif // wxUSE_UNICODE/!wxUSE_UNICODE  
  
  //Allow default 'ctrl-C' manager
  if ( WXK_CONTROL_C == kc ) return;
  
  //Allow standard 32-127 ASCII and also 128-255 ASCII extended characters
  //if ( WXK_SPACE > kc || WXK_START <= kc ) return;

  //wxTextEntry& winText( dynamic_cast<wxTextEntry&>( *m_validatorWindow ) );
  wxTextEntry* winText( dynamic_cast<wxTextEntry*>( GetWindow() ) );
  
  event.Skip( true );
  
  if ( ( _T('a') <= kc ) && ( _T('z') >= kc ) ) {
    kc += 'A' - 'a';  // always capitalize
    int ip = winText->GetInsertionPoint();
    //wxString val = winText->GetValue();
    //val.insert( ip, wxChar( kc ) );
    //winText->SetValue( val );
    winText->WriteText( wxChar( kc ) );
    //winText->SetInsertionPoint( ip + 1 );
    //winText->SetInsertionPointEnd();
    //winText->SetInsertionPoint( winText->GetLastPosition() );
    //m_lInsertionPoint = ip + 1;
    //m_bSetInsertionPoint = true;
    QueueEvent( new SetCursorEvent( EVT_SetCursorEvent, ip + 1 ) );
    event.Skip(false);  // end of the line for processing
  }
  else {
    if ( eCapsOnly == m_vt ) {
      event.Skip();  // allow everything 
    }
    else {
      // todo:  can cursor to front of line and install numeric, so test in post processing
      if ( ( 0 < winText->GetValue().size() ) && ( '0' <= kc ) && ( '9' >= kc ) ) {
        int ip = winText->GetInsertionPoint();
        winText->WriteText( wxChar( kc ) );
        //m_lInsertionPoint = ip + 1;
        //m_bSetInsertionPoint = true;
        QueueEvent( new SetCursorEvent( EVT_SetCursorEvent, ip + 1 ) );
        event.Skip(false);  // end of the line for processing
      }
//      if ( ( 8  == kc ) || ( 127 == kc ) || ( 128 <= kc ) )  // 8=bs, 127=del
//        event.Skip();  // allow control to process the characters
      // ignore all other characters
    }
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
