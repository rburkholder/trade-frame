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

#include "Account.h"

CAccount::CAccount(void)
{
}

CAccount::~CAccount(void) {
}

void CAccount::CreateDbTable( sqlite3* pDb ) {

  m_pDb = pDb;

  char* pMsg;

  int rtn = sqlite3_exec( pDb,
    "create table if not exists account ( \
    accountid TEXT CONSTRAINT pk_account PRIMARY KEY, \
    version SMALLINT DEFAULT 1, \
    accountname TEXT NOT NULL, \
    accountadvisorid TEXT, \
    CONSTRAINT fk_accountadvisor_advisorid \
      FOREIGN KEY(accountadvisorid) REFERENCES accountadvisor(advisorid) \
        ON DELETE RESTRICT ON UPDATE CASCADE \
       \
    );",
    NULL, NULL, &pMsg );

  if ( SQLITE_OK == rtn ) {
  }
  else {
//    std::cerr << pMsg << std::endl;
    sqlite3_free( pMsg );
  }
}

