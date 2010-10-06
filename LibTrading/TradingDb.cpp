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
#include <cassert>

CTradingDb::CTradingDb( const char* szDbFileName ) {

  int rtn = sqlite3_open( szDbFileName, &m_pdbTrading );
  if ( SQLITE_OK == rtn ) {
  }
  else {
    std::cerr << sqlite3_errmsg( m_pdbTrading ) << std::endl;
  }

}


CTradingDb::~CTradingDb(void) {
  int rtn = sqlite3_close( m_pdbTrading );
  assert( SQLITE_OK == rtn );
}


