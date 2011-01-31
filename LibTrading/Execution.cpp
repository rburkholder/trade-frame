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

#include <stdexcept>

#include "LibCommon\TimeSource.h"

#include "Execution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

const std::string CExecution::m_sSqlCreate(     
  "create table executions ( \
    executionid INTEGER PRIMARY KEY, \
    version SMALLINT DEFAULT 1, \
    orderid BIGINT NOT NULL, \
    quantity INTEGER NOT NULL, \
    price DOUBLE NOT NULL, \
    orderside TEXT NOT NULL, \
    executiontime TIMESTAMP NOT NULL, \
    exchange TEXT NOT NULL, \
    exchangeexecutionid TEXT NOT NULL, \
    CONSTRAINT fk_executions_orderid \
      FOREIGN KEY(orderid) REFERENCES orders(orderid) \
        ON DELETE RESTRICT ON UPDATE CASCADE \
       \
    );" 
    );
const std::string CExecution::m_sSqlSelect( 
  "SELECT executionid, orderid, quantity, price, orderside, executiontime, exchagne, exchangeexecutionid \
  FROM executions WHERE executionid = :id;" );
const std::string CExecution::m_sSqlInsert( // executionid is auto-supplied (e
  "INSERT INTO executions ( executionid, orderid, quantity, price, orderside, executiontime, exchange, exchangeexecutionid) \
   VALUES (:executionid, :orderid, :quantity, :price, :orderside, :executiontime, :exchange, :exchangeexecutionid );" );
const std::string CExecution::m_sSqlUpdate( 
  "UPDATE executions SET \
  orderid = :orderid, \
  quantity = :quantity, price = :price, orderside = :orderside, \
  executiontime = :executiontime, \
  exchange = :exchange, exchangeexecutionid = :exchangeexecutionid \
  WHERE executionid = :executionid \
  ;" ); 
const std::string CExecution::m_sSqlDelete( "DELETE FROM executions WHERE executionid = :id;" );

CExecution::CExecution( sqlite3_int64 nExecutionId, sqlite3_stmt* pStmt ) 
: m_bCanUseDb( true ),
  m_nExecutionId( nExecutionId ),
  m_nOrderId( sqlite3_column_int64( pStmt, 0 ) ),
  m_nSize( sqlite3_column_int( pStmt, 1 ) ),
  m_dblPrice( sqlite3_column_double( pStmt, 2 ) ),
  m_eOrderSide( static_cast<OrderSide::enumOrderSide>( sqlite3_column_int( pStmt, 3 ) ) ),
  m_sExchange( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 5 ) ) ),
  m_sExchangeExecutionId( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 6 ) ) )
{
  m_dtExecutionTimeStamp = *reinterpret_cast<const ptime*>( sqlite3_column_blob( pStmt, 4 ) );
}

CExecution::CExecution( 
  sqlite3_int64 nOrderId,
  double dblPrice, unsigned long nSize, OrderSide::enumOrderSide eOrderSide,
  const std::string& sExchange, const std::string& sExecutionId )
: 
  m_bCanUseDb( true ),
  m_nExecutionId( 0 ),
  m_nOrderId( nOrderId ),
  m_dblPrice( dblPrice ), m_nSize( nSize ), m_eOrderSide( eOrderSide ),
  m_sExchange( sExchange ), m_sExchangeExecutionId( sExecutionId )
{
  assert( 0 < dblPrice );
  assert( 0 < nSize );
  m_dtExecutionTimeStamp = ou::CTimeSource::Instance().Internal();
}

CExecution::CExecution( 
  double dblPrice, unsigned long nSize, OrderSide::enumOrderSide eOrderSide,
  const std::string& sExchange, const std::string& sExecutionId )
: 
  m_bCanUseDb( false ),
  m_nExecutionId( 0 ),
  m_dblPrice( dblPrice ), m_nSize( nSize ), m_eOrderSide( eOrderSide ),
  m_sExchange( sExchange ), m_sExchangeExecutionId( sExecutionId )
{
  assert( 0 < dblPrice );
  assert( 0 < nSize );
  m_dtExecutionTimeStamp = ou::CTimeSource::Instance().Internal();
}

CExecution::~CExecution(void) {
}

void CExecution::CreateDbTable( sqlite3* pDb ) {

  char* pMsg;
  int rtn;

  rtn = sqlite3_exec( pDb, m_sSqlCreate.c_str(), 0, 0, &pMsg );

  if ( SQLITE_OK != rtn ) {
    std::string sErr( "Error creating table executions: " );
    sErr += pMsg;
    sqlite3_free( pMsg );
    throw std::runtime_error( sErr );
  }

  rtn = sqlite3_exec( pDb, 
    "create index if not exists idx_executions_orderid on executions( orderid );",
    0, 0, &pMsg );

  if ( SQLITE_OK != rtn ) {
    std::string sErr( "Error creating index idx_executions_orderid: " );
    sErr += pMsg;
    sqlite3_free( pMsg );
    throw std::runtime_error( sErr );
  }
}

int CExecution::BindDbKey( sqlite3_stmt* pStmt ) {

  if ( !m_bCanUseDb ) {
    throw std::runtime_error( "CExecution::BindDbKey: cannot write to db" );
  }

  int rtn( 0 );
  rtn = sqlite3_bind_int64( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":id" ), m_nExecutionId );
  return rtn;
}

int CExecution::BindDbVariables( sqlite3_stmt* pStmt ) {

  if ( !m_bCanUseDb ) {
    throw std::runtime_error( "CExecution::BindDbKey: cannot write to db" );
  }

  int rtn( 0 );
  rtn += sqlite3_bind_int64( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":orderid" ), m_nOrderId );
  rtn += sqlite3_bind_int( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":quantity" ), m_nSize );
  rtn += sqlite3_bind_double( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":price" ), m_dblPrice );
  rtn += sqlite3_bind_int( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":orderside" ), m_eOrderSide );
  rtn += sqlite3_bind_blob( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":executiontime" ), reinterpret_cast<const void*>( &m_dtExecutionTimeStamp ), sizeof( ptime ), SQLITE_STATIC );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":exchange" ), m_sExchange.c_str(), -1, SQLITE_STATIC );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":exchangeexecutionid" ), m_sExchangeExecutionId.c_str(), -1, SQLITE_STATIC );
  return rtn;  // should be 0 if all goes well
}

} // namespace tf
} // namespace ou

