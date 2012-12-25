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

// providers need to have been opened elsewhere, as this is a lookup into the provider map only
void HandlePositionDetails( CPosition::pPosition_t& pPosition ) {
  const CPosition::TableRowDef& row( pPosition->GetRow() );
  Instrument::pInstrument_t pInstrument = CInstrumentManager::Instance().Get( row.idInstrument );
  CPosition::pProvider_t pExecutionProvider = CProviderManager::Instance().Get( row.idExecutionAccount );
  CPosition::pProvider_t pDataProvider = CProviderManager::Instance().Get( row.idDataAccount );
  pPosition->Set( pInstrument, pExecutionProvider, pDataProvider ); 
}

void HandleOrderDetails( Instrument::idInstrument_t idInstrument, Instrument::pInstrument_t& pInstrument ) {
  pInstrument = CInstrumentManager::Instance().Get( idInstrument );
}

void HandleInitializeManagers( ou::db::Session* pSession ) {
  CProviderManager::Instance().AttachToSession( pSession );
  CInstrumentManager::Instance().AttachToSession( pSession );
  CAccountManager::Instance().AttachToSession( pSession );
  CPortfolioManager::Instance().AttachToSession( pSession );
  COrderManager::Instance().AttachToSession( pSession );

  // link up with PortfolioManager for call back
  CPortfolioManager::Instance().SetOnPositionNeedDetails( &HandlePositionDetails );
  // link up with OrderManager for call back
  COrderManager::Instance().SetOnOrderNeedsDetails( &HandleOrderDetails );
}

void HandleDenitializeManagers( ou::db::Session& session ) {
  // take down the links
  COrderManager::Instance().SetOnOrderNeedsDetails( 0 );
  CPortfolioManager::Instance().SetOnPositionNeedDetails( 0 );

  CProviderManager::Instance().DetachFromSession( &session );
  CInstrumentManager::Instance().DetachFromSession( &session );
  CAccountManager::Instance().DetachFromSession( &session );
  CPortfolioManager::Instance().DetachFromSession( &session );
  COrderManager::Instance().DetachFromSession( &session );
}

} // namespace tf
} // namespace ou

