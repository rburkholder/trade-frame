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

// CAV.cpp : Defines the entry point for the application.
//

#include "StdAfx.h"

#include <iostream>
#include <set>

#include <TFIQFeed/LoadMktSymbols.h>

#include "Worker.h"
#include "SymbolSelection.h"

Worker::Worker(void): m_thread( boost::ref( *this ) ) {
}

Worker::~Worker(void) {
}

void Worker::operator()( void ) {
  std::cout << "running" << std::endl;

  typedef ou::tf::iqfeed::InMemoryMktSymbolList InMemoryMktSymbolList;
  InMemoryMktSymbolList list;
//  ou::tf::iqfeed::LoadMktSymbols( list, ou::tf::iqfeed::MktSymbolLoadType::Download, true );
//  ou::tf::iqfeed::LoadMktSymbols( list, ou::tf::iqfeed::MktSymbolLoadType::LoadTextFromDisk, false );
//  symbols.SaveToFile( "symbols.ser" );
  try {
    std::cout << "Loading serialized symbols ... ";
    list.LoadFromFile( "symbols.ser" );
    std::cout << "done." << std::endl;
  }
  catch (...) {
    std::cout << "ouch" << std::endl;
  }

  typedef std::vector<std::string> vStrings_t;
  std::vector<std::string> vExchanges;
  vExchanges.push_back( "NYSE" );
  vExchanges.push_back( "NGSM" );

  std::set<std::string> setSelected;

  size_t total( 0 );
  typedef InMemoryMktSymbolList::symbols_t symbols_t;
  typedef symbols_t::index<InMemoryMktSymbolList::ixExchange>::type SymbolsByExchange_t;
  SymbolsByExchange_t::iterator endSymbols = list.m_symbols.get<InMemoryMktSymbolList::ixExchange>().end();
  for ( vStrings_t::iterator iterExchanges = vExchanges.begin(); vExchanges.end() != iterExchanges; iterExchanges++ ) {
    size_t cnt( 0 );
    SymbolsByExchange_t::iterator iterSymbols = list.m_symbols.get<InMemoryMktSymbolList::ixExchange>().find( *iterExchanges );
    while ( endSymbols != iterSymbols ) {
      if ( *iterExchanges != iterSymbols->sExchange ) break;
      if ( ou::tf::iqfeed::MarketSymbol::Equity == iterSymbols->sc ) {
        if ( iterSymbols->bHasOptions ) {
/*          std::cout 
            << iterSymbols->sSymbol << ", " 
            << iterSymbols->nNAICS << ", " 
            << iterSymbols->sExchange << ", " 
            << iterSymbols->sListedMarket 
            << std::endl; */
          setSelected.insert( iterSymbols->sSymbol ); 
          cnt++;
        }
      }
      iterSymbols++;
    }
    std::cout << "Count for " << *iterExchanges << ": " << cnt << std::endl;
    total += cnt;
  }
  std::cout << "Total for exchanges: " << total << std::endl;

  SymbolSelection selection( 30 );
  selection.SetSymbols( setSelected.begin(), setSelected.end() );
  selection.DailyBars( 30 );
  selection.Block();

  std::cout << "History Downloaded" << std::endl;
 
}