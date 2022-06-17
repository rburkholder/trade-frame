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
  Instrument::pInstrument_t pInstrument = InstrumentManager::Instance().Get( row.idInstrument );
  Position::pProvider_t pExecutionProvider = ProviderManager::Instance().Get( row.idExecutionAccount );
  Position::pProvider_t pDataProvider = ProviderManager::Instance().Get( row.idDataAccount );
  pPosition->Set( pInstrument, pExecutionProvider, pDataProvider );
}

void HandleOrderDetails( Instrument::idInstrument_t idInstrument, Instrument::pInstrument_t& pInstrument ) {
  pInstrument = InstrumentManager::Instance().Get( idInstrument );
}

void HandleInitializeManagers( ou::db::Session* pSession ) {
  ProviderManager::Instance().AttachToSession( pSession );
  InstrumentManager::Instance().AttachToSession( pSession );
  AccountManager::Instance().AttachToSession( pSession );
  PortfolioManager::Instance().AttachToSession( pSession );
  OrderManager::Instance().AttachToSession( pSession );

  // link up with PortfolioManager for call back
  PortfolioManager::Instance().SetOnPositionNeedDetails( &HandlePositionDetails );
  // link up with OrderManager for call back
  OrderManager::Instance().SetOnOrderNeedsDetails( &HandleOrderDetails );
}

void HandleDenitializeManagers( ou::db::Session& session ) {
  // take down the links
  OrderManager::Instance().SetOnOrderNeedsDetails( nullptr );
  PortfolioManager::Instance().SetOnPositionNeedDetails( nullptr );

  ProviderManager::Instance().DetachFromSession( &session );
  InstrumentManager::Instance().DetachFromSession( &session );
  AccountManager::Instance().DetachFromSession( &session );
  PortfolioManager::Instance().DetachFromSession( &session );
  OrderManager::Instance().DetachFromSession( &session );
}

} // namespace tf
} // namespace ou

