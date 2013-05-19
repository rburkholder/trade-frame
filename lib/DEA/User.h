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
namespace dea { // double entry accounting

class User {
public:

  typedef keytypes::idUser_t idUser_t;

  typedef boost::shared_ptr<User> pUser_t;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "userid", idUser );  // numeric value for key
      ou::db::Field( a, "login", sLogin );
      ou::db::Field( a, "password", sPassword );
      ou::db::Field( a, "fname" sFirstName );
      ou::db::Field( a, "lname", sLastName );
      ou::db::Field( a, "email", sEmail );
      ou::db::Field( a, "created", dtCreated );
    }

    idUser_t idUser;
    std::string sLogin;
    std::string sPassword;
    std::string sFirstName;
    std::string sLastName;
    std::string sEmail;
    ptime dtCreated;

  };

  User(void);
  ~User(void);
protected:
private:
};

} // namespace dea
} // namespace ou
