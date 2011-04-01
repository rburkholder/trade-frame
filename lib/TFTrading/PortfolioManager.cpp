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

#include "StdAfx.h"

#include "PortfolioManager.h"

// todo:  need to store the prepared queries for re-use

namespace ou { // One Unified
namespace tf { // TradeFrame

//
// Portfolio
//

CPortfolioManager::pPortfolio_t CPortfolioManager::ConstructPortfolio( 
  const idPortfolio_t& idPortfolio, const idAccountOwner_t& idAccountOwner, const std::string& sDescription 
  ) {
  pPortfolio_t pPortfolio;
  iterPortfolio_t iter = m_mapPortfolio.find( idPortfolio );
  if ( m_mapPortfolio.end() != iter ) {
    throw std::runtime_error( "CPortfolioManager::Create, portfolio already exists" );
  }
  else {
    pPortfolio.reset( new CPortfolio( idPortfolio, idAccountOwner, sDescription ) );
    m_mapPortfolio.insert( mapPortfolio_pair_t( idPortfolio, pPortfolio ) );
    if ( 0 != m_pDbSession ) {
      ou::db::QueryFields<CPortfolio::TableRowDef>( const_cast<CPortfolio::TableRowDef&>( pPortfolio->GetRow() ) );
    }
  }
  return pPortfolio;
}

namespace PortfolioManagerQueries {
  struct PortfolioKey {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "portfolioid", idPortfolio );
    }
    const ou::tf::keytypes::idPortfolio_t& idPortfolio;
    PortfolioKey( const ou::tf::keytypes::idPortfolio_t& idPortfolio_ ): idPortfolio( idPortfolio_ ) {};
  };
}

CPortfolioManager::pPortfolio_t CPortfolioManager::GetPortfolio( const idPortfolio_t& idPortfolio ) {

  assert( "" != idPortfolio );  // todo:  add this check in other handlers

  pPortfolio_t pPortfolio;
  iterPortfolio_t iter = m_mapPortfolio.find( idPortfolio );
  if ( m_mapPortfolio.end() != iter ) {
    pPortfolio = iter->second;
  }
  else {
    PortfolioManagerQueries::PortfolioKey key( idPortfolio );
    ou::db::QueryFields<PortfolioManagerQueries::PortfolioKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
      = m_pDbSession->SQL<PortfolioManagerQueries::PortfolioKey>( "select * from accounts", key ).Where( "accountid = ?" ).NoExecute();
    m_pDbSession->Bind<PortfolioManagerQueries::PortfolioKey>( pExistsQuery );
    if ( m_pDbSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
      CPortfolio::TableRowDef row;
      m_pDbSession->Columns<PortfolioManagerQueries::PortfolioKey, CPortfolio::TableRowDef>( pExistsQuery, row );
      pPortfolio.reset( new CPortfolio( row ) );
      m_mapPortfolio.insert( mapPortfolio_pair_t( idPortfolio, pPortfolio ) );
    }
    else {
      throw std::runtime_error( "CPortfolioManager::GetPortfolio, portfolio does not exist" );
    }
  }
  
  return pPortfolio;
}

namespace PortfolioManagerQueries {
  struct PortfolioUpdate {
    template<class A>
    void Fields( A& a ) {
      row.Fields( a );
      ou::db::Field( a, "portfolioid", idPortfolio );
    }
    const ou::tf::keytypes::idPortfolio_t& idPortfolio;
    CPortfolio::TableRowDef& row;
    PortfolioUpdate( CPortfolio::TableRowDef& row_, const ou::tf::keytypes::idPortfolio_t& idPortfolio_ )
      : row( row_ ), idPortfolio( idPortfolio_ ) {};
  };
}

void CPortfolioManager::UpdatePortfolio( const idPortfolio_t& idPortfolio ) {

  pPortfolio_t p( GetPortfolio( idPortfolio ) );  // has exception if does not exist

  UpdateRecord<idPortfolio_t, CPortfolio::TableRowDef, mapPortfolio_t, PortfolioManagerQueries::PortfolioUpdate>(
    idPortfolio, p->GetRow(), m_mapPortfolio, "portfolioid = ?" );

}

void CPortfolioManager::DeletePortfolio( const idPortfolio_t& idPortfolio ) {

  pPortfolio_t p( GetPortfolio( idPortfolio ) );  // has exception if does not exist

  DeleteRecord<idPortfolio_t, mapPortfolio_t, PortfolioManagerQueries::PortfolioKey>( 
    idPortfolio, m_mapPortfolio, "portfolioid = ?" );

}

//
// Table Management
//

void CPortfolioManager::RegisterTablesForCreation( void ) {
  m_pDbSession->RegisterTable<CPortfolio::TableCreateDef>( tablenames::sPortfolio );
  m_pDbSession->RegisterTable<CPosition::TableCreateDef>( tablenames::sPosition );
}

void CPortfolioManager::RegisterRowDefinitions( void ) {
  m_pDbSession->MapRowDefToTableName<CPortfolio::TableRowDef>( tablenames::sPortfolio );
  m_pDbSession->MapRowDefToTableName<CPosition::TableRowDef>( tablenames::sPosition );
}

void CPortfolioManager::PopulateTables( void ) {
}

} // namespace tf
} // namespace ou
