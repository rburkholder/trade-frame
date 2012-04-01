/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include "sqlite3.h"

namespace ou {
namespace db {
namespace sqlite {

struct structStatementState {
  sqlite3_stmt* pStmt;
  bool bIsReset;  // ensure it is reset just prior to execute
  structStatementState( void ) : pStmt( 0 ), bIsReset( true ) {};
};

} // namespace sqlite
} // db
} // ou
