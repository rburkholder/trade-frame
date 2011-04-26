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

// inherited by ISqlite3 and IPostgresql for specialization

#pragma once

#include <string>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include "Constants.h"

namespace ou {
namespace db {

template<class S, class T> // S Session variable (ou::db::CSession), T class for CRTP operations
class SessionBase {
public:

  typedef typename S::pSession_t pSession_t;

  SessionBase(void);
  virtual ~SessionBase(void);

  void Open( const std::string& sDbFileName, enumOpenFlags = EOpenFlagsZero );
  void Close( void );

protected:

  bool m_bOpened;

  std::string m_sDbFileName;

  pSession_t m_pSession;

  void Initialize( void ) {};
  void InitializeManagersDb( pSession_t& pSession ) {};
  void RegisterRowDefinitions( void ) {};
  void RegisterTablesForCreation( void ) {};
  void PopulateTables( void ) {};
  void Denitialize( void ) {};

private:
};

template<class S, class T>
SessionBase<S,T>::SessionBase(void) 
  : m_bOpened( false )
{
  static_cast<T*>( this )->Initialize();
}

template<class S, class T>
SessionBase<S,T>::~SessionBase(void) {
  Close();
  static_cast<T*>( this )->Denitialize();
}

template<class S, class T>
void SessionBase<S,T>::Open( const std::string& sDbFileName, enumOpenFlags flags ) {

  if ( !m_bOpened ) {
    m_pSession.reset( new S );
    if ( boost::filesystem::exists( sDbFileName ) ) {
      // open already created and loaded database
      dynamic_cast<S*>( this )->ImplOpen( sDbFileName, flags );
      static_cast<T*>( this )->InitializeManagersDb( m_pSession );
      static_cast<T*>( this )->RegisterRowDefinitions();
    }
    else {
      // create and build new database
      dynamic_cast<S*>( this )->ImplOpen( sDbFileName, EOpenFlagsAutoCreate );
      static_cast<T*>( this )->InitializeManagersDb( m_pSession );
      static_cast<T*>( this )->RegisterTablesForCreation();
      dynamic_cast<S*>( this )->CreateTables();
      static_cast<T*>( this )->RegisterRowDefinitions();
      static_cast<T*>( this )->PopulateTables();
    }
    m_bOpened = true;
  }

}

template<class S, class T>
void SessionBase<S,T>::Close( void ) {
  if ( m_bOpened ) {
    m_bOpened = false;
    dynamic_cast<S*>( this )->ImplClose();
  }
}

} // db
} // ou
