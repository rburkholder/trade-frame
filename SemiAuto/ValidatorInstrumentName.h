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

class InstrumentNameValidator: public wxValidator {
public:
  InstrumentNameValidator( const InstrumentNameValidator& validator ): wxValidator() { Init(); };
  InstrumentNameValidator( wxString* p = 0 ): wxValidator(), m_pString( p ) { Init(); };
  InstrumentNameValidator* Clone( void ) const {
    return new InstrumentNameValidator( *this );
  }
  bool TransferFromWindow( void );
  bool TransferToWindow( void );
  bool Validate( wxWindow* parent );
protected:
  void OnChar( wxKeyEvent& event ) {
    long ch = event.GetKeyCode();
    if ( ( 'a' <= ch ) && ( 'z' >= ch ) ) {
//      event.m_keyCode = event.m_uniChar = event.m_rawCode = ch - 'a' + 'A';
      ch += 'A' - 'a';
      if ( m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl))) {
        wxTextCtrl& winText( dynamic_cast<wxTextCtrl&>( *m_validatorWindow ) );
        winText.WriteText( wxChar( ch ) );
      }
    }
    else event.Skip();
  }
private:
  wxString* m_pString;
  void Init( void ) {
    Bind( wxEVT_CHAR, &InstrumentNameValidator::OnChar, this );
  }
};

