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

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <LibSqlite/sqlite3.h>

#include "TradingEnumerations.h"

class CExecution {
public:

  typedef unsigned long ExecutionId_t;  // used for database updates, need to persist like orderid
  typedef boost::shared_ptr<CExecution> pExecution_t;
  typedef const pExecution_t& pExecution_ref;

  CExecution( sqlite3_int64 nExecutionId, sqlite3_stmt* pStmt );
  CExecution( // in memory useage
    double dblPrice, unsigned long nSize, OrderSide::enumOrderSide eOrderSide,
    const std::string& sExchange, const std::string& sExecutionId );
  CExecution( // for when record will be written to db
    sqlite3_int64 nOrderId,
    double dblPrice, unsigned long nSize, OrderSide::enumOrderSide eOrderSide,
    const std::string& sExchange, const std::string& sExecutionId );
  ~CExecution(void);

  double GetPrice( void ) const { return m_dblPrice; };
  unsigned long GetSize( void ) const { return m_nSize; };
  OrderSide::enumOrderSide GetOrderSide( void ) const { return m_eOrderSide; };
  const std::string& GetExchange( void ) const { return m_sExchange; };
  const std::string& GetExchangeExecutionId( void ) const { return m_sExchangeExecutionId; };
  ptime GetTimeStamp( void ) const { return m_dtExecutionTimeStamp; };

  static void CreateDbTable( sqlite3* pDb );
  int BindDbKey( sqlite3_stmt* pStmt );
  int BindDbVariables( sqlite3_stmt* pStmt );
  static const std::string& GetSqlSelect( void ) { return m_sSqlSelect; };
  static const std::string& GetSqlInsert( void ) { return m_sSqlInsert; };
  static const std::string& GetSqlUpdate( void ) { return m_sSqlUpdate; };
  static const std::string& GetSqlDelete( void ) { return m_sSqlDelete; };

protected:

  sqlite3_int64 m_nExecutionId;
  sqlite3_int64 m_nOrderId;
  double m_dblPrice;  // execution price
  unsigned long m_nSize;  // quantity executed
  OrderSide::enumOrderSide m_eOrderSide;
  ptime m_dtExecutionTimeStamp;
  std::string m_sExchange;
  std::string m_sExchangeExecutionId;  // unique execution id supplied by provider

private:

  bool m_bCanUseDb;

  static const std::string m_sSqlCreate;
  static const std::string m_sSqlSelect;
  static const std::string m_sSqlInsert;
  static const std::string m_sSqlUpdate;
  static const std::string m_sSqlDelete;

};
