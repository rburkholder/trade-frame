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

#include "StdAfx.h"

#include "ValidatorInstrumentName.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

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
