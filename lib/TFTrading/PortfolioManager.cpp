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
      ou::db::QueryFields<CPortfolio::TableRowDef>::pQueryFields_t pQuery
        = m_pDbSession->Insert<CPortfolio::TableRowDef>( const_cast<CPortfolio::TableRowDef&>( pPortfolio->GetRow() ) );
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
    pPortfolio = iter->second.pPortfolio;
  }
  else {
    PortfolioManagerQueries::PortfolioKey key( idPortfolio );
    ou::db::QueryFields<PortfolioManagerQueries::PortfolioKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
      = m_pDbSession->SQL<PortfolioManagerQueries::PortfolioKey>( "select * from portfolios", key ).Where( "portfolioid = ?" ).NoExecute();
    m_pDbSession->Bind<PortfolioManagerQueries::PortfolioKey>( pExistsQuery );
    if ( m_pDbSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
      CPortfolio::TableRowDef rowPortfolio;
      m_pDbSession->Columns<PortfolioManagerQueries::PortfolioKey, CPortfolio::TableRowDef>( pExistsQuery, rowPortfolio );
      pPortfolio.reset( new CPortfolio( rowPortfolio ) );

      std::pair<iterPortfolio_t, bool> response;
      response = m_mapPortfolio.insert( mapPortfolio_pair_t( idPortfolio, structPortfolio( pPortfolio ) ) );
      if ( false == response.second ) {
        throw std::runtime_error( "GetPortfolio:  couldn't insert portfolio into map" );
      }

      // load up related positions as well
      ou::db::QueryFields<PortfolioManagerQueries::PortfolioKey>::pQueryFields_t pPositionQuery
        = m_pDbSession->SQL<PortfolioManagerQueries::PortfolioKey>( "select * from positions", key ).Where( "portfolioid = ?" ).NoExecute();
      while ( m_pDbSession->Execute( pPositionQuery ) ) {
        CPosition::TableRowDef rowPosition;
        m_pDbSession->Columns<PortfolioManagerQueries::PortfolioKey, CPosition::TableRowDef>( pPositionQuery, rowPosition );
        pPosition_t pPosition( new CPosition( rowPosition ) );
        if ( 0 == OnPositionNeedsDetails ) {  // fill in instrument, execution, data 
          throw std::runtime_error( "CPortfolioManager::GetPortfolio has no Details Callback" );
        }
        OnPositionNeedsDetails( pPosition );
        response.first->second.mapPosition.insert( mapPosition_pair_t( rowPosition.sName, pPosition ) );
      }

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

  // need to delete position records first
//  DeleteRecord<idPortfolio_t, mapPortfolio_t, PortfolioManagerQueries::PortfolioKey>( 
//    idPortfolio, m_mapPortfolio, "portfolioid = ?" );

  // delete portfolio records
  DeleteRecord<idPortfolio_t, mapPortfolio_t, PortfolioManagerQueries::PortfolioKey>( 
    idPortfolio, m_mapPortfolio, "portfolioid = ?" );

}

//
// Position
//

CPortfolioManager::pPosition_t CPortfolioManager::ConstructPosition( 
    const idPortfolio_t& idPortfolio, const std::string& sName, const std::string& sAlgorithm,
    const idAccount_t& idExecutionAccount, const idAccount_t& idDataAccount, 
    const pProvider_t& pExecutionProvider, const pProvider_t& pDataProvider,
    pInstrument_cref pInstrument )
{
  pPosition_t pPosition;
  // confirm portfolio exists
  GetPortfolio( idPortfolio );
  iterPortfolio_t iterPortfolio = m_mapPortfolio.find( idPortfolio );
  if ( m_mapPortfolio.end() == iterPortfolio ) {  // should exist as we already just 'got' it
    throw std::runtime_error( "ConstructPosition:  idPortfolio does not exist" );
  }

  assert( "" != sName );

  iterPosition_t iterPosition = iterPortfolio->second.mapPosition.find( sName );
  if ( iterPortfolio->second.mapPosition.end() != iterPosition ) {
    throw std::runtime_error( "ConstructPosition:  sName already exists" );
  }

  pPosition.reset( new CPosition( pInstrument, pExecutionProvider, pDataProvider, idExecutionAccount, idDataAccount, idPortfolio, sName, sAlgorithm ) );
  if ( 0 == m_pDbSession ) {
    throw std::runtime_error( "ConstructPosition:  database session not available" );
  }

  ou::db::QueryFields<CPosition::TableRowDefNoKey>::pQueryFields_t pQuery
    = m_pDbSession->Insert<CPosition::TableRowDefNoKey>( 
    const_cast<CPosition::TableRowDefNoKey&>( dynamic_cast<const CPosition::TableRowDefNoKey&>( pPosition->GetRow() ) ) );
  idPosition_t idPosition( m_pDbSession->GetLastRowId() );
  pPosition->Set( idPosition );
  iterPortfolio->second.mapPosition.insert( mapPosition_pair_t( sName, pPosition ) );

  return pPosition;
}

CPortfolioManager::pPosition_t CPortfolioManager::GetPosition( const idPortfolio_t& idPortfolio, const std::string& sName ) {

  iterPortfolio_t iterPortfolio = m_mapPortfolio.find( idPortfolio );
  if ( m_mapPortfolio.end() == iterPortfolio ) {
    throw std::runtime_error( "GetPosition:  idPortfolio does not exist" );
  }
  assert( "" != sName );

  iterPosition_t iterPosition = iterPortfolio->second.mapPosition.find( sName );
  if ( iterPortfolio->second.mapPosition.end() == iterPosition ) {
    throw std::runtime_error( "GetPosition: sName does not exist" );
  }

  return iterPosition->second;
}

namespace PortfolioManagerQueries {
  struct PositionUpdate {
    template<class A>
    void Fields( A& a ) {
      row.Fields( a );
      ou::db::Field( a, "positionid", idPosition );
    }
    const ou::tf::keytypes::idPosition_t& idPosition;
    CPosition::TableRowDefNoKey& row;
    PositionUpdate( CPosition::TableRowDefNoKey& row_, const ou::tf::keytypes::idPosition_t& idPosition_ )
      : row( row_ ), idPosition( idPosition_ ) {};
  };
}

void CPortfolioManager::UpdatePosition( const idPortfolio_t& idPortfolio, const std::string& sName ) {
  pPosition_t pPosition( GetPosition( idPortfolio, sName ) );
  UpdateRecord<idPosition_t, CPosition::TableRowDefNoKey, PortfolioManagerQueries::PositionUpdate>(
    pPosition->GetRow().idPosition, dynamic_cast<const CPosition::TableRowDefNoKey&>( pPosition->GetRow() ), "positionid = ?" );
}

namespace PortfolioManagerQueries {
  struct PositionKey {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "positionid", idPosition );
    }
    const ou::tf::keytypes::idPosition_t& idPosition;
    PositionKey( const ou::tf::keytypes::idPosition_t& idPosition_ ): idPosition( idPosition_ ) {};
  };
}

void CPortfolioManager::DeletePosition( const idPortfolio_t& idPortfolio, const std::string& sName ) {
  pPosition_t pPosition( GetPosition( idPortfolio, sName ) );
  iterPortfolio_t iterPortfolio = m_mapPortfolio.find( idPortfolio );  // no error checking as performed in previous step
  iterPosition_t iterPosition = iterPortfolio->second.mapPosition.find( sName ); // no error checking as performed in previous step
  iterPortfolio->second.mapPosition.erase( iterPosition );
  DeleteRecord<idPosition_t, PortfolioManagerQueries::PositionKey>( pPosition->GetRow().idPosition, "positionid = ?" );
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
