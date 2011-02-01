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

#include "AccountAdvisor.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CAccountOwner
{
public:

  typedef std::string keyAccountOwnerId_t;
  typedef CAccountAdvisor::keyAccountAdvisorId_t keyAccountAdvisorId_t;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "accountownerid", idAccountOwner );
      ou::db::Field( a, "accountadvisorid", idAccountAdvisor );
      ou::db::Field( a, "firstname", sFirstName );
      ou::db::Field( a, "lastname", sLastName );

      ou::db::Key( a, "accountownerid" );
      ou::db::Constraint( a, "accountadvisorid", CAccountAdvisor::m_sTableName, "accountadvisorid" );
    }

    keyAccountOwnerId_t idAccountOwner;
    keyAccountAdvisorId_t idAccountAdvisor;
    std::string sFirstName;
    std::string sLastName;

    TableRowDef( 
      const keyAccountOwnerId_t& idAccountOwner_, const keyAccountAdvisorId_t& idAccountAdvisor_,
      const std::string& sFirstName_, const std::string& sLastName_ ) 
      : idAccountOwner( idAccountOwner_ ), idAccountAdvisor( idAccountAdvisor_ ),
        sFirstName( sFirstName_ ), sLastName( sLastName_ ) {}
  };

  const static std::string m_sTableName;

  CAccountOwner( 
    const keyAccountOwnerId_t& sAccountOwnerId, 
    const keyAccountAdvisorId_t& sAccountAdvisorId,
    const std::string& sFirstName, const std::string& sLastName );
  CAccountOwner( const TableRowDef& row );
  ~CAccountOwner(void);

protected:
private:

  TableRowDef m_row;

};

} // namespace tf
} // namespace ou
