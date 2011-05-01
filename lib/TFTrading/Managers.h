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

namespace ou { // One Unified
namespace tf { // TradeFrame

void HandleInitializeManagers( ou::db::CSession* pSession );
void HandleDenitializeManagers( ou::db::CSession& session );

} // namespace tf
} // namespace ou

