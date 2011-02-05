/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include "Exchange.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

const std::string m_sSqlCreate( 
  "create table exchanges ( \
   exchangeid TEXT CONSTRAINT pk_exchanges PRIMARY KEY, \
   name TEXT NOT NULL, \
   countryid TEXT NOT NULL, \
   CONSTRAINT fk_exchanges_countryid \
     FOREIGN KEY(countryid) REFERENCES countrycodes(alpha2) \
     ON DELETE RESTRICT ON UPDATE CASCADE \
  ;" );
const std::string m_sSqlSelect( "SELECT name, countryid from exchanges where exchangeid = :id;" );
const std::string m_sSqlInsert( "INSERT INTO exchanges (exchangeid, name, countryid) VALUES ( :id, :name, :countryid );" );
const std::string m_sSqlUpdate( "UPDATE exchanges SET name = :name, coutnryid = :countryid WHERE exchangeid = :id;" );
const std::string m_sSqlDelete( "DELETE FROM exchanges WHERE exchangeid = :id;" );

CExchange::CExchange( const idExchange_t& sExchangeId, const std::string& sName, const std::string& sCountryId )
: m_sExchangeId( sExchangeId ), 
  m_sExchangeName( sName ),
  m_sCountryId( sCountryId )
{
}

CExchange::CExchange( const idExchange_t& sExchangeId, sqlite3_stmt* pStmt )
: m_sExchangeId( sExchangeId ), 
  m_sExchangeName( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 0 ) ) ),
  m_sCountryId( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 1 ) ) )
{
}

CExchange::~CExchange(void)
{
}

int CExchange::BindDbKey( sqlite3_stmt* pStmt ) {
  int rtn( 0 );
  rtn = sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":id" ), m_sExchangeId.c_str(), -1, SQLITE_TRANSIENT );
  return rtn;
}

int CExchange::BindDbVariables( sqlite3_stmt* pStmt ) {
  int rtn( 0 );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":name" ), m_sExchangeName.c_str(), -1, SQLITE_TRANSIENT );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":countryid" ), m_sExchangeName.c_str(), -1, SQLITE_TRANSIENT );
  return rtn;  // should be 0 if all goes well
}

void CExchange::CreateDbTable( sqlite3* pDb ) {

  char* pMsg;
  int rtn;

  rtn = sqlite3_exec( pDb, m_sSqlCreate.c_str(), 0, 0, &pMsg );

  if ( SQLITE_OK != rtn ) {
    std::string sErr( "Error creating table exchanges: " );
    sErr += pMsg;
    sqlite3_free( pMsg );
    throw std::runtime_error( sErr );
  }


  // built as of 2010/10/17

  rtn = 0;
  rtn += sqlite3_exec( pDb, "INSERT INTO exchanges (countryid, exchangeid, name) VALUES ('US', 'NYSE', 'New York Stock Exchange');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "INSERT INTO exchanges (countryid, exchangeid, name) VALUES ('US', 'CBOT', 'CBOT');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "INSERT INTO exchanges (countryid, exchangeid, name) VALUES ('US', 'CBOE', 'CBOE');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "INSERT INTO exchanges (countryid, exchangeid, name) VALUES ('US', 'OPRA', 'OPRA');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "INSERT INTO exchanges (countryid, exchangeid, name) VALUES ('US', 'NASDAQ', 'NASDAQ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "INSERT INTO exchanges (countryid, exchangeid, name) VALUES ('US', 'NMS', 'NMS');", 0, 0, 0 );

  if ( SQLITE_OK != rtn ) {
    throw std::runtime_error( "CExchange::CreateDbTable: could not insert records" );
  }
}

} // namespace tf
} // namespace ou
