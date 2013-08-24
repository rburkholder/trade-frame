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

PortfolioManager::pPortfolio_t PortfolioManager::ConstructPortfolio( 
  const idPortfolio_t& idPortfolio, const idAccountOwner_t& idAccountOwner, const idPortfolio_t& idOwner, 
  EPortfolioType ePortfolioType, currency_t eCurrency, const std::string& sDescription 
  ) {
  pPortfolio_t pPortfolio;
  mapPortfolios_iter_t iter = m_mapPortfolios.find( idPortfolio );
  if ( m_mapPortfolios.end() != iter ) {
    throw std::runtime_error( "PortfolioManager::Create, portfolio already exists" );
  }
  else {
    pPortfolio.reset( new Portfolio( idPortfolio, idAccountOwner, idOwner, ePortfolioType, eCurrency, sDescription ) );
    m_mapPortfolios.insert( mapPortfolio_pair_t( idPortfolio, pPortfolio ) );
    if ( 0 != m_pSession ) {
      ou::db::QueryFields<Portfolio::TableRowDef>::pQueryFields_t pQuery
        = m_pSession->Insert<Portfolio::TableRowDef>( const_cast<Portfolio::TableRowDef&>( pPortfolio->GetRow() ) );
    }
    pPortfolio->OnCommissionUpdate.Add( MakeDelegate( this, &PortfolioManager::HandlePortfolioOnCommission ) );
    pPortfolio->OnExecutionUpdate.Add( MakeDelegate( this, &PortfolioManager::HandlePortfolioOnExecution ) );
  }

  OnPortfolioAdded( idPortfolio );

  return pPortfolio;
}

//////

namespace PortfolioManagerQueries {
  struct UpdatePositionData {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "ordersidepending", eOrderSidePending );
      ou::db::Field( a, "quantitypending", nPositionPending );
      ou::db::Field( a, "ordersideactive", eOrderSideActive );
      ou::db::Field( a, "quantityactive", nPositionActive );
      ou::db::Field( a, "constructedvalue", dblConstructedValue );
      ou::db::Field( a, "marketvalue", dblMarketValue );
      ou::db::Field( a, "unrealizedpl", dblUnRealizedPL );
      ou::db::Field( a, "realizedpl", dblRealizedPL );
      ou::db::Field( a, "positionid", idPosition );
    }
    const ou::tf::keytypes::idPosition_t idPosition;
    OrderSide::enumOrderSide eOrderSidePending;  
    boost::uint32_t nPositionPending;
    OrderSide::enumOrderSide eOrderSideActive;
    boost::uint32_t nPositionActive;
    double dblConstructedValue;
    double dblMarketValue; 
    double dblUnRealizedPL;
    double dblRealizedPL; 
    UpdatePositionData( 
      const ou::tf::keytypes::idPosition_t idPosition_, 
      OrderSide::enumOrderSide eOrderSidePending_, boost::uint32_t nPositionPending_, 
      OrderSide::enumOrderSide eOrderSideActive_,  boost::uint32_t nPositionActive_, 
      double dblConstructedValue_, double dblMarketValue_,
      double dblUnRealizedPL_, double dblRealizedPL_ ) 
      : idPosition( idPosition_ ), 
        eOrderSidePending( eOrderSidePending_ ), nPositionPending( nPositionPending_ ),
        eOrderSideActive( eOrderSideActive_ ), nPositionActive( nPositionActive_ ), 
        dblConstructedValue( dblConstructedValue_ ), dblMarketValue( dblMarketValue_ ),
        dblUnRealizedPL( dblUnRealizedPL_ ), dblRealizedPL( dblRealizedPL_ ) {};
  };
}

void PortfolioManager::HandlePositionOnExecution( const Position& position ) {
  if ( 0 != m_pSession ) {
    const Position::TableRowDef& row( position.GetRow() );
    PortfolioManagerQueries::UpdatePositionData update( row.idPosition, row.eOrderSidePending, row.nPositionPending,
      row.eOrderSideActive, row.nPositionActive, row.dblConstructedValue, row.dblMarketValue, row.dblUnRealizedPL, row.dblRealizedPL );
    ou::db::QueryFields<PortfolioManagerQueries::UpdatePositionData>::pQueryFields_t pQuery
      = m_pSession->SQL<PortfolioManagerQueries::UpdatePositionData>( 
        "update positions set ordersidepending=?, quantitypending=?, ordersideactive=?, quantityactive=?, constructedvalue=?, marketvalue=?, unrealizedpl=?, realizedpl=?", update ).Where( "positionid=?" );
  }
}

//////

namespace PortfolioManagerQueries {
  struct UpdatePositionCommission {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "commission", dblCommissionPaid );
      ou::db::Field( a, "positionid", idPosition );
    }
    const ou::tf::keytypes::idPosition_t idPosition;
    double dblCommissionPaid;
    UpdatePositionCommission( const ou::tf::keytypes::idPosition_t idPosition_, double dblCommissionPaid_ )
      : idPosition( idPosition_ ), dblCommissionPaid( dblCommissionPaid_ ) {};
  };
}

void PortfolioManager::HandlePositionOnCommission( const Position& position ) {
  if ( 0 != m_pSession ) {
    const Position::TableRowDef& row( position.GetRow() );
    PortfolioManagerQueries::UpdatePositionCommission update( row.idPosition, row.dblCommissionPaid );
    ou::db::QueryFields<PortfolioManagerQueries::UpdatePositionCommission>::pQueryFields_t pQuery
      = m_pSession->SQL<PortfolioManagerQueries::UpdatePositionCommission>( "update positions set commission=?", update ).Where( "positionid=?" );
  }
}  // the Where could be appended with boost::fusion type structure for the fields, and bind?
  // need to cache the queries

/////

namespace PortfolioManagerQueries {
  struct UpdatePortfolioRealizedPL {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "realizedpl", dblRealizedPL );
      ou::db::Field( a, "portfolioid", idPortfolio );
    }
    const ou::tf::keytypes::idPortfolio_t idPortfolio;
    double dblRealizedPL;
    UpdatePortfolioRealizedPL( const ou::tf::keytypes::idPortfolio_t idPortfolio_, double dblRealizedPL_ )
      : idPortfolio( idPortfolio_ ), dblRealizedPL( dblRealizedPL_ ) {};
  };
}

void PortfolioManager::HandlePortfolioOnExecution( const Portfolio& portfolio ) {
  if ( 0 != m_pSession ) {
    const Portfolio::TableRowDef& row( portfolio.GetRow() );
    PortfolioManagerQueries::UpdatePortfolioRealizedPL update( row.idPortfolio, row.dblRealizedPL );
    ou::db::QueryFields<PortfolioManagerQueries::UpdatePortfolioRealizedPL>::pQueryFields_t pQuery
      = m_pSession->SQL<PortfolioManagerQueries::UpdatePortfolioRealizedPL>( "update portfolios set realizedpl=?", update ).Where( "portfolioid=?" );
  }
}

////////

namespace PortfolioManagerQueries {
  struct UpdatePortfolioCommission {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "commission", dblCommissionsPaid );
      ou::db::Field( a, "portfolioid", idPortfolio );
    }
    const ou::tf::keytypes::idPortfolio_t idPortfolio;
    double dblCommissionsPaid;
    UpdatePortfolioCommission( const ou::tf::keytypes::idPortfolio_t idPortfolio_, double dblCommissionsPaid_ )
      : idPortfolio( idPortfolio_ ), dblCommissionsPaid( dblCommissionsPaid_ ) {};
  };
}

void PortfolioManager::HandlePortfolioOnCommission( const Portfolio& portfolio ) {
  if ( 0 != m_pSession ) {
    const Portfolio::TableRowDef& row( portfolio.GetRow() );
    PortfolioManagerQueries::UpdatePortfolioCommission update( row.idPortfolio, row.dblCommissionsPaid );
    ou::db::QueryFields<PortfolioManagerQueries::UpdatePortfolioCommission>::pQueryFields_t pQuery
      = m_pSession->SQL<PortfolioManagerQueries::UpdatePortfolioCommission>( "update portfolios set commission=?", update ).Where( "portfolioid=?" );
  }
}

///////

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

PortfolioManager::pPortfolio_t PortfolioManager::GetPortfolio( const idPortfolio_t& idPortfolio ) {

  assert( "" != idPortfolio );  // todo:  add this check in other handlers

  pPortfolio_t pPortfolio;
  mapPortfolios_iter_t iter = m_mapPortfolios.find( idPortfolio );
  if ( m_mapPortfolios.end() != iter ) {
    pPortfolio = iter->second.pPortfolio;
  }
  else {
    // following portfolio / position code is shared with LoadActivePortfolios and could be factored out
    PortfolioManagerQueries::PortfolioKey key( idPortfolio );
    ou::db::QueryFields<PortfolioManagerQueries::PortfolioKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
      = m_pSession->SQL<PortfolioManagerQueries::PortfolioKey>( "select * from portfolios", key ).Where( "portfolioid = ?" ).NoExecute();
    m_pSession->Bind<PortfolioManagerQueries::PortfolioKey>( pExistsQuery );
    if ( m_pSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
      Portfolio::TableRowDef rowPortfolio;
      m_pSession->Columns<PortfolioManagerQueries::PortfolioKey, Portfolio::TableRowDef>( pExistsQuery, rowPortfolio );
      pPortfolio.reset( new Portfolio( rowPortfolio ) );

      std::pair<mapPortfolios_iter_t, bool> response;
      response = m_mapPortfolios.insert( mapPortfolio_pair_t( idPortfolio, structPortfolio( pPortfolio ) ) );
      if ( false == response.second ) {
        throw std::runtime_error( "GetPortfolio:  couldn't insert portfolio into map" );
      }

//      if ( !rowPortfolio.idOwner.empty() ) {
        UpdateReportingPortfolio( rowPortfolio.idOwner, rowPortfolio.idPortfolio );
//      }

      pPortfolio->OnCommissionUpdate.Add( MakeDelegate( this, &PortfolioManager::HandlePortfolioOnCommission ) );
      pPortfolio->OnExecutionUpdate.Add( MakeDelegate( this, &PortfolioManager::HandlePortfolioOnExecution ) );

      OnPortfolioAdded( idPortfolio );

      LoadPositions( idPortfolio, response.first->second.mapPosition );

    }
    else {
      throw std::runtime_error( "PortfolioManager::GetPortfolio, portfolio does not exist" );
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
    Portfolio::TableRowDef& row;
    PortfolioUpdate( Portfolio::TableRowDef& row_, const ou::tf::keytypes::idPortfolio_t& idPortfolio_ )
      : row( row_ ), idPortfolio( idPortfolio_ ) {};
  };
}

void PortfolioManager::UpdatePortfolio( const idPortfolio_t& idPortfolio ) {

  pPortfolio_t p( GetPortfolio( idPortfolio ) );  // has exception if does not exist

  UpdateRecord<idPortfolio_t, Portfolio::TableRowDef, mapPortfolios_t, PortfolioManagerQueries::PortfolioUpdate>(
    idPortfolio, p->GetRow(), m_mapPortfolios, "portfolioid = ?" );

  OnPortfolioUpdated( idPortfolio );

}

namespace PortfolioManagerQueries {
  struct ActivePortfolios {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "active", bActive );
    }
    bool bActive;
    ActivePortfolios( bool bActive_ ) : bActive( bActive_ ) {};
  };
}

void PortfolioManager::UpdateReportingPortfolio( idPortfolio_t idOwner, idPortfolio_t idReporting ) {
  iterReportingPortfolios_t iter = m_mapReportingPortfolios.find( idOwner );
  if ( m_mapReportingPortfolios.end() == iter ) {
    setPortfolioId_t setPortfolioId;
    iter = m_mapReportingPortfolios.insert( m_mapReportingPortfolios.begin(), 
      mapReportingPortfolios_pair_t( idOwner, setPortfolioId ) );
  }
  iter->second.insert( iter->second.begin(), idReporting );
}

void PortfolioManager::LoadActivePortfolios( void ) {

  PortfolioManagerQueries::ActivePortfolios parameter( true );
  ou::db::QueryFields<PortfolioManagerQueries::ActivePortfolios>::pQueryFields_t pQuery
    = m_pSession->SQL<PortfolioManagerQueries::ActivePortfolios>( "select * from portfolios", parameter ).Where( "active=?" ).NoExecute();
  m_pSession->Bind<PortfolioManagerQueries::ActivePortfolios>( pQuery );
  while ( m_pSession->Execute( pQuery ) ) {

    pPortfolio_t pPortfolio;

    // following portfolio / position code is shared with GetPortfolio and could be factored out
    Portfolio::TableRowDef rowPortfolio;
    m_pSession->Columns<PortfolioManagerQueries::ActivePortfolios, Portfolio::TableRowDef>( pQuery, rowPortfolio );
    pPortfolio.reset( new Portfolio( rowPortfolio ) );

    std::pair<mapPortfolios_iter_t, bool> response;
    response = m_mapPortfolios.insert( mapPortfolio_pair_t( rowPortfolio.idPortfolio, structPortfolio( pPortfolio ) ) );
    if ( false == response.second ) {
      // 2013/08/06 need a different way of handling reloads.
      // will need to totally invalidate the cache, this record as well as all records dependent upon this key
      // need a way to compare new / old records to see if there is a problem?^
      // may also need to implement events where records are updated (not too difficult) and deleted (more involved)
      // this issue results when creating the database, and preloading db with records, and then reloading records
      // or high level routines don't call this during the same run as when the db has been created
//      throw std::runtime_error( "LoadActivePortfolios:  couldn't insert portfolio into map" );
    }

    UpdateReportingPortfolio( rowPortfolio.idOwner, rowPortfolio.idPortfolio );

    pPortfolio->OnCommissionUpdate.Add( MakeDelegate( this, &PortfolioManager::HandlePortfolioOnCommission ) );
    pPortfolio->OnExecutionUpdate.Add( MakeDelegate( this, &PortfolioManager::HandlePortfolioOnExecution ) );

    OnPortfolioAdded( rowPortfolio.idPortfolio );

    LoadPositions( rowPortfolio.idPortfolio, response.first->second.mapPosition );

  }
}

void PortfolioManager::LoadPositions( const idPortfolio_t& idPortfolio, mapPosition_t& mapPosition ) {

  PortfolioManagerQueries::PortfolioKey key( idPortfolio );

  ou::db::QueryFields<PortfolioManagerQueries::PortfolioKey>::pQueryFields_t pPositionQuery
    = m_pSession->SQL<PortfolioManagerQueries::PortfolioKey>( "select * from positions", key )
      .Where( "portfolioid = ?" )
      .OrderBy( "positionid" )
      .NoExecute();
  m_pSession->Bind<PortfolioManagerQueries::PortfolioKey>( pPositionQuery );
  while ( m_pSession->Execute( pPositionQuery ) ) {
    Position::TableRowDef rowPosition;
    m_pSession->Columns<PortfolioManagerQueries::PortfolioKey, Position::TableRowDef>( pPositionQuery, rowPosition );
    pPosition_t pPosition( new Position( rowPosition ) );
    if ( 0 == OnPositionNeedsDetails ) {  // fill in instrument, execution, data 
      throw std::runtime_error( "PortfolioManager::LoadPositions has no Details Callback" );
    }
    OnPositionNeedsDetails( pPosition );
    mapPosition.insert( mapPosition_pair_t( rowPosition.sName, pPosition ) );
    pPosition->OnUpdateCommissionForPortfolioManager.Add( MakeDelegate( this, &PortfolioManager::HandlePositionOnCommission ) );
    pPosition->OnUpdateExecutionForPortfolioManager.Add( MakeDelegate( this, &PortfolioManager::HandlePositionOnExecution ) );
    OnPositionAdded( pPosition->GetRow().idPosition );
  }
}

void PortfolioManager::DeletePortfolio( const idPortfolio_t& idPortfolio ) {

  pPortfolio_t p( GetPortfolio( idPortfolio ) );  // has exception if does not exist

  // need to delete position records first
//  DeleteRecord<idPortfolio_t, mapPortfolios_t, PortfolioManagerQueries::PortfolioKey>( 
//    idPortfolio, m_mapPortfolios, "portfolioid = ?" );

  // delete portfolio records
  try {
    DeleteRecord<idPortfolio_t, mapPortfolios_t, PortfolioManagerQueries::PortfolioKey>( 
      idPortfolio, m_mapPortfolios, "portfolioid = ?" );
    OnPortfolioDeleted( idPortfolio );
  }
  catch (...) {
    throw std::runtime_error( "PortfolioManager::DeletePortfolio has dependencies" );
  }

}

//
// Position
//

PortfolioManager::pPosition_t PortfolioManager::ConstructPosition( 
    const idPortfolio_t& idPortfolio, const std::string& sName, const std::string& sAlgorithm,
    const idAccount_t& idExecutionAccount, const idAccount_t& idDataAccount, 
    const pProvider_t& pExecutionProvider, const pProvider_t& pDataProvider,
    pInstrument_cref pInstrument )
{
  pPosition_t pPosition;
  // confirm portfolio exists
  GetPortfolio( idPortfolio );
  mapPortfolios_iter_t iterPortfolio = m_mapPortfolios.find( idPortfolio );
  if ( m_mapPortfolios.end() == iterPortfolio ) {  // should exist as we already just 'got' it
    throw std::runtime_error( "ConstructPosition:  idPortfolio does not exist" );
  }

  if ( "" == sName ) {
    throw std::runtime_error( "ConstructPosition: name is empty" );
  }

  mapPosition_iter_t iterPosition = iterPortfolio->second.mapPosition.find( sName );
  if ( iterPortfolio->second.mapPosition.end() != iterPosition ) {
    throw std::runtime_error( "ConstructPosition:  sName already exists" );
  }

  pPosition.reset( new Position( pInstrument, pExecutionProvider, pDataProvider, idExecutionAccount, idDataAccount, idPortfolio, sName, sAlgorithm ) );
  if ( 0 == m_pSession ) {
    throw std::runtime_error( "ConstructPosition:  database session not available" );
  }

  ou::db::QueryFields<Position::TableRowDefNoKey>::pQueryFields_t pQuery
    = m_pSession->Insert<Position::TableRowDefNoKey>( 
    const_cast<Position::TableRowDefNoKey&>( dynamic_cast<const Position::TableRowDefNoKey&>( pPosition->GetRow() ) ) );
  idPosition_t idPosition( m_pSession->GetLastRowId() );
  pPosition->Set( idPosition );

  pPosition->OnUpdateCommissionForPortfolioManager.Add( MakeDelegate( this, &PortfolioManager::HandlePositionOnCommission ) );
  pPosition->OnUpdateExecutionForPortfolioManager.Add( MakeDelegate( this, &PortfolioManager::HandlePositionOnExecution ) );

  iterPortfolio->second.pPortfolio->AddPosition( sName, pPosition );

  OnPositionAdded( idPosition );

  return pPosition;
}

PortfolioManager::pPosition_t PortfolioManager::GetPosition( const idPortfolio_t& idPortfolio, const std::string& sName ) {

  mapPortfolios_iter_t iterPortfolio = m_mapPortfolios.find( idPortfolio );
  if ( m_mapPortfolios.end() == iterPortfolio ) {
    throw std::runtime_error( "GetPosition:  idPortfolio does not exist" );
  }
  assert( "" != sName );

  mapPosition_iter_t iterPosition = iterPortfolio->second.mapPosition.find( sName );
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
    Position::TableRowDefNoKey& row;
    PositionUpdate( Position::TableRowDefNoKey& row_, const ou::tf::keytypes::idPosition_t& idPosition_ )
      : row( row_ ), idPosition( idPosition_ ) {};
  };
}

void PortfolioManager::UpdatePosition( const idPortfolio_t& idPortfolio, const std::string& sName ) {
  pPosition_t pPosition( GetPosition( idPortfolio, sName ) );
  idPosition_t idPosition( pPosition->GetRow().idPosition );
  UpdateRecord<idPosition_t, Position::TableRowDefNoKey, PortfolioManagerQueries::PositionUpdate>(
    idPosition, dynamic_cast<const Position::TableRowDefNoKey&>( pPosition->GetRow() ), "positionid = ?" );
  OnPositionUpdated( idPosition );
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

void PortfolioManager::DeletePosition( const idPortfolio_t& idPortfolio, const std::string& sName ) {
  pPosition_t pPosition( GetPosition( idPortfolio, sName ) );
  idPosition_t idPosition( pPosition->GetRow().idPosition );
  if ( pPosition->OrdersPending() ) {
    throw std::runtime_error( "PortfolioManager::DeletePosition has orders pending" );
  }
  else {
    mapPortfolios_iter_t iterPortfolio = m_mapPortfolios.find( idPortfolio );  // no error checking as performed in previous step
    mapPosition_iter_t iterPosition = iterPortfolio->second.mapPosition.find( sName ); // no error checking as performed in previous step
    try {
      DeleteRecord<idPosition_t, PortfolioManagerQueries::PositionKey>( pPosition->GetRow().idPosition, "positionid = ?" );
      iterPortfolio->second.mapPosition.erase( iterPosition );
    }
    catch (...) {
      throw std::runtime_error( "PortfolioManager::DeletePosition position has dependencies" );
    }
  }
  OnPositionDeleted( idPosition );
}


//
// Table Management
//

void PortfolioManager::HandleRegisterTables( ou::db::Session& session ) {
  session.RegisterTable<Portfolio::TableCreateDef>( tablenames::sPortfolio );
  session.RegisterTable<Position::TableCreateDef>( tablenames::sPosition );
}

void PortfolioManager::HandleRegisterRows( ou::db::Session& session ) {
  session.MapRowDefToTableName<Portfolio::TableRowDef>( tablenames::sPortfolio );
  session.MapRowDefToTableName<Position::TableRowDef>( tablenames::sPosition );
  session.MapRowDefToTableName<Position::TableRowDefNoKey>( tablenames::sPosition );
}

void PortfolioManager::HandlePopulateTables( ou::db::Session& session ) {
}

// this stuff could probably be rolled into Session with a template
void PortfolioManager::AttachToSession( ou::db::Session* pSession ) {
  ManagerBase::AttachToSession( pSession );
  pSession->OnRegisterTables.Add( MakeDelegate( this, &PortfolioManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Add( MakeDelegate( this, &PortfolioManager::HandleRegisterRows ) );
  pSession->OnPopulate.Add( MakeDelegate( this, &PortfolioManager::HandlePopulateTables ) );
}

void PortfolioManager::DetachFromSession( ou::db::Session* pSession ) {
  pSession->OnRegisterTables.Remove( MakeDelegate( this, &PortfolioManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Remove( MakeDelegate( this, &PortfolioManager::HandleRegisterRows ) );
  pSession->OnPopulate.Remove( MakeDelegate( this, &PortfolioManager::HandlePopulateTables ) );
  ManagerBase::DetachFromSession( pSession );
}

} // namespace tf
} // namespace ou
