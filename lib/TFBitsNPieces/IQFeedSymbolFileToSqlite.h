/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

// Started 2012/10/14

/*

Coding for writing to a sqlite database was stopped as it appeared to take about four to five hours to update
about a million records.

header file:

#include <TFTrading/DBOps.h>
#include <TFIQFeed/ValidateMktSymbolLine.h>

  DBOps m_db;
  trd_t m_trd;  // needs to be changed so that new structure can be used each time with prepared statement
  ou::db::QueryFields<ou::tf::iqfeed::MarketSymbol::TableRowDef>::pQueryFields_t pInsertIQFeedSymbol;

source file:
#include <TFIQFeed/ParseMktSymbolDiskFile.h>


  m_db.OnRegisterTables.Add( MakeDelegate( this, &AppCollectAndView::HandleRegisterTables ) );
  m_db.OnRegisterRows.Add( MakeDelegate( this, &AppCollectAndView::HandleRegisterRows ) );
  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppCollectAndView::HandlePopulateDatabase ) );

  m_db.Open( "cav.db" );

  typedef ou::tf::iqfeed::ParseMktSymbolDiskFile diskfile_t;
  diskfile_t diskfile;
  ou::tf::iqfeed::ValidateMktSymbolLine validator;
  diskfile.SetOnProcessLine( MakeDelegate( &validator, &ou::tf::iqfeed::ValidateMktSymbolLine::Parse<diskfile_t::iterator_t> ) );
  validator.SetOnProcessLine( MakeDelegate( this, &AppCollectAndView::HandleParsedStructure ) );

  pInsertIQFeedSymbol = m_db.Insert<ou::tf::iqfeed::MarketSymbol::TableRowDef>( m_trd ).NoExecute();

  diskfile.Run();

  validator.SetOnProcessHasOption( MakeDelegate( this, &AppCollectAndView::HandleUpdateHasOption ) );
  validator.PostProcess();
  validator.Summary();


void AppCollectAndView::HandleParsedStructure( trd_t& trd ) {
  m_trd = trd;
  int i;
  if ( "A" == trd.sSymbol ) {
    i = 0;
  }
  m_db.Reset( pInsertIQFeedSymbol );
  m_db.Bind<ou::tf::iqfeed::MarketSymbol::TableRowDef>( pInsertIQFeedSymbol );
  m_db.Execute( pInsertIQFeedSymbol );
//  ou::db::QueryFields<ou::tf::iqfeed::MarketSymbol::TableRowDef>::pQueryFields_t pInsertIQFeedSymbol 
//    = m_db.Insert<ou::tf::iqfeed::MarketSymbol::TableRowDef>( trd );
}

void AppCollectAndView::HandleUpdateHasOption( const std::string& ) {
}

void AppCollectAndView::HandleRegisterTables(  ou::db::Session& session ) {
  session.RegisterTable<ou::tf::iqfeed::MarketSymbol::TableCreateDef>( "iqfeedsymbols" );
}

void AppCollectAndView::HandleRegisterRows(  ou::db::Session& session ) {
  session.MapRowDefToTableName<ou::tf::iqfeed::MarketSymbol::TableRowDef>( "iqfeedsymbols" );
}





*/

