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

#include <cassert>

#include "AccountManager.h"

CAccountManager::CAccountManager( void ) 
: ManagerBase<CAccountManager, std::string, CAccountAdvisor>(),
  m_bCanPersist( false )
{
  
}

CAccountManager::CAccountManager( const std::string& sDbFileName ) 
: ManagerBase<CAccountManager, std::string, CAccountAdvisor>(),
  m_sDbFileName( sDbFileName ),
  m_bCanPersist( true )
{
  
}

CAccountManager::~CAccountManager(void) {
}

void CAccountManager::SetDbFileName( const std::string& sDbFileName ) {
  assert( !m_bCanPersist );
  m_sDbFileName = sDbFileName;
  m_bCanPersist = true;
}

void CAccountManager::AddAccountAdvisor( const std::string& sAccountAdvisorId, const std::string& sAccountAdvisorName, bool bPersist ) {
}