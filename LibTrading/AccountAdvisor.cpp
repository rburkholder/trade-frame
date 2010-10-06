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

#include "AccountAdvisor.h"

CAccountAdvisor::CAccountAdvisor( void )
{
}

CAccountAdvisor::CAccountAdvisor( const std::string& sAdvisorId, const std::string& sAdvisorName )
{
  SetAdvisorId( sAdvisorId );
  SetAdvisorName( sAdvisorName );
}

CAccountAdvisor::~CAccountAdvisor(void) {
}

void CAccountAdvisor::CreateDbTable( sqlite3* pDb ) {

  m_pDb = pDb;

  char* pMsg;

  int rtn = sqlite3_exec( pDb,
    "create table if not exists accountadvisor ( \
    advisorid TEXT CONSTRAINT pk_accountadvisor PRIMARY KEY, \
    version SMALLINT DEFAULT 1, \
    advisorname TEXT NOT NULL \
    );",
    NULL, NULL, &pMsg );

  if ( SQLITE_OK == rtn ) {
  }
  else {
//    std::cerr << pMsg << std::endl;
    sqlite3_free( pMsg );
  }
}
