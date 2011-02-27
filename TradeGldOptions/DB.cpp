/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include "DB.h"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include <TFTrading/AccountAdvisor.h>
#include <TFTrading/AccountOwner.h>
#include <TFTrading/Account.h>
#include <TFTrading/Portfolio.h>
#include <TFTrading/Position.h>
#include <TFTrading/Order.h>
#include <TFTrading/Execution.h>
#include <TFTrading/Instrument.h>
#include <TFTrading/Exchange.h>

using namespace ou::tf;

CDB::CDB(void) {
}


CDB::~CDB(void) {
}

void CDB::Open( const std::string& sDbName ) {

  if ( boost::filesystem::exists( sDbName ) ) {
    // open already created and loaded database
    m_db.Open( sDbName );
  }
  else {
    // create and build new database
    m_db.Open( sDbName, ou::db::EOpenFlagsAutoCreate );
    Populate();
  }

}

void CDB::Close( void ) {
  m_db.Close();
}

void CDB::Populate( void ) {

  m_db.RegisterTable<CAccountAdvisor::TableCreateDef>( tablenames::sAccountAdvisor );
  m_db.RegisterTable<CAccountOwner::TableCreateDef>( tablenames::sAccountOwner );
  m_db.RegisterTable<CAccount::TableCreateDef>( tablenames::sAccount );
  m_db.RegisterTable<CPortfolio::TableCreateDef>( tablenames::sPortfolio );
  m_db.RegisterTable<CPosition::TableCreateDef>( tablenames::sPosition );
  m_db.RegisterTable<COrder::TableCreateDef>( tablenames::sOrder );
  m_db.RegisterTable<CExecution::TableCreateDef>( tablenames::sExecution );
  m_db.RegisterTable<CInstrument::TableCreateDef>( tablenames::sInstrument );
  m_db.RegisterTable<CAlternateInstrumentName::TableCreateDef>( tablenames::sAltInstrumentName );
  m_db.RegisterTable<CExchange::TableCreateDef>( tablenames::sExchange );

  m_db.CreateTables();

  m_db.MapTableToFields<CAccountAdvisor::TableRowDef>( tablenames::sAccountAdvisor );
  m_db.MapTableToFields<CAccountOwner::TableRowDef>( tablenames::sAccountOwner );
  m_db.MapTableToFields<CAccount::TableRowDef>( tablenames::sAccount );
  m_db.MapTableToFields<CPortfolio::TableRowDef>( tablenames::sPortfolio );
  m_db.MapTableToFields<CPosition::TableRowDef>( tablenames::sPosition );
  m_db.MapTableToFields<COrder::TableRowDef>( tablenames::sOrder );
  m_db.MapTableToFields<CExecution::TableRowDef>( tablenames::sExecution );
  m_db.MapTableToFields<CInstrument::TableRowDef>( tablenames::sInstrument );
  m_db.MapTableToFields<CAlternateInstrumentName::TableRowDef>( tablenames::sAltInstrumentName );
  m_db.MapTableToFields<CExchange::TableRowDef>( tablenames::sExchange );

  CAccountAdvisor::TableRowDef aa( "ray", "Raymond Burkholder", "One Unified" );
  ou::db::QueryFields<CAccountAdvisor::TableRowDef>::pQueryFields_t paa = m_db.Insert<CAccountAdvisor::TableRowDef>( aa );

  CAccountOwner::TableRowDef ao( "ray", "ray", "Raymond", "Burkholder" );
  ou::db::QueryFields<CAccountOwner::TableRowDef>::pQueryFields_t pao = m_db.Insert<CAccountOwner::TableRowDef>( ao );

  CAccount::TableRowDef acctIB( "ib01", "ray", "Raymond Burkholder", "IB", "Interactive Brokers", "acctid", "login", "password" );
  ou::db::QueryFields<CAccount::TableRowDef>::pQueryFields_t paIB = m_db.Insert<CAccount::TableRowDef>( acctIB );

  CAccount::TableRowDef acctIQ( "iq01", "ray", "Raymond Burkholder", "IQF", "IQFeed", "acctid", "login", "password" );
  ou::db::QueryFields<CAccount::TableRowDef>::pQueryFields_t paIQF = m_db.Insert<CAccount::TableRowDef>( acctIQ );

  CPortfolio::TableRowDef portfolio( "vol01", "ray", "Volatility Tests" );
  ou::db::QueryFields<CPortfolio::TableRowDef>::pQueryFields_t pPortfolio = m_db.Insert<CPortfolio::TableRowDef>( portfolio );

  CExchange::TableRowDef exchange( "NYSE", "New York Stock Exchange", "US" );
  ou::db::QueryFields<CExchange::TableRowDef>::pQueryFields_t pExchange = m_db.Insert<CExchange::TableRowDef>( exchange );

  exchange.idExchange = "CBOT";
  exchange.sName = "Chicago Board of Trade";
  pExchange = m_db.Insert<CExchange::TableRowDef>( exchange );

  exchange.idExchange = "CBOE";
  exchange.sName = "Chicago Board Options Exchange";
  pExchange = m_db.Insert<CExchange::TableRowDef>( exchange );

  exchange.idExchange = "OPRA";
  exchange.sName = "Options Price Reporting Authority";
  pExchange = m_db.Insert<CExchange::TableRowDef>( exchange );

  exchange.idExchange = "NASDAQ";
  exchange.sName = "NASDAQ";
  pExchange = m_db.Insert<CExchange::TableRowDef>( exchange );

  exchange.idExchange = "NMS";
  exchange.sName = "NMS";
  pExchange = m_db.Insert<CExchange::TableRowDef>( exchange );

}