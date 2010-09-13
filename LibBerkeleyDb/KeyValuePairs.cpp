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

#include <cassert>

#include "KeyValuePairs.h"

// 
// CKeyValuePairsBase
//

CKeyValuePairsBase::CKeyValuePairsBase( const std::string& sDbName ) {
  CBerkeleyDBEnvManager& mgrEnv = CBerkeleyDBEnvManager::Instance();
  m_pdb = new Db( mgrEnv.GetDbEnv(), 0 );
  assert( NULL != m_pdb );
  m_pdb->open( NULL, mgrEnv.GetBDBFileName(), sDbName.c_str(), DB_BTREE, DB_CREATE, 0 );
}


CKeyValuePairsBase::~CKeyValuePairsBase(void) {
  m_pdb->close(0);
}

void CKeyValuePairsBase::Set( Dbt *pKey, Dbt *pValue ) {
  int ret;
  try {
    ret = m_pdb->put( 0, pKey, pValue, 0 ); // overwrite existing value, or create new one
  }
  catch ( DbException e ) {
    std::string err( "CKeyValuePairsBase::Save: DbException error, " );
    err.append( e.what() );
    throw std::domain_error( err );
  }
  if ( 0 != ret ) 
    throw std::runtime_error( "CKeyValuePairsBase::Save put had error" );
}

void CKeyValuePairsBase::Get( Dbt *pKey, Dbt *pValue ) {
  int ret;
  try {
    ret = m_pdb->get( 0, pKey, pValue, 0 );
  }
  catch ( DbException e ) {
    std::string err( "CKeyValuePairsBase::Get: DbException error, " );
    err.append( e.what() );
    throw std::domain_error( err );
  }
  if ( DB_NOTFOUND == ret ) {
    throw std::out_of_range( "CKeyValuePairsBase::Get key not found" );
  }
  if ( 0 != ret ) 
    throw std::runtime_error( "CKeyValuePairsBase::Get get had error" );
}

void CKeyValuePairsBase::Truncate( void ) {
  u_int32_t countp = 0;
  m_pdb->truncate( NULL, &countp, 0 );
}

