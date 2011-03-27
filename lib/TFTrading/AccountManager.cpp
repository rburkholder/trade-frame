/************************************************************************
 * Copyright(c) 2009-2011, One Unified. All rights reserved.            *
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

#include <stdexcept>
#include <cassert>

#include "AccountManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

CAccountManager::CAccountManager( void ) 
: ManagerBase<CAccountManager>()//, 
  //m_session( 0 )
{
}

CAccountManager::~CAccountManager(void) {
}

CAccountAdvisor::pAccountAdvisor_t CAccountManager::GetAccountAdvisor( const idAccountAdvisor_t& sAdvisorId ) {

  pAccountAdvisor_t p;

  return p;
}

CAccountAdvisor::pAccountAdvisor_t CAccountManager::AddAccountAdvisor( const idAccountAdvisor_t& sAdvisorId, const std::string& sAdvisorName ) {

  pAccountAdvisor_t p;

  bool bExists = false;
  try {
    p = GetAccountAdvisor( sAdvisorId );
    // if we have something we can't insert something
    bool bExists = true;
  }
  catch (...) {
    // had an error, so assume no record exists, and proceed
  }

  if ( bExists ) {
    throw std::runtime_error( "CAccountManager::AddAccountAdvisor: record already exists" );
  }

  // create class, as it will do some of the processing
  p.reset( new CAccountAdvisor( sAdvisorId, sAdvisorName, "" ) );

  return p;
}

CAccountAdvisor::pAccountAdvisor_t CAccountManager::UpdateAccountAdvisor( const idAccountAdvisor_t& sAdvisorId ) {

  pAccountAdvisor_t p;

  iterAccountAdvisor_t iter = m_mapAccountAdvisor.find( sAdvisorId );
  if ( m_mapAccountAdvisor.end() == iter ) {
    throw std::runtime_error( "CAccountManager::UpdateAccountAdvisor: could not find advisor in local storage" );
  }
  p = iter->second;

  return p;
}

void CAccountManager::DeleteAccountAdvisor( const idAccountAdvisor_t& sAdvisorId ) {

  iterAccountAdvisor_t iter = m_mapAccountAdvisor.find( sAdvisorId );
  if ( m_mapAccountAdvisor.end() == iter ) {
    throw std::runtime_error( "CAccountManager::DeleteAccountAdvisor: could not find advisor in local storage" );
  }

  m_mapAccountAdvisor.erase( iter );

}

void CAccountManager::RegisterTablesForCreation( void ) {
  m_pDbSession->RegisterTable<CAccountAdvisor::TableCreateDef>( tablenames::sAccountAdvisor );
  m_pDbSession->RegisterTable<CAccountOwner::TableCreateDef>( tablenames::sAccountOwner );
  m_pDbSession->RegisterTable<CAccount::TableCreateDef>( tablenames::sAccount );
}

void CAccountManager::RegisterRowDefinitions( void ) {
  m_pDbSession->MapRowDefToTableName<CAccountAdvisor::TableRowDef>( tablenames::sAccountAdvisor );
  m_pDbSession->MapRowDefToTableName<CAccountOwner::TableRowDef>( tablenames::sAccountOwner );
  m_pDbSession->MapRowDefToTableName<CAccount::TableRowDef>( tablenames::sAccount );
}

void CAccountManager::PopulateTables( void ) {
}


} // namespace tf
} // namespace ou
