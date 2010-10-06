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

#include <string>

#include <LibSqlite/sqlite3.h>

class CAccount
{
public:

  CAccount(void);
  ~CAccount(void);

  void CreateDbTable( sqlite3* pDb );

protected:

private:

  std::string m_sAccountId;
  std::string m_sAccountName;
  std::string m_sAccountAdvisorId;

  sqlite3* m_pDb;

};

