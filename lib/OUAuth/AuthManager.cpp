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

#include "AuthManager.h"

#include <OUCommon/TimeSource.h>

namespace ou { // One Unified
namespace auth { // double entry accounting

AuthManager::AuthManager(void): ou::db::ManagerBase<AuthManager>() {
}

AuthManager::~AuthManager(void) {
}

//
// User 
//

bool AuthManager::Authenticate( const std::string& sLogin, const std::string& sPassword ) {
  miUsersByLogin_t::iterator iterLogin;
  bool bFound = LocateLogin( sLogin, iterLogin );
  if ( bFound ) {
    bFound = iterLogin->pUser->GetRow().sPassword == sPassword;
  }
  return bFound;
}

namespace AuthManagerQueries {
  struct LoginKey {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "login", sLogin );
    }
    const std::string& sLogin;
    LoginKey( const std::string& sLogin_ ): sLogin( sLogin_ ) {};
  };
}  // namespace

bool AuthManager::LocateLogin( const std::string& sLogin, miUsersByLogin_t::iterator& iter ) {
  bool bFound = false;
  iter = m_miUsers.get<ixLogin>().find( sLogin );
  if ( m_miUsers.get<ixLogin>().end() != iter ) {
    bFound = true;
  }
  else {
    if ( 0 != m_pSession ) {
      AuthManagerQueries::LoginKey keyLogin( sLogin );
      ou::db::QueryFields<AuthManagerQueries::LoginKey>::pQueryFields_t pLoginExistsQuery
        = m_pSession->SQL<AuthManagerQueries::LoginKey>( "select * from users", keyLogin ).Where( "login=?" ).NoExecute();
      m_pSession->Bind<AuthManagerQueries::LoginKey>( pLoginExistsQuery );
      if ( m_pSession->Execute( pLoginExistsQuery ) ) {
        bFound = true;
        User::TableRowDef rowUser;
        m_pSession->Columns<AuthManagerQueries::LoginKey, User::TableRowDef>( pLoginExistsQuery, rowUser );
        pUser_t pUser( new User( rowUser ) );
        m_miUsers.insert( UserRecord( rowUser.idUser, rowUser.sLogin, pUser ) );
      }
      else {
      }
    }
  }
  return bFound;
}

AuthManager::pUser_t AuthManager::ConstructUser( const std::string& sLogin, const std::string& sPassword,
    const std::string& sFirstName, const std::string& sLastName, const std::string& sEmail ) {
  miUsersByLogin_t::iterator iter;
  pUser_t pUser;
  if ( LocateLogin( sLogin, iter ) ) {
//    throw std::runtime_error( "Login already created" );
  }
  else {
    pUser.reset( new User( sLogin, sPassword, sFirstName, sLastName, sEmail ) );
    pUser->SetCreationTime( ou::TimeSource::LocalCommonInstance().Internal() );
    ou::db::QueryFields<User::TableRowDefNoKey>::pQueryFields_t pQueryUserWrite
      = m_pSession->Insert<User::TableRowDefNoKey>(
      const_cast<User::TableRowDefNoKey&>( dynamic_cast<const User::TableRowDefNoKey&>( pUser->GetRow() ) ) );
    idUser_t idUser = m_pSession->GetLastRowId();
    m_miUsers.insert( UserRecord( idUser, pUser->GetRow().sLogin, pUser ) );  // should throw if non-unique
  }
  return pUser; 
}

//AuthManager::pUser_t AuthManager::GetUser( idUser_t idUser ) {
//}

void AuthManager::DeleteUser( idUser_t idUser ) {
}

void AuthManager::HandleRegisterTables( ou::db::Session& session ) {
  session.RegisterTable<User::TableCreateDef>( tablenames::sUser );
}

void AuthManager::HandleRegisterRows( ou::db::Session& session ) {
  session.MapRowDefToTableName<User::TableRowDef>( tablenames::sUser );
  session.MapRowDefToTableName<User::TableRowDefNoKey>( tablenames::sUser );
}

void AuthManager::HandlePopulateTables( ou::db::Session& session ) {
  // InstrumentManager.cpp for example for multi-record
  User::TableRowDef system( false, "system", "ou", "System", "", "" );
  ou::db::QueryFields<User::TableRowDef>::pQueryFields_t pUser 
    = session.Insert<User::TableRowDef>( system ).NoExecute();
  session.Reset( pUser );
  session.Bind<User::TableRowDef>( pUser );
  session.Execute( pUser );
}

void AuthManager::AttachToSession( ou::db::Session* pSession ) {
  ManagerBase::AttachToSession( pSession );
  pSession->OnRegisterTables.Add( MakeDelegate( this, &AuthManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Add( MakeDelegate( this, &AuthManager::HandleRegisterRows ) );
  pSession->OnPopulate.Add( MakeDelegate( this, &AuthManager::HandlePopulateTables ) );
}

void AuthManager::DetachFromSession( ou::db::Session* pSession ) {
  pSession->OnRegisterTables.Remove( MakeDelegate( this, &AuthManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Remove( MakeDelegate( this, &AuthManager::HandleRegisterRows ) );
  pSession->OnPopulate.Remove( MakeDelegate( this, &AuthManager::HandlePopulateTables ) );
  ManagerBase::DetachFromSession( pSession );
}

} // namespace auth
} // namespace ou
