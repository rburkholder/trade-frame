/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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

/* 
 * File:   MoneyManager.cpp
 * Author: raymond@burkholder.net
 * 
 * Created on June 5, 2019, 8:37 AM
 */

// TODO: need to implement margin calculations with options
#include <TFOptions/Margin.h>

#include "MoneyManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

namespace {
  const size_t nMaxToAuthorize( 15 );
  const double dblPortfolioCashToTrade( 100000.0 );
  const double dblPortfolioMargin( 0.25 );
  const double dblAmountToTradePerInstrument = /* 3% */ 0.03 * ( dblPortfolioCashToTrade / dblPortfolioMargin ); // ~ 33 instances at 3% is ~100% investment
//    double dblAmountToTradePerInstrument = /* 3% */ 0.20 * ( m_dblPortfolioCashToTrade / m_dblPortfolioMargin ); // ~ fake for SPY
}

MoneyManager::MoneyManager()
: m_dblAuthorized {},
  m_nAuthorized {}
{
  std::cout
    << "Starting allocations at "
    << dblAmountToTradePerInstrument
    << " per instrument." <<
    std::endl;
}

MoneyManager::~MoneyManager() {
}

// authorize an underlying
bool MoneyManager::Authorize( pOrder_t& pOrder, pPosition_t& pPosition, pPortfolio_t& pPortfolio ) {
  switch ( pOrder->GetInstrument()->GetInstrumentType() ) {
    case ou::tf::InstrumentType::Future:
    case ou::tf::InstrumentType::Stock:
      break;
    default:
      assert( false );
  }
  const std::string& sUnderlying = pOrder->GetInstrument()->GetInstrumentName();
  return Authorize( sUnderlying );
}

// authorize an option
bool MoneyManager::Authorize( pOrder_t& pOrder, pPosition_t& pPosition, pPortfolio_t& pPortfolio, pWatch_t& pWatchUnderlying ) {
  switch ( pOrder->GetInstrument()->GetInstrumentType() ) {
    case ou::tf::InstrumentType::Option:
    case ou::tf::InstrumentType::FuturesOption:
      break;
    default:
      assert( false );
  }
  const std::string& sUnderlying = pWatchUnderlying->GetInstrument()->GetInstrumentName();
  return Authorize( sUnderlying );
}

bool MoneyManager::Authorize( const std::string& sUnderlying ) {
  using iterator = setNames_t::iterator;
  iterator iter;
  iter = m_setAuthorized.find( sUnderlying ); // TOOD: could use portfolio, but would limit number of underlying
  if ( m_setAuthorized.end() != iter ) {
    return true;
  }
  iter = m_setRejected.find( sUnderlying );
  if ( m_setRejected.end() != iter ) {
    return false;
  }
  if ( m_nAuthorized < nMaxToAuthorize ) {
    m_nAuthorized++;
    m_setAuthorized.insert( sUnderlying );
    return true;
  }
  else {
    m_setRejected.insert( sUnderlying );
    return false;
  }
  assert( false );  // should not arrive here
}

// this stuff could probably be rolled into Session with a template
void MoneyManager::AttachToSession( ou::db::Session* pSession ) {
  ManagerBase::AttachToSession( pSession );
//  pSession->OnRegisterTables.Add( MakeDelegate( this, &PortfolioManager::HandleRegisterTables ) );
//  pSession->OnRegisterRows.Add( MakeDelegate( this, &PortfolioManager::HandleRegisterRows ) );
//  pSession->OnPopulate.Add( MakeDelegate( this, &PortfolioManager::HandlePopulateTables ) );
//  pSession->OnLoad.Add( MakeDelegate( this, &PortfolioManager::HandleLoadTables ) );
}

void MoneyManager::DetachFromSession( ou::db::Session* pSession ) {
//  pSession->OnRegisterTables.Remove( MakeDelegate( this, &PortfolioManager::HandleRegisterTables ) );
//  pSession->OnRegisterRows.Remove( MakeDelegate( this, &PortfolioManager::HandleRegisterRows ) );
//  pSession->OnPopulate.Remove( MakeDelegate( this, &PortfolioManager::HandlePopulateTables ) );
//  pSession->OnLoad.Remove( MakeDelegate( this, &PortfolioManager::HandleLoadTables ) );
  ManagerBase::DetachFromSession( pSession );
}

} // namespace tf
} // namespace ou
