/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include "Database.h"

#include "Account.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

void Initialization( void );
void Denitialization( void );

void InitializeManagersDb( ou::db::CSession::pSession_t& pDbSession );
void RegisterTablesForCreation( void );
void RegisterRowDefinitions( void );
void PopulateTables( void );

} // namespace tf
} // namespace ou

