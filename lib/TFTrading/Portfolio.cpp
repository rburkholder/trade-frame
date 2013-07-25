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

namespace ou { // One Unified
namespace tf { // TradeFrame

Portfolio::Portfolio( // in memory
  const idPortfolio_t& idPortfolio, currency_t sCurrency,
  const std::string& sDescription ) 
: m_row( idPortfolio, "", "", sCurrency, sDescription ),
  m_bCanUseDb( false )
{
}

Portfolio::Portfolio( // master portfolio currency record
  const idPortfolio_t& idPortfolio, 
  const idAccountOwner_t& idAccountOwner, currency_t sCurrency,
  const std::string& sDescription ) 
: m_row( idPortfolio, idAccountOwner, "", sCurrency, sDescription ), 
  m_bCanUseDb( true )
{
}

Portfolio::Portfolio( // sub-portfolio record
  const idPortfolio_t& idPortfolio, const idAccountOwner_t& idAccountOwner, const idPortfolio_t& idOwner,
  currency_t sCurrency,
  const std::string& sDescription ) 
: m_row( idPortfolio, idAccountOwner, idOwner, sCurrency, sDescription ),
  m_bCanUseDb( true )
{
}

Portfolio::Portfolio( const TableRowDef& row ) 
  : m_row( row ), m_bCanUseDb( true )
{
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

//  pPosition->OnQuote.Add( MakeDelegate( this, &Portfolio::HandleQuote ) );
//  pPosition->OnTrade.Add( MakeDelegate( this, &Portfolio::HandleTrade ) );
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
//  iterUser->second->OnTrade.Remove( MakeDelegate( this, &Portfolio::HandleTrade ) );
//  iterUser->second->OnQuote.Remove( MakeDelegate( this, &Portfolio::HandleQuote ) );

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

void Portfolio::AddSubPortfolio( const idPortfolio_t& idPortfolio, pPortfolio_t& pPortfolio ) {

  mapPortfolios_iter_t iter = m_mapSubPortfolios.find( idPortfolio );

  if ( m_mapSubPortfolios.end() != iter ) {
    throw std::runtime_error( "Portfolio::AddSubPortfolio portfolio already exists: " + idPortfolio );
  }

  m_mapSubPortfolios[ idPortfolio ] = pPortfolio;
}

void Portfolio::RemoveSubPortfolio( const idPortfolio_t& idPortfolio ) {

  mapPortfolios_iter_t iter = m_mapSubPortfolios.find( idPortfolio );

  if ( m_mapSubPortfolios.end() != iter ) {
    throw std::runtime_error( "Portfolio::RemoveSubPortfolio portfolio does not exist: " + idPortfolio );
  }

  m_mapSubPortfolios.erase( iter );
}

void Portfolio::SetOwnerPortfolio( const idPortfolio_t& idOwner, pPortfolio_t& pPortfolio ) {
  if ( idOwner != m_row.idOwner ) {
    throw std::runtime_error( "Portfolio::SetOwnerPortfoio " + idOwner + " does not match " + m_row.idOwner );
  }
  m_pOwnerPortfolio = pPortfolio;
}

// as positions and portfolios get attached, they should perform an initial update of 
//   unrealized, realized, & commission (if non-zero)

void Portfolio::ReCalc( void ) {

  m_plCurrent.Zero();

  for ( mapPortfolios_iter_t iter = m_mapSubPortfolios.begin(); iter != m_mapSubPortfolios.end(); ++iter ) {
  }

  for ( mapPositions_iter_t iter = m_mapPositionsViaUserName.begin(); iter != m_mapPositionsViaUserName.end(); ++iter ) {
    m_plCurrent.dblUnRealized += iter->second->GetUnRealizedPL();
    m_plCurrent.dblRealized += iter->second->GetRealizedPL();
    m_plCurrent.dblCommissionsPaid += iter->second->GetCommissionPaid();
  }
  m_plCurrent.Sum();
  if ( m_plCurrent > m_plMax ) m_plMax = m_plCurrent;
  if ( m_plCurrent < m_plMin ) m_plMin = m_plCurrent;

  m_row.dblRealizedPL = m_plCurrent.dblRealized;
  m_row.dblCommissionsPaid = m_plCurrent.dblCommissionsPaid;
}

void Portfolio::HandleUnRealizedPL( const PositionDelta_delegate_t& position ) {

  m_plCurrent.dblUnRealized += ( -position.get<1>() + position.get<2>() );

  m_row.dblRealizedPL = m_plCurrent.dblRealized;

  m_plCurrent.Sum();
  if ( m_plCurrent > m_plMax ) m_plMax = m_plCurrent;
  if ( m_plCurrent < m_plMin ) m_plMin = m_plCurrent;


  // need to propogate up portfolios yet

}

void Portfolio::HandleExecution( const PositionDelta_delegate_t& position ) {

  m_plCurrent.dblRealized += ( -position.get<1>() + position.get<2>() );

  m_row.dblCommissionsPaid = m_plCurrent.dblCommissionsPaid;

  m_plCurrent.Sum();
  if ( m_plCurrent > m_plMax ) m_plMax = m_plCurrent;
  if ( m_plCurrent < m_plMin ) m_plMin = m_plCurrent;

  // need to propogate up portfolios yet

}

void Portfolio::HandleCommission( const PositionDelta_delegate_t& position ) {

  m_plCurrent.dblCommissionsPaid += ( -position.get<1>() + position.get<2>() );

  m_plCurrent.Sum();
  if ( m_plCurrent > m_plMax ) m_plMax = m_plCurrent;
  if ( m_plCurrent < m_plMin ) m_plMin = m_plCurrent;

  // need to propogate up portfolios yet

}

void Portfolio::HandleQuote( const Position* pPosition ) {
//  ReCalc();
  OnQuote( this );
}

void Portfolio::HandleTrade( const Position* pPosition ) {
  OnTrade( this );
}

void Portfolio::EmitStats( std::stringstream& ss ) {
  for ( mapPositions_iter_t iter = m_mapPositionsViaUserName.begin(); m_mapPositionsViaUserName.end() != iter; ++iter ) {
    iter->second->EmitStatus( ss );
  }
  ss << "Portfolio URPL=" << m_plCurrent.dblUnRealized
    << ", RPL=" << m_plCurrent.dblRealized 
    << ", Comm=" << m_plCurrent.dblCommissionsPaid
    << "=> PL-C=" << m_plCurrent.dblRealized - m_plCurrent.dblCommissionsPaid
    << ": Min=" << m_plMin.dblNet
    << ", Net=" << m_plCurrent.dblNet
    << ", Max=" << m_plMax.dblNet
    << std::endl
    ;
}

} // namespace tf
} // namespace ou
