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

#include <string>
#include <stdexcept>

#include "LibCommon\TimeSource.h"

#include "Execution.h"

CExecution::CExecution( 
  double dblPrice, unsigned long nSize, OrderSide::enumOrderSide eOrderSide,
  const std::string& sExchange, const std::string& sExecutionId )
: 
  m_dblPrice( dblPrice ), m_nSize( nSize ), m_eOrderSide( eOrderSide ),
  m_sExchange( sExchange ), m_sExecutionId( sExecutionId )
{
  assert( 0 < dblPrice );
  assert( 0 < nSize );
  m_dtExecutionTimeStamp = CTimeSource::Instance().Internal();
}

CExecution::~CExecution(void) {
}

void CExecution::CreateDbTable( sqlite3* pDb ) {

  char* pMsg;
  int rtn;

  rtn = sqlite3_exec( pDb,
    "create table if not exists executions ( \
    executionid INTEGER PRIMARY KEY, \
    version SMALLINT DEFAULT 1, \
    orderid BIGINT NOT NULL, \
    CONSTRAINT fk_executions_orderid \
      FOREIGN KEY(orderid) REFERENCES orders(orderid) \
        ON DELETE RESTRICT ON UPDATE CASCADE \
       \
    );",
    0, 0, &pMsg );

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


