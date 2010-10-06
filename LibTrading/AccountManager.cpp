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

#include "StdAfx.h"

#include "AccountManager.h"

CAccountManager::CAccountManager( sqlite3* pDb ) 
: ManagerBase<CAccountManager, std::string, CAccountAdvisor>(),
  m_pDb( pDb )
{
  
}

CAccountManager::~CAccountManager(void) {
}

void CAccountManager::AddAccountAdvisor( const std::string& sAccountAdvisorId, const std::string& sAccountAdvisorName ) {
  CAccountAdvisor aa( sAccountAdvisorId, sAccountAdvisorName );
}

void CAccountManager::CreateDbTables( void ) {

  CAccountAdvisor aa;
  aa.CreateDbTable( m_pDb );

  CAccount a;
  a.CreateDbTable( m_pDb );

}