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
#include <wx/validate.h>
#include <wx/string.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class InstrumentNameValidator: public wxValidator {
public:
  enum EValidationType{ eCapsOnly, eCapsAlphaNum };
  InstrumentNameValidator( const InstrumentNameValidator& validator )
    : wxValidator(), m_vt( validator.m_vt ), m_pString( validator.m_pString ) { Init(); };
  InstrumentNameValidator( wxString* p = 0, EValidationType vt = eCapsOnly ): wxValidator(), m_vt( vt ), m_pString( p ) { Init(); };
  InstrumentNameValidator* Clone( void ) const {
    return new InstrumentNameValidator( *this );
  }
  bool TransferFromWindow( void );
  bool TransferToWindow( void );
  bool Validate( wxWindow* parent );
protected:
  void OnChar( wxKeyEvent& event ) {
//      event.m_keyCode = event.m_uniChar = event.m_rawCode = ch - 'a' + 'A';
    long ch = event.GetKeyCode();
    //      if ( m_validatorWindow->IsKindOf(CLASSINFO(wxTextEntry))) {
    wxTextEntry& winText( dynamic_cast<wxTextEntry&>( *m_validatorWindow ) );
    if ( ( 'a' <= ch ) && ( 'z' >= ch ) ) {
      ch += 'A' - 'a';  // always capitalize
      winText.WriteText( wxChar( ch ) );
    }
    else {
      if ( eCapsOnly == m_vt ) {
        event.Skip();  // allow everything 
      }
      else {
        if ( ( 0 < winText.GetValue().size() ) && ( '0' <= ch ) && ( '9' >= ch ) ) winText.WriteText( wxChar( ch ) );
        if ( ( 8 /* bs */ == ch ) || ( 127 /* del */ == ch ) || ( 128 <= ch ) ) event.Skip();
        // ignore all other characters
      }
    }
  }
private:
  EValidationType m_vt;
  wxString* m_pString;
  void Init( void ) {
    Bind( wxEVT_CHAR, &InstrumentNameValidator::OnChar, this );
  }
};

} // namespace tf
} // namespace ou
