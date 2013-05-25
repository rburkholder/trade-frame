/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

// started 2013-05-18

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include "KeyTypes.h"

namespace ou { // One Unified
namespace auth { // auth

class User {
public:

  typedef keytypes::idUser_t idUser_t;

  typedef boost::shared_ptr<User> pUser_t;

  struct TableRowDefNoKey {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "isgroup", bIsGroup );  // user or group
      ou::db::Field( a, "login", sLogin );
      ou::db::Field( a, "password", sPassword );
      ou::db::Field( a, "fname", sFirstName );
      ou::db::Field( a, "lname", sLastName );
      ou::db::Field( a, "email", sEmail );
      ou::db::Field( a, "created", dtCreated );
    }

    bool bIsGroup;
    std::string sLogin;
    std::string sPassword;
    std::string sFirstName;
    std::string sLastName;
    std::string sEmail;
    ptime dtCreated;

    TableRowDefNoKey( void ): bIsGroup( false ), dtCreated( boost::date_time::special_values::not_a_date_time  ) {};
    TableRowDefNoKey( const TableRowDefNoKey& row ):
      bIsGroup( row.bIsGroup ), sLogin( row.sLogin ), sFirstName( row.sFirstName ), sLastName( row.sLastName ),
      sEmail( row.sEmail ), dtCreated( row.dtCreated ) {};
    TableRowDefNoKey( bool bIsGroup_, const std::string& sLogin_, const std::string& sPassword_,
      const std::string& sFirstName_, const std::string& sLastName_, const std::string& sEmail_ ):
      bIsGroup( bIsGroup_ ), sLogin( sLogin_ ), sPassword( sPassword_ ), sFirstName( sFirstName_ ), sLastName( sLastName_ ), sEmail( sEmail_ ) {};
  };

  struct TableRowDef: TableRowDefNoKey {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "userid", idUser ); // numeric value for key
      TableRowDefNoKey::Fields( a );
    }

    idUser_t idUser;

    TableRowDef( void ): idUser( 0 ), TableRowDefNoKey() {};
    TableRowDef( bool bIsGroup_, const std::string& sLogin_, const std::string& sPassword_,
      const std::string& sFirstName_, const std::string& sLastName_, const std::string& sEmail_ ):
      idUser( 0 ), TableRowDefNoKey( bIsGroup_, sLogin_, sPassword_, sFirstName_, sLastName_, sEmail_ ) {};
    TableRowDef( idUser_t idUser_, bool bIsGroup_, const std::string& sLogin_, const std::string& sPassword_,
      const std::string& sFirstName_, const std::string& sLastName_, const std::string& sEmail_ ):
      idUser( idUser_ ), TableRowDefNoKey( bIsGroup_, sLogin_, sPassword_, sFirstName_, sLastName_, sEmail_ ) {};
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "userid" );
      // secondary indexes:  sLogin, sEmail, maybe sFirstName, sLastName
    }
  };

  User( const TableRowDef& row ): m_row( row ) {};
  User( const std::string& sLogin_, const std::string& sPassword_,
      const std::string& sFirstName_, const std::string& sLastName_, const std::string& sEmail_);
  ~User(void);

  void SetCreationTime( const ptime& dt ) { m_row.dtCreated = dt; };
  void SetKey( const idUser_t& idUser ) { m_row.idUser = idUser; };

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:
private:
  TableRowDef m_row;
};

} // namespace auth
} // namespace ou
