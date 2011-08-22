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

#include "ValidatorInstrumentName.h"

bool InstrumentNameValidator::TransferFromWindow( void ) {
//  wxWindow* pWin = GetWindow();
//  if ( pWin->IsKindOf(CLASSINFO(wxTextCtrl))) {
  if ( m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl))) {
    wxTextCtrl& winText( dynamic_cast<wxTextCtrl&>( *m_validatorWindow ) );
    *m_pString = winText.GetValue();
  }
  else {
    throw std::runtime_error( "can't find wxTextCtrl" );
  }
  return true;
}

bool InstrumentNameValidator::TransferToWindow( void ) {
  wxWindow* pWin = GetWindow();
  if ( pWin->IsKindOf(CLASSINFO(wxTextCtrl))) {
    wxTextCtrl& winText( dynamic_cast<wxTextCtrl&>( *pWin ) );
    winText.SetValue( *m_pString );
  }
  else {
    throw std::runtime_error( "can't find wxTextCtrl" );
  }
  return true;
}

bool InstrumentNameValidator::Validate( wxWindow* parent ) {
  return true;
}
