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

#include <string>
#include <stdexcept>

#include "AccountAdvisor.h"

CAccountAdvisor::CAccountAdvisor( const std::string& sAdvisorId, const std::string& sAdvisorName ) 
: m_sAdvisorId( sAdvisorId ), m_sAdvisorName( sAdvisorName )
{
}

CAccountAdvisor::~CAccountAdvisor(void) {
}

void CAccountAdvisor::CreateDbTable( sqlite3* pDb ) {

  char* pMsg;
  int rtn;

  rtn = sqlite3_exec( pDb,
    "create table if not exists accountadvisors ( \
    accountadvisorid TEXT CONSTRAINT pk_accountadvisors PRIMARY KEY, \
    version SMALLINT DEFAULT 1, \
    name TEXT NOT NULL \
    );",
    0, 0, &pMsg );

  if ( SQLITE_OK != rtn ) {
    std::string sErr( "Error creating table accountadvisors: " );
    sErr += pMsg;
    sqlite3_free( pMsg );
    throw std::runtime_error( sErr );
  }
}
