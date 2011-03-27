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
#include <map>

#include <sstream>

#include "KeyTypes.h"

#include "ManagerBase.h"
#include "AccountAdvisor.h"
#include "AccountOwner.h"
#include "Account.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CAccountManager: public ManagerBase<CAccountManager> {
public:

  typedef CAccountAdvisor::idAccountAdvisor_t idAccountAdvisor_t;
  typedef CAccountOwner::idAccountOwner_t idAccountOwner_t;
  typedef CAccount::idAccount_t idAccount_t;

  typedef CAccountAdvisor::pAccountAdvisor_t pAccountAdvisor_t;
  typedef CAccountOwner::pAccountOwner_t pAccountOwner_t;
  typedef CAccount::pAccount_t pAccount_t;

  CAccountManager( void );
  ~CAccountManager(void);

  pAccountAdvisor_t AddAccountAdvisor( const idAccountAdvisor_t& sAdvisorId, const std::string& sAdvisorName );
  pAccountAdvisor_t GetAccountAdvisor( const idAccountAdvisor_t& sAdvisorId );
  pAccountAdvisor_t UpdateAccountAdvisor( const idAccountAdvisor_t& sAdvisorId );  // uses existing class variables
  void DeleteAccountAdvisor( const idAccountAdvisor_t& sAdvisorId );

  pAccountOwner_t AddAccountOwner( const idAccountOwner_t& sAdvisorId, const std::string& sAccountAdvisorName );
  pAccountOwner_t GetAccountOwner( const idAccountOwner_t& sAdvisorId );
  pAccountOwner_t UpdateAccountOwner( const idAccountOwner_t& sAdvisorId );  // uses existing class variables
  void DeleteAccountOwner( const idAccountOwner_t& sAdvisorId );

  pAccountAdvisor_t AddAccount( const idAccount_t& sAccountId, const std::string& sAccountAdvisorName );
  pAccountAdvisor_t GetAccount( const idAccount_t& sAccountId );
  pAccountAdvisor_t UpdateAccount( const idAccount_t& sAccountId );  // uses existing class variables
  void DeleteAccount( const idAccount_t& sAccountId );

  void RegisterTablesForCreation( void );
  void RegisterRowDefinitions( void );
  void PopulateTables( void );

protected:

private:

  typedef std::pair<idAccountAdvisor_t, pAccountAdvisor_t> pairAccountAdvisor_t;
  typedef std::map<idAccountAdvisor_t, pAccountAdvisor_t> mapAccountAdvisor_t;
  typedef mapAccountAdvisor_t::iterator iterAccountAdvisor_t;
  mapAccountAdvisor_t m_mapAccountAdvisor;

  typedef std::pair<idAccountOwner_t, pAccountOwner_t> pairAccountOwner_t;
  typedef std::map<idAccountOwner_t, pAccountOwner_t> mapAccountOwner_t;
  typedef mapAccountOwner_t::iterator iterAccountOwner_t;
  mapAccountOwner_t m_mapAccountOwner;

  typedef std::pair<idAccount_t, pAccount_t> pairAccount_t;
  typedef std::map<idAccount_t, pAccount_t> mapAccount_t;
  typedef mapAccount_t::iterator iterAccount_t;
  mapAccount_t m_mapAccount;

};

} // namespace tf
} // namespace ou
