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

#include "Config.hpp"
#include "DbRecordDividend.hpp"
#include "Process.hpp"

const static std::string sTableName_Dividend( "dividend" );

void HandleRegisterTables( ou::db::Session& session ) {
  // called when db created
  //std::cout << "HandleRegisterTables placeholder" << std::endl;
  session.RegisterTable<db::record::Dividend::TableCreateDef>( sTableName_Dividend );
}

void HandleRegisterRows( ou::db::Session& session ) {
  // called when db created and when exists
  //std::cout << "HandleRegisterRows placeholder" << std::endl;
  session.MapRowDefToTableName<db::record::Dividend::TableRowDef>( sTableName_Dividend );
}

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

      db::record::Dividend::TableRowDef trd(
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

      ou::db::QueryFields<db::record::Dividend::TableRowDef>::pQueryFields_t pQuery
        = db.Insert<db::record::Dividend::TableRowDef>( const_cast<db::record::Dividend::TableRowDef&>( trd ) );

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
