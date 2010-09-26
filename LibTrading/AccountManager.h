/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include <string>

#include <LibBerkeleyDb/Record.h>

#include "ManagerBase.h"
#include "Account.h"
#include "AccountAdvisor.h"

#define OU_DB_ACCOUNTADVISOR_RECORD_FIELDS \
  ((OU_DB_ACCOUNTADVISOR_ADVISORID, ProcessFieldSk, sAdvisorId  )) \
  ((OU_DB_ACCOUNTADVISOR_ADVISORNAME, ProcessFieldSk, sAdvisorName  )) \
  /**/

#define OU_DB_ACCOUNT_RECORD_FIELDS \
  ((OU_DB_ACCOUNT_ACCOUNTID,      ProcessFieldSk, sAccountId        )) \
  ((OU_DB_ACCOUNT_ACCOUNTNAME,    ProcessFieldSk, sAccountName      )) \
  ((OU_DB_ACCOUNT_ACCOUNTADVISOR, ProcessFieldFk<CAutoIncKeys::keyValue_t>, fkAccountAdvisor  )) \
  /**/

class CAccountManager: public ManagerBase<CAccountManager, std::string, CAccountAdvisor> {
public:
  CAccountManager(void);
  ~CAccountManager(void);
protected:
private:
  OU_DB_DECLARE_STRUCTURES(AccountAdvisor, OU_DB_ACCOUNTADVISOR_RECORD_FIELDS)
  OU_DB_DECLARE_STRUCTURES(Account, OU_DB_ACCOUNT_RECORD_FIELDS)
};
