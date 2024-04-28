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
  if ( PortfolioExists( idPortfolio ) ) {
    throw std::runtime_error( "PortfolioManager::Create, portfolio already exists" );
  }

  pPortfolio = std::make_shared<ou::tf::Portfolio>( idPortfolio, idAccountOwner, idOwner, ePortfolioType, eCurrency, sDescription );
  m_mapPortfolios.insert( mapPortfolio_pair_t( idPortfolio, pPortfolio ) );
  if ( nullptr != m_pSession ) {
    ou::db::QueryFields<Portfolio::TableRowDef>::pQueryFields_t pQuery
      = m_pSession->Insert<Portfolio::TableRowDef>( const_cast<Portfolio::TableRowDef&>( pPortfolio->GetRow() ) );
  }

  PortfolioCommon( pPortfolio );

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
      ou::db::Field( a, "unrealizedpl", dblUnRealizedPL );
      ou::db::Field( a, "realizedpl", dblRealizedPL );
      ou::db::Field( a, "positionid", idPosition );
    }
    const ou::tf::keytypes::idPosition_t idPosition;
    OrderSide::EOrderSide eOrderSidePending;
    boost::uint32_t nPositionPending;
    OrderSide::EOrderSide eOrderSideActive;
    boost::uint32_t nPositionActive;
    double dblConstructedValue;
    double dblUnRealizedPL;
    double dblRealizedPL;
    UpdatePositionData(
      const ou::tf::keytypes::idPosition_t idPosition_,
      OrderSide::EOrderSide eOrderSidePending_, boost::uint32_t nPositionPending_,
      OrderSide::EOrderSide eOrderSideActive_,  boost::uint32_t nPositionActive_,
      double dblConstructedValue_,
      double dblUnRealizedPL_, double dblRealizedPL_ )
      : idPosition( idPosition_ ),
        eOrderSidePending( eOrderSidePending_ ), nPositionPending( nPositionPending_ ),
        eOrderSideActive( eOrderSideActive_ ), nPositionActive( nPositionActive_ ),
        dblConstructedValue( dblConstructedValue_ ),
        dblUnRealizedPL( dblUnRealizedPL_ ), dblRealizedPL( dblRealizedPL_ ) {};
  };
}

void PortfolioManager::HandlePositionOnExecution( const Position& position ) {
  if ( nullptr != m_pSession ) {
    const Position::TableRowDef& row( position.GetRow() );
    PortfolioManagerQueries::UpdatePositionData update( row.idPosition, row.eOrderSidePending, row.nPositionPending,
      row.eOrderSideActive, row.nPositionActive, row.dblConstructedValue, row.dblUnRealizedPL, row.dblRealizedPL );
    ou::db::QueryFields<PortfolioManagerQueries::UpdatePositionData>::pQueryFields_t pQuery
      = m_pSession->SQL<PortfolioManagerQueries::UpdatePositionData>(
        "update positions set ordersidepending=?, quantitypending=?, ordersideactive=?, quantityactive=?, constructedvalue=?, unrealizedpl=?, realizedpl=?", update ).Where( "positionid=?" );
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
  if ( nullptr != m_pSession ) {
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
  if ( nullptr != m_pSession ) {
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
  if ( nullptr != m_pSession ) {
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
      pPortfolio = std::make_shared<ou::tf::Portfolio>( rowPortfolio );

      std::pair<mapPortfolios_iter_t, bool> response;
      response = m_mapPortfolios.insert( mapPortfolio_pair_t( idPortfolio, structPortfolio( pPortfolio ) ) );
      if ( false == response.second ) {
        throw std::runtime_error( "GetPortfolio:  couldn't insert portfolio into map" );
      }

      PortfolioCommon( pPortfolio );

      LoadPositions( idPortfolio, response.first->second.mapPosition );

    }
    else {
      throw std::runtime_error( "PortfolioManager::GetPortfolio, portfolio does not exist" );
    }
  }

  return pPortfolio;
}

//////

namespace PortfolioManagerQueries {
  struct UpdatePortfolioActive {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "active", bActive );
      ou::db::Field( a, "portfolioid", idPortfolio );
    }
    const ou::tf::keytypes::idPortfolio_t idPortfolio;
    bool bActive;
    UpdatePortfolioActive( const ou::tf::keytypes::idPortfolio_t idPortfolio_, bool bActive_ )
      : idPortfolio( idPortfolio_ ), bActive( bActive_ ) {};
  };
}

void PortfolioManager::PortfolioUpdateActive( pPortfolio_t pPortfolio ) {
  if ( nullptr != m_pSession ) {
    const Portfolio::TableRowDef& row( pPortfolio->GetRow() );
    PortfolioManagerQueries::UpdatePortfolioActive update( row.idPortfolio, row.bActive );
    ou::db::QueryFields<PortfolioManagerQueries::UpdatePortfolioActive>::pQueryFields_t pQuery
      = m_pSession->SQL<PortfolioManagerQueries::UpdatePortfolioActive>( "update portfolios set active=?", update ).Where( "portfolioid=?" );
  }
}

//////

namespace PortfolioManagerQueries {
  struct UpdatePositionNotes {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "notes", sNotes );
      ou::db::Field( a, "positionid", idPosition );
    }
    const ou::tf::keytypes::idPosition_t idPosition;
    std::string sNotes;
    UpdatePositionNotes( const ou::tf::keytypes::idPosition_t idPosition_, const std::string& sNotes_ )
      : idPosition( idPosition_ ), sNotes( sNotes_ ) {};
  };
}

void PortfolioManager::PositionUpdateNotes( pPosition_t pPosition ) {
  if ( nullptr != m_pSession ) {
    const Position::TableRowDef& row( pPosition->GetRow() );
    PortfolioManagerQueries::UpdatePositionNotes update( row.idPosition, row.sNotes );
    ou::db::QueryFields<PortfolioManagerQueries::UpdatePositionNotes>::pQueryFields_t pQuery
      = m_pSession->SQL<PortfolioManagerQueries::UpdatePositionNotes>( "update positions set notes=?", update ).Where( "positionid=?" );
  }
}  // the Where could be appended with boost::fusion type structure for the fields, and bind?
  // need to cache the queries

//////

bool PortfolioManager::PortfolioExists( const idPortfolio_t& idPortfolio ) {

  assert( "" != idPortfolio );  // todo:  add this check in other handlers

  bool bExists( false );
  pPortfolio_t pPortfolio;
  mapPortfolios_iter_t iter = m_mapPortfolios.find( idPortfolio );
  if ( m_mapPortfolios.end() != iter ) {
    bExists = true;
  }
  else {
    // following portfolio / position code is shared with LoadActivePortfolios and could be factored out
    PortfolioManagerQueries::PortfolioKey key( idPortfolio );
    ou::db::QueryFields<PortfolioManagerQueries::PortfolioKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
      = m_pSession->SQL<PortfolioManagerQueries::PortfolioKey>( "select portfolioid from portfolios", key ).Where( "portfolioid = ?" ).NoExecute();
    m_pSession->Bind<PortfolioManagerQueries::PortfolioKey>( pExistsQuery );
    if ( m_pSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
      bExists = true;
    }
  }
  return bExists;
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

  // NOTE: fix, this fails as there is no table registered as PortfolioManagerQueries::PortfolioUpdate
  UpdateRecord<idPortfolio_t, Portfolio::TableRowDef, mapPortfolios_t, PortfolioManagerQueries::PortfolioUpdate>(
    idPortfolio, p->GetRow(), m_mapPortfolios, "portfolioid = ?" );

  OnPortfolioUpdated( p );

}

void PortfolioManager::UpdateReportingPortfolio( idPortfolio_t idOwner, idPortfolio_t idReporting ) {
  iterReportingPortfolios_t iter = m_mapReportingPortfolios.find( idOwner );
  if ( m_mapReportingPortfolios.end() == iter ) {
    setPortfolioId_t setPortfolioId;
    iter = m_mapReportingPortfolios.insert( m_mapReportingPortfolios.begin(),
      mapReportingPortfolios_pair_t( idOwner, setPortfolioId ) );
  }
  // add idReporting to idOwner as a reporting portfolio.
  iter->second.insert( iter->second.begin(), idReporting );
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

void PortfolioManager::LoadActivePortfolios() {
  // todo:  work with sub-portfolios, and get them attached properly

  PortfolioManagerQueries::ActivePortfolios parameter( true );
  ou::db::QueryFields<PortfolioManagerQueries::ActivePortfolios>::pQueryFields_t pQuery
    = m_pSession->SQL<PortfolioManagerQueries::ActivePortfolios>( "select * from portfolios", parameter ).Where( "active=?" ).NoExecute();
  m_pSession->Bind<PortfolioManagerQueries::ActivePortfolios>( pQuery );
  while ( m_pSession->Execute( pQuery ) ) {

    pPortfolio_t pPortfolio;

    // following portfolio / position code is shared with GetPortfolio and could be factored out
    Portfolio::TableRowDef rowPortfolio;
    m_pSession->Columns<PortfolioManagerQueries::ActivePortfolios, Portfolio::TableRowDef>( pQuery, rowPortfolio );
    pPortfolio = std::make_shared<ou::tf::Portfolio>( rowPortfolio );

    std::pair<mapPortfolios_iter_t, bool> response;
    response = m_mapPortfolios.insert( mapPortfolio_pair_t( rowPortfolio.idPortfolio, structPortfolio( pPortfolio ) ) );
    if ( false == response.second ) {
      // 2013/08/06 need a different way of handling reloads.
      // will need to totally invalidate the cache, this record as well as all records dependent upon this key
      // need a way to compare new / old records to see if there is a problem?^
      // may also need to implement events where records are updated (not too difficult) and deleted (more involved)
      // this issue results when creating the database, and preloading db with records, and then reloading records
      // or high level routines don't call this during the same run as when the db has been created
      // 2013/11/28 all active portfolios are kept in memory.  database is persistent repository across
      //  restarts.  In memory structures neeed to be kept in sync with database structures.
//      throw std::runtime_error( "LoadActivePortfolios:  couldn't insert portfolio into map" );
    }

//    UpdateReportingPortfolio( rowPortfolio.idOwner, rowPortfolio.idPortfolio );
//    if ( "" != rowPortfolio.idOwner ) {
//      GetPortfolio( rowPortfolio.idOwner )->AddSubPortfolio( pPortfolio );
//    }

    PortfolioCommon( pPortfolio );

    LoadPositions( rowPortfolio.idPortfolio, response.first->second.mapPosition );

  }
}

void PortfolioManager::PortfolioCommon( pPortfolio_t& pPortfolio ) {

  const Portfolio::TableRowDef& row( pPortfolio->GetRow() );
  UpdateReportingPortfolio( row.idOwner, row.idPortfolio );
  if ( "" != row.idOwner ) {
    GetPortfolio( row.idOwner )->AddSubPortfolio( pPortfolio );
  }

  pPortfolio->OnCommissionUpdate.Add( MakeDelegate( this, &PortfolioManager::HandlePortfolioOnCommission ) );
  pPortfolio->OnExecutionUpdate.Add( MakeDelegate( this, &PortfolioManager::HandlePortfolioOnExecution ) );

  //OnPortfolioLoaded( rowPortfolio.idPortfolio );
  OnPortfolioLoaded( pPortfolio );

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
    pPosition_t pPosition = std::make_shared<Position>( rowPosition );
    if ( nullptr == OnPositionNeedsDetails ) {  // fill in instrument, execution, data
      throw std::runtime_error( "PortfolioManager::LoadPositions has no Details Callback" );
    }
    OnPositionNeedsDetails( pPosition );
    mapPosition.insert( mapPosition_pair_t( rowPosition.sName, pPosition ) );

    this->GetPortfolio( idPortfolio )->AddPosition( pPosition->GetInstrument()->GetInstrumentName(), pPosition );

    pPosition->OnUpdateCommissionForPortfolioManager.Add( MakeDelegate( this, &PortfolioManager::HandlePositionOnCommission ) );
    pPosition->OnUpdateExecutionForPortfolioManager.Add( MakeDelegate( this, &PortfolioManager::HandlePositionOnExecution ) );
    //OnPositionLoaded( pPosition->GetRow().idPosition );
    OnPositionLoaded( pPosition );
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

PortfolioManager::pPosition_t PortfolioManager::ConstructPosition( // old mechanism
    const idPortfolio_t& idPortfolio, const std::string& sNamePosition,
    const std::string& sAlgorithm,
    const idAccount_t& idExecutionAccount, const idAccount_t& idDataAccount,
    const pProvider_t& pExecutionProvider, const pProvider_t& pDataProvider,
    pInstrument_t pInstrument )
{
  pPosition_t pPosition;

  ConstructPosition(
    idPortfolio, sNamePosition,
    [&,pInstrument,pExecutionProvider, pDataProvider]()mutable->pPosition_t{
      pPosition = std::make_shared<ou::tf::Position>( pInstrument, pExecutionProvider, pDataProvider, idExecutionAccount, idDataAccount, idPortfolio, sNamePosition, sAlgorithm );
      return pPosition;
    } );

  return pPosition;
}

PortfolioManager::pPosition_t PortfolioManager::ConstructPosition(
    const idPortfolio_t& idPortfolio, const std::string& sNamePosition, const std::string& sAlgorithm,
    const idAccount_t& idExecutionAccount, const idAccount_t& idDataAccount,
    const pProvider_t& pExecutionProvider,
    pWatch_t pWatch )
{
  pPosition_t pPosition;

  ConstructPosition(
    idPortfolio, sNamePosition,
    [&,pWatch,pExecutionProvider]()->pPosition_t{
      pPosition = std::make_shared<ou::tf::Position>( pWatch, pExecutionProvider, idExecutionAccount, idDataAccount, idPortfolio, sNamePosition, sAlgorithm );
      return pPosition;
    } );

  return pPosition;
}

PortfolioManager::pPosition_t PortfolioManager::ConstructPosition( // new mechanism
  const idPortfolio_t& idPortfolio, const std::string& sNamePosition
, const std::string& sAlgorithm
, const pProvider_t& pExecutionProvider, pWatch_t pWatch
)
{
  pPosition_t pPosition;

  ConstructPosition(
    idPortfolio, sNamePosition,
    [&,pWatch,pExecutionProvider]()->pPosition_t{
      pPosition = std::make_shared<ou::tf::Position>( pWatch, pExecutionProvider, idPortfolio, sNamePosition, sAlgorithm );
      return pPosition;
    } );

  return pPosition;
}

void PortfolioManager::ConstructPosition( // re-factored code
    const idPortfolio_t& idPortfolio, const std::string& sNamePosition,
    fConstructPosition_t&& fConstructPosition
  )
{
  // confirm portfolio exists
  GetPortfolio( idPortfolio );
  mapPortfolios_iter_t iterPortfolio = m_mapPortfolios.find( idPortfolio );
  if ( m_mapPortfolios.end() == iterPortfolio ) {  // should exist as we already just 'got' it
    throw std::runtime_error( "ConstructPosition: portfolio " + idPortfolio + " does not exist" );
  }

  if ( sNamePosition.empty() ) {
    throw std::runtime_error( "ConstructPosition: position name is empty" );
  }

  auto& [id,portfolio] = *iterPortfolio;

  mapPosition_iter_t iterPosition = portfolio.mapPosition.find( sNamePosition );
  if ( portfolio.mapPosition.end() != iterPosition ) {
    throw std::runtime_error( "ConstructPosition:  position " + sNamePosition + " exists" );
  }

  pPosition_t pPosition;
  pPosition = fConstructPosition();

  portfolio.mapPosition.insert( mapPosition_pair_t( sNamePosition, pPosition ) );

  if ( nullptr == m_pSession ) {
    throw std::runtime_error( "ConstructPosition:  database session not available" );
  }

  ou::db::QueryFields<Position::TableRowDefNoKey>::pQueryFields_t pQuery
    = m_pSession->Insert<Position::TableRowDefNoKey>(
    const_cast<Position::TableRowDefNoKey&>( dynamic_cast<const Position::TableRowDefNoKey&>( pPosition->GetRow() ) ) );
  idPosition_t idPosition( m_pSession->GetLastRowId() );
  pPosition->Set( idPosition );

  pPosition->OnUpdateCommissionForPortfolioManager.Add( MakeDelegate( this, &PortfolioManager::HandlePositionOnCommission ) );
  pPosition->OnUpdateExecutionForPortfolioManager.Add( MakeDelegate( this, &PortfolioManager::HandlePositionOnExecution ) );

  portfolio.pPortfolio->AddPosition( sNamePosition, pPosition );

  OnPositionAdded( pPosition );

}

bool PortfolioManager::PositionExists( const idPortfolio_t& idPortfolio, const std::string& sNamePosition ) {

  assert( 0 != idPortfolio.size() );

  mapPortfolios_iter_t iterPortfolio = m_mapPortfolios.find( idPortfolio );
  if ( m_mapPortfolios.end() == iterPortfolio ) {
    return false;
  }

  assert( 0 != sNamePosition.size() );

  auto& [id,portfolio] = *iterPortfolio;

  mapPosition_iter_t iterPosition = portfolio.mapPosition.find( sNamePosition );
  if ( portfolio.mapPosition.end() == iterPosition ) {
    return false;
  }

  return true;
}

PortfolioManager::pPosition_t PortfolioManager::GetPosition( const idPortfolio_t& idPortfolio, const std::string& sNamePosition ) {

  assert( 0 != idPortfolio.size() );

  mapPortfolios_iter_t iterPortfolio = m_mapPortfolios.find( idPortfolio );
  if ( m_mapPortfolios.end() == iterPortfolio ) {
    throw std::runtime_error( "GetPosition: portfolio " + idPortfolio + " does not exist" );
  }

  assert( !sNamePosition.empty() );

  auto& [id,portfolio] = *iterPortfolio;

  mapPosition_iter_t iterPosition = portfolio.mapPosition.find( sNamePosition );
  if ( portfolio.mapPosition.end() == iterPosition ) {
    const std::string sError( "GetPosition: " + sNamePosition + " does not exist" );
    throw std::runtime_error( sError );
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

// TODO: this may not be functional, has runtime 'type not found' error
void PortfolioManager::UpdatePosition( const idPortfolio_t& idPortfolio, const std::string& sName ) {
  pPosition_t pPosition( GetPosition( idPortfolio, sName ) );
  idPosition_t idPosition( pPosition->GetRow().idPosition );
  UpdateRecord<idPosition_t, Position::TableRowDefNoKey, PortfolioManagerQueries::PositionUpdate>(
    idPosition, dynamic_cast<const Position::TableRowDefNoKey&>( pPosition->GetRow() ), "positionid = ?" );
  //OnPositionUpdated( idPosition );
  OnPositionUpdated( pPosition );
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
  // todo:  this should come before client stuff
}

void PortfolioManager::HandleLoadTables( ou::db::Session& session ) {
  // todo:  this should come before client stuff
}

// this stuff could probably be rolled into Session with a template
void PortfolioManager::AttachToSession( ou::db::Session* pSession ) {
  ManagerBase::AttachToSession( pSession );
  pSession->OnRegisterTables.Add( MakeDelegate( this, &PortfolioManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Add( MakeDelegate( this, &PortfolioManager::HandleRegisterRows ) );
  pSession->OnPopulate.Add( MakeDelegate( this, &PortfolioManager::HandlePopulateTables ) );
  pSession->OnLoad.Add( MakeDelegate( this, &PortfolioManager::HandleLoadTables ) );
}

void PortfolioManager::DetachFromSession( ou::db::Session* pSession ) {
  pSession->OnRegisterTables.Remove( MakeDelegate( this, &PortfolioManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Remove( MakeDelegate( this, &PortfolioManager::HandleRegisterRows ) );
  pSession->OnPopulate.Remove( MakeDelegate( this, &PortfolioManager::HandlePopulateTables ) );
  pSession->OnLoad.Remove( MakeDelegate( this, &PortfolioManager::HandleLoadTables ) );
  ManagerBase::DetachFromSession( pSession );
}

} // namespace tf
} // namespace ou
