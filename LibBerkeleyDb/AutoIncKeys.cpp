/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#ifdef WIN32
#include "StdAfx.h"
#endif

#include "AutoIncKeys.h"

const std::string CAutoIncKeys::m_sDbName( "_AutoIncKeys" );

//
// CAutoIncKeys
//

CAutoIncKeys::CAutoIncKeys(void)
: CKeyValuePairs<unsigned long>( m_sDbName )
{
}

CAutoIncKeys::~CAutoIncKeys(void) {
}

void CAutoIncKeys::SetNextId( const std::string& sKeyName, keyValue_t value ) {
  Set( sKeyName, value );
}

CAutoIncKeys::keyValue_t CAutoIncKeys::GetNextId( const std::string& sKeyName ) {
  keyValue_t nId;
  try {
    Get( sKeyName, &nId );
    ++nId;
    Set( sKeyName, nId );
  }
  catch ( std::out_of_range e ) {
    nId = 1;
    Set( sKeyName, nId );
  }
  return nId;
}

CAutoIncKeys::keyValue_t CAutoIncKeys::GetCurrentId( const std::string& sKeyName ) {
  keyValue_t nId;
  try {
    Get( sKeyName, &nId );
  }
  catch ( std::out_of_range e ) {
    nId = 1;
    Set( sKeyName, nId );
  }
  return nId;
}

//
// CAutoIncKey
//

CAutoIncKey::CAutoIncKey(const std::string& sKeyName)
: m_sKeyName( sKeyName )
{
}

CAutoIncKey::~CAutoIncKey(void) {
}

void CAutoIncKey::SetNextId( keyValue_t value ) {
  Set( m_sKeyName, value );
}

CAutoIncKey::keyValue_t CAutoIncKey::GetNextId( void ) {
  return CAutoIncKeys::GetNextId( m_sKeyName );
}

CAutoIncKey::keyValue_t CAutoIncKey::GetCurrentId( void ) {
  return CAutoIncKeys::GetCurrentId( m_sKeyName );
}
