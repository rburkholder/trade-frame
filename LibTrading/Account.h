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

#pragma once

#include <LibBerkeleyDb/Record.h>

#define OU_TABLE_ACCOUNT_RECORD_FIELDS \
  ((OU_TABLE_ACCOUNT_ACCOUNTID,      ProcessFieldSk, sAccountId        )) \
  ((OU_TABLE_ACCOUNT_ACCOUNTNAME,    ProcessFieldSk, sAccountName      )) \
  ((OU_TABLE_ACCOUNT_VERSION,        ProcessFieldPod<unsigned long>, nVersion         )) \
  ((OU_TABLE_ACCOUNT_ACCOUNTADVISOR, ProcessFieldFk<CAutoIncKeys::keyValue_t>, fkAccountAdvisor  )) \
  /**/

class CAccount
{
public:

  OU_DB_DECLARE_STRUCTURES(Account, OU_TABLE_ACCOUNT_RECORD_FIELDS)

  CAccount(void);
  ~CAccount(void);

protected:

private:
};

