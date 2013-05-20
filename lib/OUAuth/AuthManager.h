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

// started 2013-05-19

// database caching might be an issue.
// in a multiuser context, caching user stuff may not be very good
// information between clients may get out of sync
// or deal with cache refresh events and intervals

#pragma once

#include <map>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <OUCommon/ManagerBase.h>

#include "KeyTypes.h"

#include "User.h"
#include "Privilege.h"

namespace ou { // One Unified
namespace auth { // double entry accounting

class AuthManager: public ou::db::ManagerBase<AuthManager> {
public:

  typedef keytypes::idUser_t idUser_t;
  typedef User::pUser_t pUser_t;

  // at some point in time, return a structure to the user plus privileges
  pUser_t ConstructUser( const std::string& sLogin, const std::string& sPassword,
    const std::string& sFirstName, const std::string& sLastName, const std::string& sEmail );
  //pUser_t GetUser( idUser_t idUser );
  //void UpdateUser( ... )
  void DeleteUser( idUser_t idUser );

  AuthManager(void);
  ~AuthManager(void);

  // need something more substantial:  use encrypted passwords in database
  bool Authenticate( const std::string& sLogin, const std::string& sPassword );

  void AttachToSession( ou::db::Session* pSession );
  void DetachFromSession( ou::db::Session* pSession );

protected:

private:

  struct ixUserId{};
  struct ixLogin{};

  struct UserRecord {
    idUser_t idUser;
    std::string sLogin;
    pUser_t pUser;
    UserRecord( idUser_t idUser_, const std::string& sLogin_, pUser_t pUser_ )
      : idUser( idUser_ ), sLogin( sLogin_ ), pUser( pUser_ ) {};
  };

  typedef boost::multi_index_container<
    UserRecord,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_unique<
        boost::multi_index::tag<ixUserId>, BOOST_MULTI_INDEX_MEMBER(UserRecord,idUser_t,idUser)>,
      boost::multi_index::ordered_unique<
        boost::multi_index::tag<ixLogin>, BOOST_MULTI_INDEX_MEMBER(UserRecord,std::string,sLogin)>
    >
  > miUsers_t;

  typedef miUsers_t::index<ixUserId>::type miUsersById_t;
  typedef miUsers_t::index<ixLogin>::type miUsersByLogin_t;

  miUsers_t m_miUsers;

  bool LocateLogin( const std::string& sLogin, miUsersByLogin_t::iterator& iter );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );
  void HandlePopulateTables( ou::db::Session& session );

};

} // namespace auth
} // namespace ou
