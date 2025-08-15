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

#include <boost/log/trivial.hpp>

#include <OUCommon/TimeSource.h>

#include <DBDividend/TableTag.hpp>
#include <DBDividend/TableDaily.hpp>
#include <DBDividend/TableSymbol.hpp>
#include <DBDividend/TableDividend.hpp>
#include <DBDividend/TableDividendDaily.hpp>

#include <TFTrading/Database.h>

#include "Config.hpp"
#include "Process.hpp"

void HandleRegisterTables( ou::db::Session& session ) {
  // called when db created
  //std::cout << "HandleRegisterTables placeholder" << std::endl;
  session.RegisterTable<db::table::Tag::TableCreateDef>( db::table::Tag::c_TableName );
  session.RegisterTable<db::table::Daily::TableCreateDef>( db::table::Daily::c_TableName );
  session.RegisterTable<db::table::Symbol::TableCreateDef>( db::table::Symbol::c_TableName );
  session.RegisterTable<db::table::Dividend::TableCreateDef>( db::table::Dividend::c_TableName );
  //session.RegisterTable<db::table::DividendDaily::TableCreateDef>( db::table::DividendDaily::c_TableName ); // do not construct - never enabled
}

void HandleRegisterRows( ou::db::Session& session ) {
  // called when db created and when exists
  //std::cout << "HandleRegisterRows placeholder" << std::endl;
  session.MapRowDefToTableName<db::table::Tag::TableRowDef>( db::table::Tag::c_TableName );
  session.MapRowDefToTableName<db::table::Daily::TableRowDef>( db::table::Daily::c_TableName );
  session.MapRowDefToTableName<db::table::Symbol::TableRowDef>( db::table::Symbol::c_TableName );
  session.MapRowDefToTableName<db::table::Dividend::TableRowDef>( db::table::Dividend::c_TableName );
  //session.MapRowDefToTableName<db::table::DividendDaily::TableRowDef>( db::table::DividendDaily::c_TableName ); // use only when fix-up is true
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

struct SymbolDatesSearch {
  const std::string& sSymbol;
  boost::gregorian::date dateExDividend;
  boost::gregorian::date datePayed;
  template<class A>
  void Fields( A& a ) {
    ou::db::Field( a, "symbol_name", sSymbol );
    ou::db::Field( a, "date_exdividend", dateExDividend );
    ou::db::Field( a, "date_payed", datePayed );
  }
  SymbolDatesSearch( const std::string& sSymbol_, boost::gregorian::date dateEx_, boost::gregorian::date datePayed_ )
  : sSymbol( sSymbol_ ), dateExDividend( dateEx_ ), datePayed( datePayed_ ) {}
  //SymbolDatesSearch( const std::string& sSymbol_, boost::gregorian::date dateEx_ )
  //: sSymbol( sSymbol_ ), dateExDividend( dateEx_ ) {}
};

bool SymbolExDividendExists(
  ou::db::Session& db
, const std::string& sSymbol, const boost::gregorian::date dateExDividend, const boost::gregorian::date datePayed
, db::table::Dividend::TableRowDef& row )
{
  bool bFound( false );
  SymbolDatesSearch key( sSymbol, dateExDividend, datePayed );
  //SymbolDatesSearch key( sSymbol, dateExDividend );
  //BOOST_LOG_TRIVIAL(trace) << "SymbolExDividendExists: " << sSymbol << ",ex=" << dateExDividend << ",dp=" << datePayed;
  ou::db::QueryFields<SymbolDatesSearch>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
    = db.SQL<SymbolDatesSearch>( "select * from " + db::table::Dividend::c_TableName, key ).Where( "symbol_name=? and date_exdividend=? and date_payed=?" ).NoExecute();
  //ou::db::QueryFields<SymbolDatesSearch>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
  //  = db.SQL<SymbolDatesSearch>( "select * from " + db::table::Dividend::c_TableName, key ).Where( "symbol_name=? and date_exdividend=?" ).NoExecute();
  db.Bind<SymbolDatesSearch>( pExistsQuery );
  if ( db.Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
    db.Columns<SymbolDatesSearch, db::table::Dividend::TableRowDef>( pExistsQuery, row );
    bFound = true;
  }
  return bFound;
}

int main( int argc, char* argv[] ) {

  const static std::string sBaseName(       "dividend" );
  const static std::string sConfigFileName( sBaseName + ".cfg" );
  const static std::string sDbFileName(     sBaseName + ".db"  );

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

  if ( false ) { // migrate old table content into new tables, one time op

    // mnaual changes:
    /*
    alter table dividend rename to daily_dividend;
    CREATE TABLE daily (symbol_name TEXT NOT NULL, date_run TEXT NOT NULL, last_trade DOUBLE NOT NULL, yield DOUBLE NOT NULL, average_volume BIGINT NOT NULL, shares_outstanding BIGINT NOT NULL,  CONSTRAINT PK_daily PRIMARY KEY (symbol_name, date_run));
    CREATE TABLE dividend (symbol_name TEXT NOT NULL, date_run TEXT NOT NULL, date_exdividend TEXT NOT NULL, amount_payed DOUBLE NOT NULL, date_payed TEXT NOT NULL, rate DOUBLE NOT NULL,  CONSTRAINT PK_dividend PRIMARY KEY (symbol_name, date_exdividend));
    */

    db::table::DividendDaily::TableRowDef row;
    ou::db::QueryFields<ou::db::NoBind>::pQueryFields_t pSelect
      = db.SQL<ou::db::NoBind>( "select * from " + db::table::DividendDaily::c_TableName ).NoExecute();
    // this doesn't work properly with 0 records being returned
    while ( db.Execute( pSelect ) ) {
      db.Columns<ou::db::NoBind,  db::table::DividendDaily::TableRowDef>( pSelect, row );
      //std::cout << row.sSymbol << ", " << row.dateRun << std::endl;

      db::table::Daily::TableRowDef trdDaily(
        row.sSymbol
      , row.dateRun
      , row.dblLastTrade
      , row.dblYield
      , row.nAverageVolume
      , row.nSharesOutstanding
      );

      ou::db::QueryFields<db::table::Daily::TableRowDef>::pQueryFields_t pInsertDaily
        = db.Insert<db::table::Daily::TableRowDef>( const_cast<db::table::Daily::TableRowDef&>( trdDaily ) );

      db::table::Dividend::TableRowDef trdDividend_exists;
      if ( SymbolExDividendExists( db, row.sSymbol, row.dateExDividend, row.datePayed, trdDividend_exists ) ) {} // do nothing
      else {

        db::table::Dividend::TableRowDef trdDividend(
          row.sSymbol
        , row.dateRun
        , row.dateExDividend
        , row.dblAmountPayed
        , row.datePayed
        , row.dblRate
        );

        ou::db::QueryFields<db::table::Dividend::TableRowDef>::pQueryFields_t pInsertDividend
          = db.Insert<db::table::Dividend::TableRowDef>( const_cast<db::table::Dividend::TableRowDef&>( trdDividend ) );

      }
    }

    pSelect.reset();

    db.Close();
    db.OnRegisterRows.Remove( &HandleRegisterRows );
    db.OnRegisterTables.Remove( &HandleRegisterTables );

    // force exit when done
    return EXIT_SUCCESS;
  }


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

      // Table Symbol
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

      //BOOST_LOG_TRIVIAL(trace)
      //  << "trdDaily: "
      //  << vt.sSymbol
      //   << ",dr=" << today
      //   << ','
      //  ;

      // Table Daily
      db::table::Daily::TableRowDef trdDaily(
        vt.sSymbol
      , today
      , vt.trade
      , vt.yield
      , vt.nAverageVolume
      , vt.nSharesOutstanding
      );

      ou::db::QueryFields<db::table::Daily::TableRowDef>::pQueryFields_t pQuery
        = db.Insert<db::table::Daily::TableRowDef>( const_cast<db::table::Daily::TableRowDef&>( trdDaily ) );

      // Table Dividend
      db::table::Dividend::TableRowDef trdDividend_exists;
      if ( SymbolExDividendExists( db, vt.sSymbol, vt.dateExDividend, vt.datePayed, trdDividend_exists ) ) {} // do nothing
      else {

        //BOOST_LOG_TRIVIAL(trace)
        //  << "trdDividend: "
        //  << vt.sSymbol
        //  << ",dr=" << today
        //  << ",ex=" << vt.dateExDividend
        //  << ",dp=" << vt.datePayed
        //  ;

        db::table::Dividend::TableRowDef trdDividend(
          vt.sSymbol
        , today
        , vt.dateExDividend
        , vt.amount
        , vt.datePayed
        , vt.rate
        );

        ou::db::QueryFields<db::table::Dividend::TableRowDef>::pQueryFields_t pQuery
          = db.Insert<db::table::Dividend::TableRowDef>( const_cast<db::table::Dividend::TableRowDef&>( trdDividend ) );
      }

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
