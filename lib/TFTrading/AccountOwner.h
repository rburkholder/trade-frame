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

#include <boost/shared_ptr.hpp>

#include <OUSQL/Functions.h>

#include "KeyTypes.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class AccountOwner
{
public:

  typedef keytypes::idAccountOwner_t idAccountOwner_t;
  typedef keytypes::idAccountAdvisor_t idAccountAdvisor_t;
  typedef boost::shared_ptr<AccountOwner> pAccountOwner_t;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "accountownerid", idAccountOwner );  // needs to be unique in database
      ou::db::Field( a, "accountadvisorid", idAccountAdvisor );
      ou::db::Field( a, "firstname", sFirstName );
      ou::db::Field( a, "lastname", sLastName );
    }

    idAccountOwner_t idAccountOwner;
    idAccountAdvisor_t idAccountAdvisor;
    std::string sFirstName;
    std::string sLastName;

    TableRowDef( void ) {};
    TableRowDef( 
      const idAccountOwner_t& idAccountOwner_, const idAccountAdvisor_t& idAccountAdvisor_,
      const std::string& sFirstName_, const std::string& sLastName_ ) 
      : idAccountOwner( idAccountOwner_ ), idAccountAdvisor( idAccountAdvisor_ ),
        sFirstName( sFirstName_ ), sLastName( sLastName_ ) {}
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "accountownerid" );
      ou::db::Constraint( a, "accountadvisorid", tablenames::sAccountAdvisor, "accountadvisorid" );
    }
  };

  AccountOwner( 
    const idAccountOwner_t& idAccountOwner, 
    const idAccountAdvisor_t& idAccountAdvisor,
    const std::string& sFirstName, const std::string& sLastName );
  AccountOwner( const TableRowDef& row );
  ~AccountOwner(void);

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:
private:

  TableRowDef m_row;

};

} // namespace tf
} // namespace ou
