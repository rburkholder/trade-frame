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
  CInstrument::pInstrument_t pInstrument = CInstrumentManager::Instance().Get( row.idInstrument );
  CPosition::pProvider_t pExecutionProvider = CProviderManager::Instance().Get( row.idExecutionAccount );
  CPosition::pProvider_t pDataProvider = CProviderManager::Instance().Get( row.idDataAccount );
  pPosition->Set( pInstrument, pExecutionProvider, pDataProvider ); 
}

void HandleOrderDetails( CInstrument::idInstrument_t idInstrument, CInstrument::pInstrument_t& pInstrument ) {
  pInstrument = CInstrumentManager::Instance().Get( idInstrument );
}

void Initialize( void ) {
  // link up with PortfolioManager for call back
  CPortfolioManager::Instance().SetOnPositionNeedDetails( &HandlePositionDetails );
  // link up with OrderManager for call back
  COrderManager::Instance().SetOnOrderNeedsDetails( &HandleOrderDetails );
}

void Denitialize( void ) {
  // take down the links
  COrderManager::Instance().SetOnOrderNeedsDetails( 0 );
  CPortfolioManager::Instance().SetOnPositionNeedDetails( 0 );
}

void InitializeManagersDb( ou::db::CSession::pSession_t& pDbSession ) {
  CProviderManager::Instance().SetDbSession( pDbSession );
  CInstrumentManager::Instance().SetDbSession( pDbSession );
  CAccountManager::Instance().SetDbSession( pDbSession );
  CPortfolioManager::Instance().SetDbSession( pDbSession );
  COrderManager::Instance().SetDbSession( pDbSession );
}

void RegisterTablesForCreation( void ) {
  CProviderManager::Instance().RegisterTablesForCreation(  );
  CInstrumentManager::Instance().RegisterTablesForCreation(  );
  CAccountManager::Instance().RegisterTablesForCreation(  );
  CPortfolioManager::Instance().RegisterTablesForCreation(  );
  COrderManager::Instance().RegisterTablesForCreation(  );
}

void RegisterRowDefinitions( void ) {
  CProviderManager::Instance().RegisterRowDefinitions(  );
  CInstrumentManager::Instance().RegisterRowDefinitions(  );
  CAccountManager::Instance().RegisterRowDefinitions(  );
  CPortfolioManager::Instance().RegisterRowDefinitions(  );
  COrderManager::Instance().RegisterRowDefinitions(  );
}

void PopulateTables( void ) {
  CProviderManager::Instance().PopulateTables(  );
  CInstrumentManager::Instance().PopulateTables(  );
  CAccountManager::Instance().PopulateTables(  );
  CPortfolioManager::Instance().PopulateTables(  );
  COrderManager::Instance().PopulateTables(  );
}

} // namespace tf
} // namespace ou

