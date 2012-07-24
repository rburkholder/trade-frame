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

CPortfolio::CPortfolio( 
  const idPortfolio_t& idPortfolio, 
  const std::string& sDescription ) 
: m_row( idPortfolio, "", sDescription ),
  m_bCanUseDb( false )
{
}

CPortfolio::CPortfolio( 
  const idPortfolio_t& idPortfolio, 
  const idAccountOwner_t& idAccountOwner, 
  const std::string& sDescription ) 
: m_row( idPortfolio, idAccountOwner, sDescription ),
  m_bCanUseDb( true )
{
}

CPortfolio::CPortfolio( const TableRowDef& row ) 
  : m_row( row ), m_bCanUseDb( true )
{
}

CPortfolio::~CPortfolio(void) {
}

void CPortfolio::AddPosition( const std::string &sName, pPosition_t pPosition ) {

  // prepare to add position to user named map
  iterator iterUser = m_mapPositionsViaUserName.find( sName );
  if ( m_mapPositionsViaUserName.end() != iterUser ) {
    throw std::runtime_error( "CPortfolio::Add1 position already exists" );
  }

  // prepare to add position to instrument named map
  // instrument can only have one position associated with it
  const std::string& sInstrumentName( pPosition->GetInstrument()->GetInstrumentName() );
  iterator iterInst = m_mapPositionsViaInstrumentName.find( sInstrumentName );
  if ( m_mapPositionsViaInstrumentName.end() != iterInst ) {
    throw std::runtime_error( "CPortfolio::Add2 position already exists" );
  }

  m_mapPositionsViaUserName.insert( map_t_pair( sName, pPosition ) );
  m_mapPositionsViaInstrumentName.insert( map_t_pair( sInstrumentName, pPosition ) );

  pPosition->OnQuote.Add( MakeDelegate( this, &CPortfolio::HandleQuote ) );
  pPosition->OnExecution.Add( MakeDelegate( this, &CPortfolio::HandleExecution ) );
  pPosition->OnCommission.Add( MakeDelegate( this, &CPortfolio::HandleCommission ) );

}

void CPortfolio::DeletePosition( const std::string& sName ) {

  iterator iterUser = m_mapPositionsViaUserName.find( sName );
  if ( m_mapPositionsViaUserName.end() == iterUser ) {
    throw std::runtime_error( "CPortfolio::Delete1 position does not exist" );
  }

  const std::string& sInstrumentName( iterUser->second->GetInstrument()->GetInstrumentName() );
  iterator iterInst = m_mapPositionsViaInstrumentName.find( sInstrumentName );
  if ( m_mapPositionsViaInstrumentName.end() == iterInst ) {
    throw std::runtime_error( "CPortfolio::Delete2 position does not exist" );
  }

  iterUser->second->OnCommission.Remove( MakeDelegate( this, &CPortfolio::HandleCommission ) );
  iterUser->second->OnExecution.Remove( MakeDelegate( this, &CPortfolio::HandleExecution ) );
  iterUser->second->OnQuote.Remove( MakeDelegate( this, &CPortfolio::HandleQuote ) );

  m_mapPositionsViaUserName.erase( iterUser );
  m_mapPositionsViaInstrumentName.erase( iterInst );

}

void CPortfolio::RenamePosition( const std::string& sOld, const std::string& sNew ) {

  iterator iter;

  iter = m_mapPositionsViaUserName.find( sNew );
  if ( m_mapPositionsViaUserName.end() == iter ) {
    throw std::runtime_error( "CPortfolio::Rename New position already exists" );
  }
  iter = m_mapPositionsViaUserName.find( sOld );
  if ( m_mapPositionsViaUserName.end() == iter ) {
    throw std::runtime_error( "CPortfolio::Rename Old position does not exist" );
  }

  pPosition_t pPosition( iter->second );
  m_mapPositionsViaUserName.erase( iter );
  m_mapPositionsViaUserName.insert( map_t_pair( sNew, pPosition ) );
}

CPortfolio::pPosition_t CPortfolio::GetPosition( const std::string& sName ) {

  iterator iter = m_mapPositionsViaUserName.find( sName );

  if ( m_mapPositionsViaUserName.end() == iter ) {
    throw std::runtime_error( "CPortfolio::GetPosition position does not exist" );
  }

  return iter->second;
}

void CPortfolio::ReCalc( void ) {
  m_plCurrent.Zero();

  for ( iterator iter = m_mapPositionsViaUserName.begin(); iter != m_mapPositionsViaUserName.end(); ++iter ) {
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

void CPortfolio::HandleQuote( const CPosition* pPosition ) {
  ReCalc();
  OnQuote( this );
}

void CPortfolio::HandleTrade( const CPosition* pPosition ) {
  OnTrade( this );
}

void CPortfolio::HandleExecution( execution_delegate_t ) {
  ReCalc();
  OnExecution( this );
}

void CPortfolio::HandleCommission( const CPosition* pPosition ) {
  ReCalc();
  OnCommission( this );
}

void CPortfolio::EmitStats( std::stringstream& ss ) {
  ss.str( "" );
  for ( iterator iter = m_mapPositionsViaUserName.begin(); m_mapPositionsViaUserName.end() != iter; ++iter ) {
    iter->second->EmitStatus( ss );
  }
  ss << "Portfolio URPL=" << m_plCurrent.dblUnRealized
    << ", RPL=" << m_plCurrent.dblRealized 
    << ", Comm=" << m_plCurrent.dblCommissionsPaid
    << "=> PL-C=" << m_plCurrent.dblRealized - m_plCurrent.dblCommissionsPaid
    << ": Min=" << m_plMin.dblNet
    << ", Net=" << m_plCurrent.dblNet
    << ", Max=" << m_plMax.dblNet
    << std::endl;
}

} // namespace tf
} // namespace ou
