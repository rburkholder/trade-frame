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

#include <stdexcept>
#include <cassert>

#include "ISqlite3.h"


namespace ou {
namespace db {

ISqlite3::ISqlite3(void) 
  : m_db( 0 )
{
}

ISqlite3::~ISqlite3(void) {
}

void ISqlite3::Open( const std::string& sDbFileName, enumOpenFlags flags ) {

  int sqlite3_flags = SQLITE_OPEN_READWRITE;
  sqlite3_flags |= ( 0 < ( flags & EOpenFlagsAutoCreate ) ) ? SQLITE_OPEN_CREATE : 0;

  int rtn = sqlite3_open_v2( sDbFileName.c_str(), &m_db, sqlite3_flags, 0 );
  if ( SQLITE_OK != rtn ) {
    m_db = 0;
    throw std::runtime_error( "Db open error" );
  }

  m_sDbFileName = sDbFileName;
  m_bDbOpened = true;

}

void ISqlite3::Close( void ) {

  if ( m_bDbOpened ) {
    int rtn = sqlite3_close( m_db );
    m_db = 0;
    m_bDbOpened = false;
    assert( SQLITE_OK == rtn );
  }

}


} // db
} // ou
