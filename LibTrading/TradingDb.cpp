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

#include "StdAfx.h"
#include "TradingDb.h"

#include <iostream>
#include <stdexcept>
#include <cassert>

CTradingDb::CTradingDb( const char* szDbFileName ) {

  int rtn = sqlite3_open_v2( szDbFileName, &m_pdbTrading, SQLITE_OPEN_READWRITE, NULL );
  if ( SQLITE_OK == rtn ) {
  }
  else {
    throw std::runtime_error( "CTradingDb open error" );
  }

}

CTradingDb::~CTradingDb(void) {
  int rtn = sqlite3_close( m_pdbTrading );
  assert( SQLITE_OK == rtn );
}

void PrepareStatement( 
  const std::string& sErrPrefix, const std::string& sSqlOp, 
  sqlite3* pDb, sqlite3_stmt** pStmt ) {

  int rtn;

  if ( NULL == *pStmt ) {
    rtn = sqlite3_prepare_v2( pDb, sSqlOp.c_str(), -1, pStmt, NULL );
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


