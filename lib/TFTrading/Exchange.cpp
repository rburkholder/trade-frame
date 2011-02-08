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

const std::string CExchange::m_sTableName = "exchanges";

CExchange::CExchange( const idExchange_t& idExchange, const std::string& sName, const std::string& idCountry )
: m_row( idExchange, sName, idCountry )
{
}

CExchange::~CExchange(void)
{
}

  // built as of 2010/10/17

//  rtn = 0;
//  rtn += sqlite3_exec( pDb, "INSERT INTO exchanges (countryid, exchangeid, name) VALUES ('US', 'NYSE', 'New York Stock Exchange');", 0, 0, 0 );
//  rtn += sqlite3_exec( pDb, "INSERT INTO exchanges (countryid, exchangeid, name) VALUES ('US', 'CBOT', 'CBOT');", 0, 0, 0 );
//  rtn += sqlite3_exec( pDb, "INSERT INTO exchanges (countryid, exchangeid, name) VALUES ('US', 'CBOE', 'CBOE');", 0, 0, 0 );
//  rtn += sqlite3_exec( pDb, "INSERT INTO exchanges (countryid, exchangeid, name) VALUES ('US', 'OPRA', 'OPRA');", 0, 0, 0 );
//  rtn += sqlite3_exec( pDb, "INSERT INTO exchanges (countryid, exchangeid, name) VALUES ('US', 'NASDAQ', 'NASDAQ');", 0, 0, 0 );
//  rtn += sqlite3_exec( pDb, "INSERT INTO exchanges (countryid, exchangeid, name) VALUES ('US', 'NMS', 'NMS');", 0, 0, 0 );

//  if ( SQLITE_OK != rtn ) {
//    throw std::runtime_error( "CExchange::CreateDbTable: could not insert records" );
//  }

} // namespace tf
} // namespace ou
