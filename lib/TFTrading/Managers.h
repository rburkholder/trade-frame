/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include <OUSQL/Session.h>

#include "ProviderManager.h"
#include "InstrumentManager.h"
#include "AccountManager.h"
#include "PortfolioManager.h"
#include "OrderManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

template<class DB>
void InitializeManagers( ou::db::CSession<DB>* pDbSession ) {
//  CProviderManager::Instance().SetSession( pDbSession );
//  CInstrumentManager::Instance().SetSession( pDbSession );
//  CAccountManager::Instance().SetSession( pDbSession );
//  CPortfolioManager::Instance().SetSession( pDbSession );
//  COrderManager::Instance().SetSession( pDbSession );
}

} // namespace tf
} // namespace ou

