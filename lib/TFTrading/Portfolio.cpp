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

#include <sstream>
#include <stdexcept>

#include "Portfolio.h"

// 2013/07/25
// * position currency must match portfolio currency
// * currency ratio between from sub-portfolio and portfolio needs to be maintained
// * top level portfolio is always raio 1
// * first level of sub-portfolios can be alternate currency, and have non one ratio
// * also portfolio type as 'multi-legged position'
// * no further level sub-portfolios can have different currency
// * so.. master portfolio, sub-currency portfolio, regular portfolio

namespace ou { // One Unified
namespace tf { // TradeFrame


//Portfolio::Portfolio( // in memory
//  const idPortfolio_t& idPortfolio, EPortfolioType ePortfolioType, currency_t sCurrency, const std::string& sDescription ) 
//: m_row( idPortfolio, "", "", ePortfolioType, sCurrency, sDescription ),
//  m_bCanUseDb( false )
//{
//}

//Portfolio::Portfolio( // master portfolio currency record
//  const idPortfolio_t& idPortfolio, 
//  const idAccountOwner_t& idAccountOwner, currency_t sCurrency,
//  const std::string& sDescription ) 
//: m_row( idPortfolio, idAccountOwner, "", Master, sCurrency, sDescription ), 
//  m_bCanUseDb( true )
//{
//}

Portfolio::Portfolio( // portfolio record
  const idPortfolio_t& idPortfolio, const idAccountOwner_t& idAccountOwner, const idPortfolio_t& idOwner,
   EPortfolioType ePortfolioType, currency_t sCurrency, const std::string& sDescription ) 
: m_row( idPortfolio, idAccountOwner, idOwner, ePortfolioType, sCurrency, sDescription ),
  m_bCanUseDb( true )
{
  bool bOk = true;
  if ( "" == idPortfolio ) bOk = false;
  if ( ( Master == ePortfolioType ) && ( "" != idOwner ) ) bOk = false;
  if ( ( "" != idAccountOwner ) && ( Master != ePortfolioType ) && ( "" == idOwner ) ) bOk = false;
  if ( !bOk ) {
    throw std::runtime_error( "Portfolio::Portfolio parameter problems" );
  }
}

Portfolio::Portfolio( const TableRowDef& row ) 
  : m_row( row ), m_bCanUseDb( true )
{
  m_plCurrent.dblCommissionsPaid = m_row.dblCommissionsPaid;
  m_plCurrent.dblRealized = m_row.dblRealizedPL;
  m_plCurrent.Sum();
  m_plMax = m_plMin = m_plCurrent;
}

Portfolio::~Portfolio(void) {
}

Portfolio::pPosition_t Portfolio::AddPosition( const std::string &sName, pPosition_t pPosition ) {

  // prepare to add position to user named map
  mapPositions_iter_t iterUser = m_mapPositionsViaUserName.find( sName );
  if ( m_mapPositionsViaUserName.end() != iterUser ) {
    throw std::runtime_error( "Portfolio::Add1 position already exists" );
  }

  // prepare to add position to instrument named map
  // instrument can only have one position associated with it
  const std::string& sInstrumentName( pPosition->GetInstrument()->GetInstrumentName() );
  mapPositions_iter_t iterInst = m_mapPositionsViaInstrumentName.find( sInstrumentName );
  if ( m_mapPositionsViaInstrumentName.end() != iterInst ) {
    throw std::runtime_error( "Portfolio::Add2 position already exists" );
  }

  m_mapPositionsViaUserName.insert( mapPositions_pair_t( sName, pPosition ) );
  m_mapPositionsViaInstrumentName.insert( mapPositions_pair_t( sInstrumentName, pPosition ) );

  pPosition->OnUnRealizedPL.Add( MakeDelegate( this, &Portfolio::HandleUnRealizedPL ) );
  pPosition->OnExecution.Add( MakeDelegate( this, &Portfolio::HandleExecution ) );
  pPosition->OnCommission.Add( MakeDelegate( this, &Portfolio::HandleCommission ) );

  return pPosition;
}

void Portfolio::DeletePosition( const std::string& sName ) {

  mapPositions_iter_t iterUser = m_mapPositionsViaUserName.find( sName );
  if ( m_mapPositionsViaUserName.end() == iterUser ) {
    throw std::runtime_error( "Portfolio::Delete1 position does not exist" );
  }

  const std::string& sInstrumentName( iterUser->second->GetInstrument()->GetInstrumentName() );
  mapPositions_iter_t iterInst = m_mapPositionsViaInstrumentName.find( sInstrumentName );
  if ( m_mapPositionsViaInstrumentName.end() == iterInst ) {
    throw std::runtime_error( "Portfolio::Delete2 position does not exist" );
  }

  iterUser->second->OnCommission.Remove( MakeDelegate( this, &Portfolio::HandleCommission ) );
  iterUser->second->OnExecution.Remove( MakeDelegate( this, &Portfolio::HandleExecution ) );
  iterUser->second->OnUnRealizedPL.Remove( MakeDelegate( this, &Portfolio::HandleUnRealizedPL ) );

  m_mapPositionsViaUserName.erase( iterUser );
  m_mapPositionsViaInstrumentName.erase( iterInst );

}

void Portfolio::RenamePosition( const std::string& sOld, const std::string& sNew ) {

  mapPositions_iter_t iter;

  iter = m_mapPositionsViaUserName.find( sNew );
  if ( m_mapPositionsViaUserName.end() == iter ) {
    throw std::runtime_error( "Portfolio::Rename New position already exists" );
  }
  iter = m_mapPositionsViaUserName.find( sOld );
  if ( m_mapPositionsViaUserName.end() == iter ) {
    throw std::runtime_error( "Portfolio::Rename Old position does not exist" );
  }

  pPosition_t pPosition( iter->second );
  m_mapPositionsViaUserName.erase( iter );
  m_mapPositionsViaUserName.insert( mapPositions_pair_t( sNew, pPosition ) );
}

Portfolio::pPosition_t Portfolio::GetPosition( const std::string& sName ) {

  mapPositions_iter_t iter = m_mapPositionsViaUserName.find( sName );

  if ( m_mapPositionsViaUserName.end() == iter ) {
    throw std::runtime_error( "Portfolio::GetPosition position does not exist" );
  }

  return iter->second;
}

// not used
void Portfolio::ReCalc( void ) {

  m_plCurrent.Zero();

  for ( mapPositions_iter_t iter = m_mapPositionsViaUserName.begin(); iter != m_mapPositionsViaUserName.end(); ++iter ) {
    m_plCurrent.dblUnRealized += iter->second->GetUnRealizedPL();
    m_plCurrent.dblRealized += iter->second->GetRealizedPL();
    m_plCurrent.dblCommissionsPaid += iter->second->GetCommissionPaid();
  }
  m_plCurrent.Sum();
  if ( m_plCurrent > m_plMax ) m_plMax = m_plCurrent;
  if ( m_plCurrent < m_plMin ) m_plMin = m_plCurrent;

  m_row.dblRealizedPL = m_plCurrent.dblRealized;
}

void Portfolio::AddSubPortfolio( pPortfolio_t& pPortfolio ) {

  if ( Master == pPortfolio->GetRow().ePortfolioType ) {
    //throw std::runtime_error( "Portfolio::AddSubPortfolio: sub-portfolio cannot be a master portfolio" );
    std::cout << "Master portfolio found" << std::endl;
  }
  else {
    if ( Master == m_row.ePortfolioType ) {
      // if this portfolio is master, no problem
    }
    else { 
      // if sub portfolio is currency summary portfolio, and this isn't master, then this needs to be master portfolio
      if ( CurrencySummary == pPortfolio->GetRow().ePortfolioType ) {
        throw std::runtime_error( "Portfolio::AddSubPortfolio: alternate currency portfolio only attachable to master portfolio" );
      }
    }

    const idPortfolio_t& idSubPortfolio( pPortfolio->GetRow().idPortfolio );
    mapPortfolios_iter_t iter = m_mapSubPortfolios.find( idSubPortfolio );

    if ( m_mapSubPortfolios.end() != iter ) {
      throw std::runtime_error( "Portfolio::AddSubPortfolio portfolio already exists: " + idSubPortfolio );
    }

    m_mapSubPortfolios[ idSubPortfolio ] = pPortfolio;

    pPortfolio->OnCommission.Add( MakeDelegate( this, &Portfolio::HandleCommission ) );
    pPortfolio->OnExecution.Add( MakeDelegate( this, &Portfolio::HandleExecution ) );
    pPortfolio->OnUnRealizedPL.Add( MakeDelegate( this, &Portfolio::HandleUnRealizedPL ) );
  }
}

void Portfolio::RemoveSubPortfolio( const idPortfolio_t& idPortfolio ) {

  mapPortfolios_iter_t iter = m_mapSubPortfolios.find( idPortfolio );

  if ( m_mapSubPortfolios.end() != iter ) {
    throw std::runtime_error( "Portfolio::RemoveSubPortfolio portfolio does not exist: " + idPortfolio );
  }

  Portfolio* pPortfolio = iter->second.get();

  pPortfolio->OnCommission.Add( MakeDelegate( this, &Portfolio::HandleCommission ) );
  pPortfolio->OnExecution.Add( MakeDelegate( this, &Portfolio::HandleExecution ) );
  pPortfolio->OnUnRealizedPL.Add( MakeDelegate( this, &Portfolio::HandleUnRealizedPL ) );

  m_mapSubPortfolios.erase( iter );
}
/*
void Portfolio::SetOwnerPortfolio( const idPortfolio_t& idOwner, pPortfolio_t& pPortfolio ) {
  if ( idOwner != m_row.idOwner ) {
    throw std::runtime_error( "Portfolio::SetOwnerPortfoio " + idOwner + " does not match " + m_row.idOwner );
  }
  m_pOwnerPortfolio = pPortfolio;
}
*/
// as positions and portfolios get attached, they should perform an initial update of 
//   unrealized, realized, & commission (if non-zero)

void Portfolio::HandleUnRealizedPL( const PositionDelta_delegate_t& position ) {

  m_plCurrent.dblUnRealized += ( -position.get<1>() + position.get<2>() );

//  m_row.db.dblUnRealized = m_plCurrent.dblUnRealized;

  m_plCurrent.Sum();
  if ( m_plCurrent > m_plMax ) m_plMax.dblUnRealized = m_plCurrent.dblUnRealized;
  if ( m_plCurrent < m_plMin ) m_plMin.dblUnRealized = m_plCurrent.dblUnRealized;

  // need to propogate up portfolios yet
  OnUnRealizedPL( position );
  OnUnRealizedPLUpdate( *this );

}

void Portfolio::HandleExecution( const PositionDelta_delegate_t& position ) {

  m_row.dblRealizedPL += ( -position.get<1>() + position.get<2>() );

  m_plCurrent.dblRealized = m_row.dblRealizedPL;

  m_plCurrent.Sum();
  if ( m_plCurrent > m_plMax ) m_plMax.dblRealized = m_plCurrent.dblRealized;
  if ( m_plCurrent < m_plMin ) m_plMin.dblRealized = m_plCurrent.dblRealized;

  // need to propogate up portfolios yet
  OnExecution( position );
  OnExecutionUpdate( *this );
}

void Portfolio::HandleCommission( const PositionDelta_delegate_t& position ) {

  m_row.dblCommissionsPaid += ( -position.get<1>() + position.get<2>() );

  m_plCurrent.dblCommissionsPaid = m_row.dblCommissionsPaid;

  m_plCurrent.Sum();
  if ( m_plCurrent > m_plMax ) m_plMax.dblCommissionsPaid = m_plCurrent.dblCommissionsPaid;
  if ( m_plCurrent < m_plMin ) m_plMin.dblCommissionsPaid = m_plCurrent.dblCommissionsPaid;

  // need to propogate up portfolios yet
  OnCommission( position );
  OnCommissionUpdate( *this );

}

//void Portfolio::EmitStats( std::stringstream& ss ) {
std::ostream& operator<<( std::ostream& os, const Portfolio& portfolio ) {
  for ( Portfolio::mapPositions_t::const_iterator iter = portfolio.m_mapPositionsViaUserName.begin(); 
    portfolio.m_mapPositionsViaUserName.end() != iter; 
    ++iter ) {
      os << iter->second;
//    iter->second->EmitStatus( ss );
  }
  os << "Portfolio URPL=" << portfolio.m_plCurrent.dblUnRealized
    << ", RPL=" << portfolio.m_plCurrent.dblRealized 
    << ", Comm=" << portfolio.m_plCurrent.dblCommissionsPaid
    << "=> PL-C=" << portfolio.m_plCurrent.dblRealized - portfolio.m_plCurrent.dblCommissionsPaid
    << ": Min=" << portfolio.m_plMin.dblNet
    << ", Net=" << portfolio.m_plCurrent.dblNet
    << ", Max=" << portfolio.m_plMax.dblNet
    << std::endl
    ;
  return os;
}

} // namespace tf
} // namespace ou
