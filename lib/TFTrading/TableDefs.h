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

// construct a new session, complete with tables and a pre-populate

#pragma once

#include <string>
#include <vector>

#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

//#include <OUSQL/Session.h>

#include <TFTrading/Instrument.h>
#include <TFTrading/Exchange.h>
#include <TFTrading/AccountAdvisor.h>
#include <TFTrading/AccountOwner.h>
#include <TFTrading/Account.h>
#include <TFTrading/Portfolio.h>
#include <TFTrading/Position.h>
#include <TFTrading/Order.h>
#include <TFTrading/Execution.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace db { // DataBase

template<class DB>
void RegisterTableCreation( ou::db::CSession<DB>& session ) {

  session.RegisterTable<CExchange::TableCreateDef>( tablenames::sExchange );
  session.RegisterTable<CInstrument::TableCreateDef>( tablenames::sInstrument );
  session.RegisterTable<CAlternateInstrumentName::TableCreateDef>( tablenames::sAltInstrumentName );

  session.RegisterTable<CAccountAdvisor::TableCreateDef>( tablenames::sAccountAdvisor );
  session.RegisterTable<CAccountOwner::TableCreateDef>( tablenames::sAccountOwner );
  session.RegisterTable<CAccount::TableCreateDef>( tablenames::sAccount );
  session.RegisterTable<CPortfolio::TableCreateDef>( tablenames::sPortfolio );
  session.RegisterTable<CPosition::TableCreateDef>( tablenames::sPosition );
  session.RegisterTable<COrder::TableCreateDef>( tablenames::sOrder );
  session.RegisterTable<CExecution::TableCreateDef>( tablenames::sExecution );
}

template<class DB>
void RegisterRowDefinitions( ou::db::CSession<DB>& session ) {

  session.MapRowDefToTableName<CExchange::TableRowDef>( tablenames::sExchange );
  session.MapRowDefToTableName<CInstrument::TableRowDef>( tablenames::sInstrument );
  session.MapRowDefToTableName<CAlternateInstrumentName::TableRowDef>( tablenames::sAltInstrumentName );

  session.MapRowDefToTableName<CAccountAdvisor::TableRowDef>( tablenames::sAccountAdvisor );
  session.MapRowDefToTableName<CAccountOwner::TableRowDef>( tablenames::sAccountOwner );
  session.MapRowDefToTableName<CAccount::TableRowDef>( tablenames::sAccount );
  session.MapRowDefToTableName<CPortfolio::TableRowDef>( tablenames::sPortfolio );
  session.MapRowDefToTableName<CPosition::TableRowDef>( tablenames::sPosition );
  session.MapRowDefToTableName<COrder::TableRowDef>( tablenames::sOrder );
  session.MapRowDefToTableName<CExecution::TableRowDef>( tablenames::sExecution );

  //session.CreateTables(); // done elsewhere
}

template<class DB>
void PopulateTables( ou::db::CSession<DB>& session ) {

  std::vector<std::string> vsExchangesPreload;
  vsExchangesPreload +=
    "NYSE", "New York Stock Exchange",
    "CBOT", "Chicago Board of Trade",
    "CBOE", "Chicago Board Options Exchange",
    "OPRA", "Options Price Reporting Authority",
    "NASDAQ", "NASDAQ",
    "NMS", "NMS",
    "SMART", "Interactive Brokers Smart"
  ;
  
  assert( 0 < vsExchangesPreload.size() );
  assert( 0 == ( vsExchangesPreload.size() % 2 ) );

  CExchange::TableRowDef exchange;
  exchange.idCountry = "US";

  std::vector<std::string>::iterator iter = vsExchangesPreload.begin();

  ou::db::QueryFields<CExchange::TableRowDef>::pQueryFields_t pExchange = session.Insert<CExchange::TableRowDef>( exchange ).NoExecute();

  while ( vsExchangesPreload.end() != iter ) {
    exchange.idExchange = *(iter++);
    exchange.sName = *(iter++);
    session.Reset( pExchange );
    session.Bind<CExchange::TableRowDef>( pExchange );
    session.Execute( pExchange );
  }

}

} // namespace db
} // namespace tf
} // namespace ou

