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

#include "Account.h"

CAccount::CAccount(void)
{
}

CAccount::~CAccount(void) {
}

void CAccount::CreateDbTable( sqlite3* pDb ) {

  char* pMsg;
  int rtn;

  rtn = sqlite3_exec( pDb,
    "create table if not exists accounts ( \
    accountid TEXT CONSTRAINT pk_accounts PRIMARY KEY, \
    version SMALLINT DEFAULT 1, \
    name TEXT NOT NULL, \
    accountownerid TEXT NOT NULL, \
    brokername TEXT, \
    brokerageaccountid TEXT, \
    CONSTRAINT fk_accounts_accountownerid \
      FOREIGN KEY(accountownerid) REFERENCES accountowners(accountownderid) \
        ON DELETE RESTRICT ON UPDATE CASCADE \
       \
    );",
    0, 0, &pMsg );

  if ( SQLITE_OK != rtn ) {
    std::string sErr( "Error creating table accounts: " );
    sErr += pMsg;
    sqlite3_free( pMsg );
    throw std::runtime_error( sErr );
  }
}

