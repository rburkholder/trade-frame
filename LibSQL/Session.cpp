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

#include "Session.h"

#include <stdexcept>
#include <cassert>

namespace ou {
namespace db {

//void CDbSession::PrepareStatement(
//  const std::string& sErrPrefix, const std::string& sSqlOp, sqlite3_stmt** pStmt ) {

/*
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
*/

//
//

//
//

} // db
} // ou
