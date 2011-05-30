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
    if ( 0 != m_pSession ) {
      ou::db::QueryFields<CPortfolio::TableRowDef>::pQueryFields_t pQuery
        = m_pSession->Insert<CPortfolio::TableRowDef>( const_cast<CPortfolio::TableRowDef&>( pPortfolio->GetRow() ) );
    }
    pPortfolio->OnCommission.Add( MakeDelegate( this, &CPortfolioManager::HandlePortfolioOnCommission ) );
    pPortfolio->OnExecution.Add( MakeDelegate( this, &CPortfolioManager::HandlePortfolioOnExecution ) );
  }
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

void CPortfolioManager::HandlePositionOnExecution( execution_delegate_t exec ) {
  if ( 0 != m_pSession ) {
    const CPosition::TableRowDef& row( exec.first.GetRow() );
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

void CPortfolioManager::HandlePositionOnCommission( const CPosition* pPosition ) {
  if ( 0 != m_pSession ) {
    const CPosition::TableRowDef& row( pPosition->GetRow() );
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

void CPortfolioManager::HandlePortfolioOnExecution( const CPortfolio* pPortfolio ) {
  if ( 0 != m_pSession ) {
    const CPortfolio::TableRowDef& row( pPortfolio->GetRow() );
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

void CPortfolioManager::HandlePortfolioOnCommission( const CPortfolio* pPortfolio ) {
  if ( 0 != m_pSession ) {
    const CPortfolio::TableRowDef& row( pPortfolio->GetRow() );
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

CPortfolioManager::pPortfolio_t CPortfolioManager::GetPortfolio( const idPortfolio_t& idPortfolio ) {

  assert( "" != idPortfolio );  // todo:  add this check in other handlers

  pPortfolio_t pPortfolio;
  iterPortfolio_t iter = m_mapPortfolio.find( idPortfolio );
  if ( m_mapPortfolio.end() != iter ) {
    pPortfolio = iter->second.pPortfolio;
  }
  else {
    // following portfolio / position code is shared with LoadActivePortfolios and could be factored out
    PortfolioManagerQueries::PortfolioKey key( idPortfolio );
    ou::db::QueryFields<PortfolioManagerQueries::PortfolioKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
      = m_pSession->SQL<PortfolioManagerQueries::PortfolioKey>( "select * from portfolios", key ).Where( "portfolioid = ?" ).NoExecute();
    m_pSession->Bind<PortfolioManagerQueries::PortfolioKey>( pExistsQuery );
    if ( m_pSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
      CPortfolio::TableRowDef rowPortfolio;
      m_pSession->Columns<PortfolioManagerQueries::PortfolioKey, CPortfolio::TableRowDef>( pExistsQuery, rowPortfolio );
      pPortfolio.reset( new CPortfolio( rowPortfolio ) );

      std::pair<iterPortfolio_t, bool> response;
      response = m_mapPortfolio.insert( mapPortfolio_pair_t( idPortfolio, structPortfolio( pPortfolio ) ) );
      if ( false == response.second ) {
        throw std::runtime_error( "GetPortfolio:  couldn't insert portfolio into map" );
      }
      pPortfolio->OnCommission.Add( MakeDelegate( this, &CPortfolioManager::HandlePortfolioOnCommission ) );
      pPortfolio->OnExecution.Add( MakeDelegate( this, &CPortfolioManager::HandlePortfolioOnExecution ) );

      LoadPositions( idPortfolio, response.first->second.mapPosition );

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

void CPortfolioManager::LoadActivePortfolios( void ) {

  PortfolioManagerQueries::ActivePortfolios parameter( true );
  ou::db::QueryFields<PortfolioManagerQueries::ActivePortfolios>::pQueryFields_t pQuery
    = m_pSession->SQL<PortfolioManagerQueries::ActivePortfolios>( "select * from portfolios", parameter ).Where( "active=?" ).NoExecute();
  m_pSession->Bind<PortfolioManagerQueries::ActivePortfolios>( pQuery );
  while ( m_pSession->Execute( pQuery ) ) {
    pPortfolio_t pPortfolio;

    // following portfolio / position code is shared with GetPortfolio and could be factored out
    CPortfolio::TableRowDef rowPortfolio;
    m_pSession->Columns<PortfolioManagerQueries::ActivePortfolios, CPortfolio::TableRowDef>( pQuery, rowPortfolio );
    pPortfolio.reset( new CPortfolio( rowPortfolio ) );

    std::pair<iterPortfolio_t, bool> response;
    response = m_mapPortfolio.insert( mapPortfolio_pair_t( rowPortfolio.idPortfolio, structPortfolio( pPortfolio ) ) );
    if ( false == response.second ) {
      throw std::runtime_error( "LoadActivePortfolios:  couldn't insert portfolio into map" );
    }
    pPortfolio->OnCommission.Add( MakeDelegate( this, &CPortfolioManager::HandlePortfolioOnCommission ) );
    pPortfolio->OnExecution.Add( MakeDelegate( this, &CPortfolioManager::HandlePortfolioOnExecution ) );

    LoadPositions( rowPortfolio.idPortfolio, response.first->second.mapPosition );

  }
}

void CPortfolioManager::LoadPositions( const idPortfolio_t& idPortfolio, mapPosition_t& mapPosition ) {

  PortfolioManagerQueries::PortfolioKey key( idPortfolio );

  ou::db::QueryFields<PortfolioManagerQueries::PortfolioKey>::pQueryFields_t pPositionQuery
    = m_pSession->SQL<PortfolioManagerQueries::PortfolioKey>( "select * from positions", key ).Where( "portfolioid = ?" ).NoExecute();
  m_pSession->Bind<PortfolioManagerQueries::PortfolioKey>( pPositionQuery );
  while ( m_pSession->Execute( pPositionQuery ) ) {
    CPosition::TableRowDef rowPosition;
    m_pSession->Columns<PortfolioManagerQueries::PortfolioKey, CPosition::TableRowDef>( pPositionQuery, rowPosition );
    pPosition_t pPosition( new CPosition( rowPosition ) );
    if ( 0 == OnPositionNeedsDetails ) {  // fill in instrument, execution, data 
      throw std::runtime_error( "CPortfolioManager::LoadPositions has no Details Callback" );
    }
    OnPositionNeedsDetails( pPosition );
    mapPosition.insert( mapPosition_pair_t( rowPosition.sName, pPosition ) );
    pPosition->OnCommission.Add( MakeDelegate( this, &CPortfolioManager::HandlePositionOnCommission ) );
    pPosition->OnExecution.Add( MakeDelegate( this, &CPortfolioManager::HandlePositionOnExecution ) );
  }
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
  if ( 0 == m_pSession ) {
    throw std::runtime_error( "ConstructPosition:  database session not available" );
  }

  ou::db::QueryFields<CPosition::TableRowDefNoKey>::pQueryFields_t pQuery
    = m_pSession->Insert<CPosition::TableRowDefNoKey>( 
    const_cast<CPosition::TableRowDefNoKey&>( dynamic_cast<const CPosition::TableRowDefNoKey&>( pPosition->GetRow() ) ) );
  idPosition_t idPosition( m_pSession->GetLastRowId() );
  pPosition->Set( idPosition );

  pPosition->OnCommission.Add( MakeDelegate( this, &CPortfolioManager::HandlePositionOnCommission ) );
  pPosition->OnExecution.Add( MakeDelegate( this, &CPortfolioManager::HandlePositionOnExecution ) );

  iterPortfolio->second.pPortfolio->AddPosition( sName, pPosition );

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

void CPortfolioManager::HandleRegisterTables( ou::db::CSession& session ) {
  session.RegisterTable<CPortfolio::TableCreateDef>( tablenames::sPortfolio );
  session.RegisterTable<CPosition::TableCreateDef>( tablenames::sPosition );
}

void CPortfolioManager::HandleRegisterRows( ou::db::CSession& session ) {
  session.MapRowDefToTableName<CPortfolio::TableRowDef>( tablenames::sPortfolio );
  session.MapRowDefToTableName<CPosition::TableRowDef>( tablenames::sPosition );
  session.MapRowDefToTableName<CPosition::TableRowDefNoKey>( tablenames::sPosition );
}

void CPortfolioManager::HandlePopulateTables( ou::db::CSession& session ) {
}

// this stuff could probably be rolled into CSession with a template
void CPortfolioManager::AttachToSession( ou::db::CSession* pSession ) {
  ManagerBase::AttachToSession( pSession );
  pSession->OnRegisterTables.Add( MakeDelegate( this, &CPortfolioManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Add( MakeDelegate( this, &CPortfolioManager::HandleRegisterRows ) );
  pSession->OnPopulate.Add( MakeDelegate( this, &CPortfolioManager::HandlePopulateTables ) );

}

void CPortfolioManager::DetachFromSession( ou::db::CSession* pSession ) {
  pSession->OnRegisterTables.Remove( MakeDelegate( this, &CPortfolioManager::HandleRegisterTables ) );
  pSession->OnRegisterRows.Remove( MakeDelegate( this, &CPortfolioManager::HandleRegisterRows ) );
  pSession->OnPopulate.Remove( MakeDelegate( this, &CPortfolioManager::HandlePopulateTables ) );
  ManagerBase::DetachFromSession( pSession );
}

} // namespace tf
} // namespace ou
