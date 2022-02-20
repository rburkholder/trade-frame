/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

  SessionBase();
  virtual ~SessionBase();

  void Open( const std::string& sDbFileName, enumOpenFlags = EOpenFlagsZero );
  void Close();

  bool IsOpen() const { return m_bOpened; }

protected:

  bool m_bOpened;

  std::string m_sDbFileName;

  void InitializeManagers() {}; // null placeholder
  void RegisterRowDefinitions() {}; // null placeholder
  void RegisterTablesForCreation() {}; // null placeholder
  void PopulateTables() {}; // null placeholder
  void DenitializeManagers() {}; // null placeholder
private:
};

template<class S, class T>
SessionBase<S,T>::SessionBase()
  : m_bOpened( false )
{
//  static_cast<T*>( this )->Initialize();
}

template<class S, class T>
SessionBase<S,T>::~SessionBase() {
  Close();
//  static_cast<T*>( this )->Denitialize();
}

template<class S, class T>
void SessionBase<S,T>::Open( const std::string& sDbFileName, enumOpenFlags flags ) {

  if ( !m_bOpened ) {
    if ( boost::filesystem::exists( sDbFileName ) ) {
      // open already created and loaded database
      dynamic_cast<S*>( this )->ImplOpen( sDbFileName, flags );
      static_cast<T*>( this )->InitializeManagers();
      static_cast<T*>( this )->RegisterRowDefinitions();
      static_cast<T*>( this )->LoadTables();
    }
    else {
      // create and build new database
      dynamic_cast<S*>( this )->ImplOpen( sDbFileName, EOpenFlagsAutoCreate );
      static_cast<T*>( this )->InitializeManagers();
      static_cast<T*>( this )->RegisterTablesForCreation();
      dynamic_cast<S*>( this )->CreateTables();
      static_cast<T*>( this )->RegisterRowDefinitions();
      static_cast<T*>( this )->PopulateTables();
    }
    m_bOpened = true;
  }

}

template<class S, class T>
void SessionBase<S,T>::Close() {
  if ( m_bOpened ) {
    m_bOpened = false;
    static_cast<T*>( this )->DenitializeManagers();
    dynamic_cast<S*>( this )->ImplClose();
  }
}

} // db
} // ou
