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

#include "ProviderManager.h"
#include "InstrumentManager.h"
#include "AccountManager.h"
#include "PortfolioManager.h"
#include "OrderManager.h"

#include "Managers.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// providers need to have been opened elsewhere, as this is a lookup into the provider map only
void HandlePositionDetails( Position::pPosition_t& pPosition ) {
  const Position::TableRowDef& row( pPosition->GetRow() );
  Instrument::pInstrument_t pInstrument = InstrumentManager::GlobalInstance().Get( row.idInstrument );
  Position::pProvider_t pExecutionProvider = ProviderManager::GlobalInstance().Get( row.idExecutionAccount );
  Position::pProvider_t pDataProvider = ProviderManager::GlobalInstance().Get( row.idDataAccount );
  pPosition->Set( pInstrument, pExecutionProvider, pDataProvider );
}

void HandleOrderDetails( Instrument::idInstrument_t idInstrument, Instrument::pInstrument_t& pInstrument ) {
  pInstrument = InstrumentManager::GlobalInstance().Get( idInstrument );
}

void HandleInitializeManagers( ou::db::Session* pSession ) {
  ProviderManager::GlobalInstance().AttachToSession( pSession );
  InstrumentManager::GlobalInstance().AttachToSession( pSession );
  AccountManager::GlobalInstance().AttachToSession( pSession );
  PortfolioManager::GlobalInstance().AttachToSession( pSession );
  OrderManager::GlobalInstance().AttachToSession( pSession );

  // link up with PortfolioManager for call back
  PortfolioManager::GlobalInstance().SetOnPositionNeedDetails( &HandlePositionDetails );
  // link up with OrderManager for call back
  OrderManager::GlobalInstance().SetOnOrderNeedsDetails( &HandleOrderDetails );
}

void HandleDenitializeManagers( ou::db::Session& session ) {
  // take down the links
  OrderManager::GlobalInstance().SetOnOrderNeedsDetails( nullptr );
  PortfolioManager::GlobalInstance().SetOnPositionNeedDetails( nullptr );

  ProviderManager::GlobalInstance().DetachFromSession( &session );
  InstrumentManager::GlobalInstance().DetachFromSession( &session );
  AccountManager::GlobalInstance().DetachFromSession( &session );
  PortfolioManager::GlobalInstance().DetachFromSession( &session );
  OrderManager::GlobalInstance().DetachFromSession( &session );
}

} // namespace tf
} // namespace ou

