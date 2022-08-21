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

#include <OUCommon/KeyWordMatch.h>

#include <TFTrading/TradingEnumerations.h>

#include <TFIQFeed/InMemoryMktSymbolList.h>

#include "Process.hpp"

int main( int argc, char* argv[] ) {

  ou::tf::iqfeed::InMemoryMktSymbolList list;

  std::cout << "loading started ..." << std::endl;

  list.LoadFromFile( "../symbols.ser" );

  std::cout << "loading done" << std::endl;

  using vExchanges_t = std::vector<std::string>;
  vExchanges_t vExchanges;
  vExchanges = {
    "NYSE"
  , "NYSE,NYSE_ARCA"
  , "NYSE_AMERICAN"
  , "NASDAQ"
  , "NASDAQ,NCM"
  , "NASDAQ,NGM"
  , "NASDAQ,NGSM"
  };

  using dividend_t = Process::dividend_t;
  using vSymbols_t = Process::vSymbols_t;
  vSymbols_t vSymbols;

  ou::KeyWordMatch<bool> kwm( true, 20 ); // scan can't find these
  kwm.AddPattern( "BSCE", false );
  kwm.AddPattern( "DBBPF", false );
  kwm.AddPattern( "ELMSW", false );
  kwm.AddPattern( "ELSBF", false );
  kwm.AddPattern( "GLMGF", false );
  kwm.AddPattern( "GRZZU", false );
  kwm.AddPattern( "HUICF", false );
  kwm.AddPattern( "LIBC", false );
  kwm.AddPattern( "LYPHF", false );
  kwm.AddPattern( "ORPH", false );
  kwm.AddPattern( "PB", false );
  kwm.AddPattern( "PFH", false ); // this one works, bug somewhere?
  kwm.AddPattern( "PGTRF", false );
  kwm.AddPattern( "RTWRF", false );
  kwm.AddPattern( "SSDT", false );
  kwm.AddPattern( "SBEAU", false );
  kwm.AddPattern( "UELMO", false );

  list.SelectSymbolsByExchange(
    vExchanges.begin(), vExchanges.end(),
    [&vSymbols,&kwm](const ou::tf::iqfeed::InMemoryMktSymbolList::trd_t trd){
      if ( ou::tf::iqfeed::ESecurityType::Equity == trd.sc ) {
        if ( kwm.FindMatch( trd.sSymbol ) ) { // note the reverse logic in kwm
          //std::cout << trd.sSymbol << std::endl;
          vSymbols.push_back( dividend_t( trd.sSymbol ) );
        }
      }
    }
    );

  std::cout << "#symbols=" << vSymbols.size() << std::endl;

  Process process( vSymbols );
  process.Wait();

  std::cout
    << "exchange,symbol,yield,rate,amount,vol,exdiv,payed,option"
    << std::endl;

  for ( vSymbols_t::value_type& vt: vSymbols ) {
    if ( ( 7.0 < vt.yield ) && ( 5000 <= vt.nAverageVolume ) ) {
      std::cout
               << vt.sSymbol
        << "," << vt.sExchange
        << "," << vt.yield
        << "," << vt.rate
        << "," << vt.amount
        << "," << vt.nAverageVolume
        << "," << vt.dateExDividend
        << "," << vt.datePayed
        << "," << vt.sOptionRoots
        << std::endl;
    }
  }

  return 0;
}

// https://dividendhistory.org
// https://www.barchart.com
// https://www.dividend.com

// https://www.nasdaq.com/market-activity/funds-and-etfs/xxxx/dividend-history
