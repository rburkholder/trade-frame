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

#include "Session.h"

namespace ou {
namespace db {

Session::Session( void )
  : SessionImpl<ISqlite3>(),
    SessionBase<SessionImpl<ISqlite3>, Session>() {
}

Session::~Session( void ) {
}

void Session::InitializeManagers( void ) {
  OnInitializeManagers( this );
}

void Session::RegisterTablesForCreation( void ) {
  OnRegisterTables( *this );
}

void Session::RegisterRowDefinitions( void ) {
  OnRegisterRows( *this );
}

void Session::PopulateTables( void ) {
  OnPopulate( *this );
}

void Session::LoadTables( void ) {
  OnLoad( *this );
}

void Session::DenitializeManagers( void ) {
  OnDenitializeManagers( *this );
}

} // db
} // ou
