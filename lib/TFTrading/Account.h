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

#include "KeyTypes.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CAccount
{
public:

  typedef keytypes::idAccount_t idAccount_t;
  typedef keytypes::idAccountOwner_t idAccountOwner_t;

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

    idAccount_t idAccount;
    idAccountOwner_t idAccountOwner;
    std::string sAccountName;
    std::string sProviderName;
    std::string sBrokerName;
    std::string sBrokerAccountId;
    std::string sLogin;
    std::string sPassword;

    TableRowDef( const idAccount_t& idAccount_, const idAccountOwner_t& idAccountOwner_,
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
    const idAccount_t& idAccount,
    const idAccountOwner_t& idAccountOwner,
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
