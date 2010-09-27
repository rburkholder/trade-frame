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

const std::string CAccountManager::m_sAccountAdvisorDbName( "AccountAdvisor" );
const std::string CAccountManager::m_sAccountDbName( "Account" );

CAccountManager::CAccountManager( const std::string& sDbFileName ) 
: ManagerBase<CAccountManager, std::string, CAccountAdvisor>(),
  m_sDbFileName( sDbFileName ),
  m_tblAccountAdvisor( sDbFileName, m_sAccountAdvisorDbName ),
  m_tblAccount( sDbFileName, m_sAccountDbName )
{
  
}

CAccountManager::~CAccountManager(void) {
}

void CAccountManager::AddAccountAdvisor( const std::string& sAccountAdvisorId, const std::string& sAccountAdvisorName ) {
  CAccountAdvisor aa( sAccountAdvisorId, sAccountAdvisorName );
  if ( !m_tblAccountAdvisor.Exists( aa.m_tplAccountAdvisor ) ) {
    m_tblAccountAdvisor.Insert( aa.m_tplAccountAdvisor, aa.m_recAccountAdvisor );
  }
  else {
  }
}