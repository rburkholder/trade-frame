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

  typedef keytypes::idAccountAdvisor_t idAccountAdvisor_t;
  typedef CAccountAdvisor::pAccountAdvisor_t pAccountAdvisor_t;

  typedef keytypes::idAccountOwner_t idAccountOwner_t;
  typedef CAccountOwner::pAccountOwner_t pAccountOwner_t;

  typedef keytypes::idAccount_t idAccount_t;
  typedef CAccount::pAccount_t pAccount_t;

  CAccountManager( void );
  ~CAccountManager(void);

  pAccountAdvisor_t ConstructAccountAdvisor( const idAccountAdvisor_t& idAccountAdvisor, const std::string& sAdvisorName, const std::string& sCompanyName = "" );
  pAccountAdvisor_t GetAccountAdvisor( const idAccountAdvisor_t& idAccountAdvisor );
  void UpdateAccountAdvisor( const idAccountAdvisor_t& idAccountAdvisor );
  void DeleteAccountAdvisor( const idAccountAdvisor_t& idAccountAdvisor );

  pAccountOwner_t ConstructAccountOwner(
    const idAccountOwner_t& idAccountOwner, const idAccountAdvisor_t& idAccountAdvisor,
    const std::string& sFirstName, const std::string& sLastName
    );
  pAccountOwner_t GetAccountOwner( const idAccountOwner_t& idAccountOwner );
  void UpdateAccountOwner( const idAccountOwner_t& idAccountOwner );  // uses existing class variables
  void DeleteAccountOwner( const idAccountOwner_t& idAccountOwner );

  pAccount_t ConstructAccount( 
    const idAccount_t& idAccount, const idAccountOwner_t& idAccountOwner,
    std::string sAccountName, keytypes::eidProvider_t idProvider, 
    std::string sBrokerName, std::string sBrokerAccountId, std::string sLogin, std::string sPassword
    );
  pAccount_t GetAccount( const idAccount_t& sAccountId );
  void UpdateAccount( const idAccount_t& sAccountId );  // uses existing class variables
  void DeleteAccount( const idAccount_t& sAccountId );

  void AttachToSession( ou::db::CSession* pSession );
  void DetachFromSession( ou::db::CSession* pSession );

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

  void HandleRegisterTables( ou::db::CSession& session );
  void HandleRegisterRows( ou::db::CSession& session );
  void HandlePopulateTables( ou::db::CSession& session );

};

} // namespace tf
} // namespace ou
