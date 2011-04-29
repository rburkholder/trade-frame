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

#include "Session.h"

namespace ou {
namespace db {

CSession::CSession( void )
  : CSessionImpl<ISqlite3>(),
    SessionBase<CSessionImpl<ISqlite3>, CSession>() {
}

CSession::~CSession( void ) {
}

void CSession::InitializeManagers( void ) {
  OnInitializeManagers( this );
}

void CSession::RegisterTablesForCreation( void ) {
  OnRegisterTables( *this );
}

void CSession::RegisterRowDefinitions( void ) {
  OnRegisterRows( *this );
}

void CSession::PopulateTables( void ) {
  OnPopulate( *this );
}

void CSession::DenitializeManagers( void ) {
  OnDenitializeManagers( *this );
}

} // db
} // ou
