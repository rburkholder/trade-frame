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

#include "AccountOwner.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CAccount
{
public:

  typedef std::string keyAccountId_t;
  typedef CAccountOwner::keyAccountOwnerId_t keyAccountOwnerId_t;

  struct TableRowDef {
    template<class A>
    void Fields( A& A ) {
      ou::db::Field( a, "accountid" );
      ou::db::Field( a, "accountownerid" );
      ou::db::Field( a, "accountname" );
      ou::db::Field( a, "providername" );
      ou::db::Field( a, "brokername" );
      ou::db::Field( a, "brokeraccountid" );
      ou::db::Field( a, "login" );
      ou::db::Field( a, "password" );

      ou::db::Key( a, "accountid" );
      ou::db::Constraint( "accountownerid", CAccountOwner::m_sTableName, "accountownderid" );
    }

    keyAccountId_t idAccount;
    keyAccountOwnerId_t idAccountOwner;
    std::string sAccountName;
    std::string sProviderName;
    std::string sBrokerName;
    std::string sBrokerAccountId;
    std::string sLogin;
    std::string sPassword;

    TableRowDef( const keyAccountId_t& idAccount_, const keyAccountOwnerId_t& idAccountOwner_,
      const std::string& sAccountName_, const std::string& sProviderName_, 
      const std::string& sBrokerName_, const std::string& sBrokerAccountId_,
      const std::string& sLogin_, const std::string& sPassword_ ) 
      : idAccount( idAccount_ ), idAccountOwner( idAccountOwner_ ),
        sAccountName( sAccountName_ ), sProviderName( sProviderName_ ), 
        sBrokerName( sBrokerName_ ), sBrokerAccountId( sBrokerAccountId_ ),
        sLogin( sLogin_ ), sPassword( sPassword_ ) {};
  };

  const static std::string m_sTableName;

  CAccount(
    const keyAccountId_t& sAccountId,
    const keyAccountOwnerId_t& sAccountOwnerId,
    const std::string& sAccountName,
    const std::string& sProviderName,
    const std::string& sBrokerName,
    const std::string& sBrokerAccountId,
    const std::string& sLogin, 
    const std::string& sPassword
    );
  CAccount( const TableRowDef& row ) : m_row( row ) {};
  ~CAccount(void);

protected:

private:

  TableRowDef m_row;

};

} // namespace tf
} // namespace ou
