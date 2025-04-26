/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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

/*
 * File:    Main.cpp
 * Author:  raymond@burkholder.net
 * Project: Dividend
 * Created: April 1, 2022  19:09
 */

#include <OUCommon/TimeSource.h>

#include <TFTrading/Database.h>

#include "TableTag.hpp"
#include "TableSymbol.hpp"
#include "TableDividend.hpp"

#include "Config.hpp"
#include "Process.hpp"

void HandleRegisterTables( ou::db::Session& session ) {
  // called when db created
  //std::cout << "HandleRegisterTables placeholder" << std::endl;
  session.RegisterTable<db::table::Tag::TableCreateDef>( db::table::Tag::c_TableName );
  session.RegisterTable<db::table::Symbol::TableCreateDef>( db::table::Symbol::c_TableName );
  session.RegisterTable<db::table::Dividend::TableCreateDef>( db::table::Dividend::c_TableName );
}

void HandleRegisterRows( ou::db::Session& session ) {
  // called when db created and when exists
  //std::cout << "HandleRegisterRows placeholder" << std::endl;
  session.MapRowDefToTableName<db::table::Tag::TableRowDef>( db::table::Tag::c_TableName );
  session.MapRowDefToTableName<db::table::Symbol::TableRowDef>( db::table::Symbol::c_TableName );
  session.MapRowDefToTableName<db::table::Dividend::TableRowDef>( db::table::Dividend::c_TableName );
}

struct SymbolKey {
  const std::string& sSymbol;
  template<class A>
  void Fields( A& a ) {
    ou::db::Field( a, "symbol_name", sSymbol );
  }
  SymbolKey( const std::string& sSymbol_ ): sSymbol( sSymbol_ ) {};
};

bool SymbolExists( ou::db::Session& db, const std::string& sSymbol, db::table::Symbol::TableRowDef& row ) {
  bool bFound( false );
  SymbolKey key( sSymbol );
  ou::db::QueryFields<SymbolKey>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
    = db.SQL<SymbolKey>( "select * from " + db::table::Symbol::c_TableName, key ).Where( "symbol_name = ?" ).NoExecute();
  db.Bind<SymbolKey>( pExistsQuery );
  if ( db.Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
    db.Columns<SymbolKey, db::table::Symbol::TableRowDef>( pExistsQuery, row );
    bFound = true;
  }
  return bFound;
}

struct UpdateSymbol {
  const std::string& sSymbol;
  const boost::gregorian::date dateUpdated;
  template<class A>
  void Fields( A& a ) {
    ou::db::Field( a, "date_updated", dateUpdated );
    ou::db::Field( a, "symbol_name", sSymbol );
  }
  UpdateSymbol( const std::string& sSymbol_, const boost::gregorian::date dateUpdated_ )
  : sSymbol( sSymbol_ ), dateUpdated( dateUpdated_ ) {}
};

int main( int argc, char* argv[] ) {

  const static std::string sConfigFileName( "dividend.cfg" );
  const static std::string sDbFileName(     "dividend.db"  );

  config::Choices choices;

  if ( Load( sConfigFileName, choices ) ) {
  }
  else {
    return EXIT_FAILURE;
  }

  //if ( boost::filesystem::exists( c_sDbName ) ) {
  //  boost::filesystem::remove( c_sDbName );
  //}

  ou::db::Session db;
  db.OnRegisterTables.Add( &HandleRegisterTables );
  db.OnRegisterRows.Add( &HandleRegisterRows );
  db.Open( sDbFileName );

  using vSymbols_t = Process::vSymbols_t;
  vSymbols_t vSymbols;

  {
    Process process( choices, vSymbols );
    process.Wait();
  }

  const boost::gregorian::date today( ou::TimeSource::GlobalInstance().External().date() );

  const bool bStdOut( false );

  if ( bStdOut ) {
    std::cout
    << "symbol,name,exchange,last($),yield(%),ycalc,rate,amount,vol,exdiv,payed,shares,state,option"
    << std::endl;
  }

  size_t cnt {};
  for ( vSymbols_t::value_type& vt: vSymbols ) {
    if ( ( choices.m_dblMinimumYield <= vt.yield ) && ( choices.m_nMinimumVolume <= vt.nAverageVolume ) ) {
    //if ( ( choices.m_dblMinimumYield <= vt.yield_calculated ) && ( choices.m_nMinimumVolume <= vt.nAverageVolume ) ) {
      if ( bStdOut ) {
        std::cout
                 << vt.sSymbol
          << ',' << vt.sCompanyName
          << ',' << vt.sExchange
          << ',' << vt.trade
          << ',' << vt.yield
          << ',' << vt.yield_calculated
          << ',' << vt.rate
          << ',' << vt.amount
          << ',' << vt.nAverageVolume
          << ',' << vt.dateExDividend
          << ',' << vt.datePayed
          << ',' << vt.nSharesOutstanding
          << ',' << vt.sState
          << ',' << vt.sOptionRoots
          << std::endl;
      }

      db::table::Symbol::TableRowDef trdSymbol( // might be overwritten
        vt.sSymbol, vt.sCompanyName, today, today, vt.sState
      );
      if ( SymbolExists( db, vt.sSymbol, trdSymbol ) ) {
        UpdateSymbol update( vt.sSymbol, today );
        ou::db::QueryFields<UpdateSymbol>::pQueryFields_t pQuery
          = db.SQL<UpdateSymbol>(
            "update " + db::table::Symbol::c_TableName + " set date_updated=?", update ).Where( "symbol_name=?" )
          ;
        trdSymbol.dateUpdated = today;
      }
      else {
        ou::db::QueryFields<db::table::Symbol::TableRowDef>::pQueryFields_t pQuery
          = db.Insert<db::table::Symbol::TableRowDef>( trdSymbol );
      }

      db::table::Dividend::TableRowDef trdDividend(
        vt.sSymbol
      , today
      , vt.trade
      , vt.rate
      , vt.yield
      , vt.amount
      , vt.nAverageVolume
      , vt.nSharesOutstanding
      , vt.datePayed
      , vt.dateExDividend
      );

      ou::db::QueryFields<db::table::Dividend::TableRowDef>::pQueryFields_t pQuery
        = db.Insert<db::table::Dividend::TableRowDef>( const_cast<db::table::Dividend::TableRowDef&>( trdDividend ) );

      ++cnt;
    }
  }

  std::cout << "symbols written: " << cnt << std::endl;

  db.Close();
  db.OnRegisterRows.Remove( &HandleRegisterRows );
  db.OnRegisterTables.Remove( &HandleRegisterTables );

  return EXIT_SUCCESS;
}

// https://dividendhistory.org
// https://www.barchart.com
// https://www.dividend.com

// https://www.nasdaq.com/market-activity/funds-and-etfs/xxxx/dividend-history
