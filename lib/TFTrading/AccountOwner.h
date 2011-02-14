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

class CAccountOwner
{
public:

  typedef keytypes::idAccountOwner_t idAccountOwner_t;
  typedef keytypes::idAccountAdvisor_t idAccountAdvisor_t;

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
      ou::db::Constraint( a, "accountadvisorid", CAccountAdvisor::m_sTableName, "accountadvisorid" );
    }
  };

  const static std::string m_sTableName;

  CAccountOwner( 
    const idAccountOwner_t& idAccountOwner, 
    const idAccountAdvisor_t& idAccountAdvisor,
    const std::string& sFirstName, const std::string& sLastName );
  CAccountOwner( const TableRowDef& row );
  ~CAccountOwner(void);

protected:
private:

  TableRowDef m_row;

};

} // namespace tf
} // namespace ou
