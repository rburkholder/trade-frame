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

#pragma once

#include <OUCommon/Delegate.h>

#include <OUSQL/SessionImpl.h>
#include <OUSQL/SessionBase.h>

#include "ISqlite3.h"

// 2011/04/21
// no in-session caching.  everything comes from the database.
// need to think about session/window/view management if logged in from several browsers

// need to create various classes for various execeptions

namespace ou {
namespace db {

// this is an example of how to integrate everything together for session management.

class Session: 
  public SessionImpl<ISqlite3>,  // various session functionality
  public SessionBase<SessionImpl<ISqlite3>, Session> {  // session open/close control
public:

  typedef boost::shared_ptr<Session> pSession_t;

  Session( void );
  virtual ~Session( void );

  ou::Delegate<Session*> OnInitializeManagers;  // various managers to be initialized with db pointers
  ou::Delegate<Session&> OnRegisterTables;  // get callbacks to register their tables
  ou::Delegate<Session&> OnRegisterRows;  // get callbacks to register their rows
  ou::Delegate<Session&> OnPopulate;  // get callbacks to populate their tables
  ou::Delegate<Session&> OnLoad;  // Either populate (database initialization) or Load (subseqent startups)
  ou::Delegate<Session&> OnDenitializeManagers; //
  
  void InitializeManagers( void );  // called by inherited SessionBase.h
  void RegisterRowDefinitions( void );  // called by inherited SessionBase.h
  void RegisterTablesForCreation( void );  // called by inherited SessionBase.h
  void PopulateTables( void );  // called by inherited SessionBase.h
  void LoadTables( void );      // called by inherited SessionBase.h
  void DenitializeManagers( void );  // called by inherieted SessionBase.h

protected:
private:
};


} // db
} // ou
