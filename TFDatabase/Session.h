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

#include <string>

#include <boost/shared_ptr.hpp>

#include <OUSQL/Session.h>

#include <TFTrading/AccountAdvisor.h>
#include <TFTrading/AccountOwner.h>
#include <TFTrading/Account.h>
#include <TFTrading/Portfolio.h>
#include <TFTrading/Position.h>
#include <TFTrading/Order.h>
#include <TFTrading/Execution.h>
#include <TFTrading/Instrument.h>
#include <TFTrading/Exchange.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

template<class DB>
void ConstructTables( ou::db::CSession<DB>& session ) {

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

  // may need to do tables individually in case other tables have already been created.
  session.CreateTables();

  session.MapTableToFields<CExchange::TableRowDef>( tablenames::sExchange );
  session.MapTableToFields<CInstrument::TableRowDef>( tablenames::sInstrument );
  session.MapTableToFields<CAlternateInstrumentName::TableRowDef>( tablenames::sAltInstrumentName );

  session.MapTableToFields<CAccountAdvisor::TableRowDef>( tablenames::sAccountAdvisor );
  session.MapTableToFields<CAccountOwner::TableRowDef>( tablenames::sAccountOwner );
  session.MapTableToFields<CAccount::TableRowDef>( tablenames::sAccount );
  session.MapTableToFields<CPortfolio::TableRowDef>( tablenames::sPortfolio );
  session.MapTableToFields<CPosition::TableRowDef>( tablenames::sPosition );
  session.MapTableToFields<COrder::TableRowDef>( tablenames::sOrder );
  session.MapTableToFields<CExecution::TableRowDef>( tablenames::sExecution );

// move these back into a vector in exchange.cpp and then do a loop to load to the db
  
  CExchange::TableRowDef exchange( "NYSE", "New York Stock Exchange", "US" );
  ou::db::QueryFields<CExchange::TableRowDef>::pQueryFields_t pExchange = session.Insert<CExchange::TableRowDef>( exchange );

  exchange.idExchange = "CBOT";
  exchange.sName = "Chicago Board of Trade";
  pExchange = session.Insert<CExchange::TableRowDef>( exchange );

  exchange.idExchange = "CBOE";
  exchange.sName = "Chicago Board Options Exchange";
  pExchange = session.Insert<CExchange::TableRowDef>( exchange );

  exchange.idExchange = "OPRA";
  exchange.sName = "Options Price Reporting Authority";
  pExchange = session.Insert<CExchange::TableRowDef>( exchange );

  exchange.idExchange = "NASDAQ";
  exchange.sName = "NASDAQ";
  pExchange = session.Insert<CExchange::TableRowDef>( exchange );

  exchange.idExchange = "NMS";
  exchange.sName = "NMS";
  pExchange = session.Insert<CExchange::TableRowDef>( exchange );

}

} // namespace tf
} // namespace ou

