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

class InstrumentNameValidator: public wxTextValidator {
public:
  enum EValidationType{ eCapsOnly, eCapsAlphaNum };
  InstrumentNameValidator( const InstrumentNameValidator& validator )
    : wxTextValidator( wxFILTER_ALPHANUMERIC ), 
      m_vt( validator.m_vt ), m_pString( validator.m_pString ), m_bInProcess( false ) { Init(); };
  InstrumentNameValidator( wxString* p = 0, EValidationType vt = eCapsOnly )
    : wxTextValidator( wxFILTER_ALPHANUMERIC ), 
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
  EValidationType m_vt;
  wxString* m_pString;
  bool m_bInProcess;  // prevents re-entry via WriteText
  bool m_bSetInsertionPoint; // need to set insertion point outside of handler (or create custom event to update stuff)
  long m_lInsertionPoint;
  void Init( void ) {
    m_bSetInsertionPoint = false;
    m_lInsertionPoint = 0;
    Bind( wxEVT_CHAR, &InstrumentNameValidator::OnChar, this );
    Bind( wxEVT_IDLE, &InstrumentNameValidator::OnIdle, this );
  }
  
};

} // namespace tf
} // namespace ou

