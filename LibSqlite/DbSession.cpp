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

#include "stdafx.h"

#include "DbSession.h"

#include <stdexcept>
#include <cassert>

void CDbSession::PrepareStatement( 
  const std::string& sErrPrefix, const std::string& sSqlOp, sqlite3_stmt** pStmt ) {

  assert( m_bDbOpened );

  int rtn;

  if ( NULL == *pStmt ) {
    rtn = sqlite3_prepare_v2( m_db, sSqlOp.c_str(), -1, pStmt, NULL );
    if ( SQLITE_OK != rtn ) {
      std::string sErr( sErrPrefix );
      sErr += ":  error in prepare";
      throw std::runtime_error( sErr );
    }
  }
  else {
    rtn = sqlite3_reset( *pStmt );
    if ( SQLITE_OK != rtn ) {
      std::string sErr( sErrPrefix );
      sErr += ":  error in reset";
      throw std::runtime_error( sErr );
    }
  }

}


CDbSession::CDbSession(void)
: m_db( 0 ), m_bDbOpened( false )
{
}


CDbSession::CDbSession( const char* szDbFileName )
: m_db( 0 ), m_bDbOpened( false )
{
  Open( szDbFileName );
}


CDbSession::~CDbSession(void)
{
  Close();
}

void CDbSession::Open( const char* szDbFileName ) {
  int rtn = sqlite3_open_v2( szDbFileName, &m_db, SQLITE_OPEN_READWRITE, 0 );
  if ( SQLITE_OK != rtn ) {
    m_db = 0;
    throw std::runtime_error( "Db open error" );
  }

  m_sDbFileName = szDbFileName;
  m_bDbOpened = true;
}

void CDbSession::Close( void ) {
  if ( m_bDbOpened ) {
    int rtn = sqlite3_close( m_db );
    m_db = 0;
    m_bDbOpened = false;
    assert( SQLITE_OK == rtn );
  }
}

