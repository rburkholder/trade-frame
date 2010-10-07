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

#include "AccountOwner.h"

CAccountOwner::CAccountOwner(void) {
}


CAccountOwner::~CAccountOwner(void) {
}

void CAccountOwner::CreateDbTable( sqlite3* pDb ) {

  char* pMsg;
  int rtn;

  rtn = sqlite3_exec( pDb,
    "create table if not exists accountowner ( \
    accountownderid TEXT CONSTRAINT pk_accountowner PRIMARY KEY, \
    version SMALLINT DEFAULT 1, \
    accountadvisorid TEXT NOT NULL, \
    name TEXT NOT NULL, \
    CONSTRAINT fk_accountowner_accountadvisorid \
      FOREIGN KEY(accountadvisorid) REFERENCES accountadvisor(accountadvisorid) \
        ON DELETE RESTRICT ON UPDATE CASCADE \
    );",
    0, 0, &pMsg );

  if ( SQLITE_OK != rtn ) {
    std::string sErr( "Error creating table accountowner: " );
    sErr += pMsg;
    sqlite3_free( pMsg );
    throw std::runtime_error( sErr );
  }
}

