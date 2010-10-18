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

const std::string CPortfolio::m_sSqlCreate( 
  "create table  portfolios ( \
    portfolioid TEXT CONSTRAINT pk_portfolios PRIMARY KEY, \
    version SMALLINT DEFAULT 1, \
    accountownerid TEXT NOT NULL, \
    description TEXT default '', \
    realizedpl double default 0.0, \
    commissionspaid double default 0.0, \
    CONSTRAINT fk_portfolios_accountownerid \
      FOREIGN KEY(accountownerid) REFERENCES accountownerss(accountownerid) \
        ON DELETE RESTRICT ON UPDATE CASCADE \
       \
    );" );
const std::string CPortfolio::m_sSqlSelect( 
  "SELECT accountid, description, realizedpl, commissionspaid FROM portfolios where portfolioid = :id;" );
const std::string CPortfolio::m_sSqlInsert( 
  "INSERT INTO portfolios (portfolioid, accountownerid, description, realizedpl, commissionspaid) \
   VALUES (:id, :accountownerid, :description, :realizedpl, :commissionspaid) where portfolioid = :id;" );
const std::string CPortfolio::m_sSqlUpdate( 
  "UPDATE portfolios SET \
   description = :description, realizedpl = :realizedpl, commissionspaid = :commissionspaid \
   WHERE porfolioid = :portfolioid;" );
const std::string CPortfolio::m_sSqlDelete( "DELETE FROM portfolios WHERE portfolioid = :id;" );

CPortfolio::CPortfolio( 
    const keyPortfolioId_t& sPortfolioId, 
    const std::string& sDescription ) 
: m_sPortfolioId( sPortfolioId ),
  m_sDescription( sDescription ),
  m_bCanUseDb( false )
{
}

CPortfolio::CPortfolio( 
    const keyPortfolioId_t& sPortfolioId, 
    const keyAccountOwnerId_t& sAccountOwnerId, 
    const std::string& sDescription ) 
: m_sPortfolioId( sPortfolioId ),
  m_sAccountOwnerId( sAccountOwnerId ),
  m_sDescription( sDescription ),
  m_bCanUseDb( true )
{
}

CPortfolio::CPortfolio( const keyPortfolioId_t& sPortfolioId, sqlite3_stmt* pStmt ) 
: m_sPortfolioId( sPortfolioId ),
  m_sAccountOwnerId( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 0 ) ) ),
  m_sDescription( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 1 ) ) ),
  m_bCanUseDb( true )
{
  m_plCurrent.dblRealized = sqlite3_column_double( pStmt, 2 );
  m_plCurrent.dblCommissionsPaid = sqlite3_column_double( pStmt, 3 );
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
}

void CPortfolio::HandleQuote( const CPosition* pPosition ) {
  ReCalc();
}

void CPortfolio::HandleTrade( const CPosition* pPosition ) {
}

void CPortfolio::HandleExecution( CPosition::execution_delegate_t ) {
  ReCalc();
}

void CPortfolio::HandleCommission( const CPosition* pPosition ) {
  ReCalc();
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

void CPortfolio::CreateDbTable( sqlite3* pDb ) {

  char* pMsg;
  int rtn;

  rtn = sqlite3_exec( pDb, m_sSqlCreate.c_str(), 0, 0, &pMsg );

  if ( SQLITE_OK != rtn ) {
    std::string sErr( "Error creating table portfolios: " );
    sErr += pMsg;
    sqlite3_free( pMsg );
    throw std::runtime_error( sErr );
  }

  rtn = sqlite3_exec( pDb, 
    "create index idx_portfolio_accountid on portfolios( accountid );",
    0, 0, &pMsg );

  if ( SQLITE_OK != rtn ) {
    std::string sErr( "Error creating index idx_portfolios_accountid: " );
    sErr += pMsg;
    sqlite3_free( pMsg );
    throw std::runtime_error( sErr );
  }
}

int CPortfolio::BindDbKey( sqlite3_stmt* pStmt ) {
  if ( !m_bCanUseDb ) {
    throw std::runtime_error( "CPortfolio::BindDbKey, can not save to db" );
  }
  int rtn( 0 );
  rtn = sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":id" ), m_sPortfolioId.c_str(), -1, SQLITE_TRANSIENT );
  return rtn;
}

int CPortfolio::BindDbVariables( sqlite3_stmt* pStmt ) {
  if ( !m_bCanUseDb ) {
    throw std::runtime_error( "CPortfolio::BindDbVariables, can not save to db" );
  }
  int rtn( 0 );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":accountownerid" ), m_sAccountOwnerId.c_str(), -1, SQLITE_TRANSIENT );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":description" ), m_sDescription.c_str(), -1, SQLITE_TRANSIENT );
  rtn += sqlite3_bind_double( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":realizedpl" ), m_plCurrent.dblRealized );
  rtn += sqlite3_bind_double( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":commissionspaid" ), m_plCurrent.dblCommissionsPaid );
  return rtn;  // should be 0 if all goes well
}

