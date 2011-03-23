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

#include "StdAfx.h"

#include "Managers.h"

#include "ProviderManager.h"
#include "InstrumentManager.h"
#include "AccountManager.h"
#include "PortfolioManager.h"
#include "OrderManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

void InitializeManagersDb( ou::db::CSession::pSession_t& pDbSession ) {
  CProviderManager::Instance().SetDbSession( pDbSession );
  CInstrumentManager::Instance().SetDbSession( pDbSession );
  CAccountManager::Instance().SetDbSession( pDbSession );
  CPortfolioManager::Instance().SetDbSession( pDbSession );
  COrderManager::Instance().SetDbSession( pDbSession );
}

} // namespace tf
} // namespace ou

