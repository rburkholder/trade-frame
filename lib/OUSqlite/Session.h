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

class Session: // session open/close control
  public SessionImpl<ISqlite3>,  // various session functionality
  public SessionBase<SessionImpl<ISqlite3>, Session>
{
  friend SessionImpl<ISqlite3>;
  friend SessionBase<SessionImpl<ISqlite3>, Session>;
public:

  using pSession_t = boost::shared_ptr<Session>;

  Session();
  virtual ~Session();

  ou::Delegate<Session*> OnInitializeManagers;  // various managers to be initialized with db pointers
  ou::Delegate<Session&> OnRegisterTables;  // get callbacks to register their tables
  ou::Delegate<Session&> OnRegisterRows;  // get callbacks to register their rows
  ou::Delegate<Session&> OnPopulate;  // get callbacks to populate their tables
  ou::Delegate<Session&> OnLoad;  // Either populate (database initialization) or Load (subseqent startups)
  ou::Delegate<Session&> OnDenitializeManagers; //

protected:

  void InitializeManagers();  // called by inherited SessionBase.h
  void RegisterRowDefinitions();  // called by inherited SessionBase.h
  void RegisterTablesForCreation();  // called by inherited SessionBase.h
  void PopulateTables();  // called by inherited SessionBase.h
  void LoadTables();      // called by inherited SessionBase.h
  void DenitializeManagers();  // called by inherited SessionBase.h

private:
};


} // db
} // ou
