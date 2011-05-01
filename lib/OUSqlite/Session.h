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

#include <OUCommon/Delegate.h>

#include <OUSQL/SessionImpl.h>
#include <OUSQL/SessionBase.h>

#include "ISqlite3.h"

namespace ou {
namespace db {

// this is an example of how to integrate everything together for session management.

class CSession: 
  public CSessionImpl<ISqlite3>,  // various session functionality
  public SessionBase<CSessionImpl<ISqlite3>, CSession> {  // session open/close control
public:

  typedef boost::shared_ptr<CSession> pSession_t;

  CSession( void );
  virtual ~CSession( void );

  ou::Delegate<CSession*> OnInitializeManagers;  // various managers to be initialized with db pointers
  ou::Delegate<CSession&> OnRegisterTables;  // get callbacks to register their tables
  ou::Delegate<CSession&> OnRegisterRows;  // get callbacks to register their rows
  ou::Delegate<CSession&> OnPopulate;  // get callbacks to populate their tables
  ou::Delegate<CSession&> OnDenitializeManagers; //
  
  void InitializeManagers( void );  // called by inherited SessionBase.h
  void RegisterRowDefinitions( void );  // called by inherited SessionBase.h
  void RegisterTablesForCreation( void );  // called by inherited SessionBase.h
  void PopulateTables( void );  // called by inherited SessionBase.h
  void DenitializeManagers( void );  // called by inherieted SessionBase.h

protected:
private:
};


} // db
} // ou
