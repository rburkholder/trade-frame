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

  typedef std::set<std::string> vStrings_t;
  vStrings_t vExchanges;
  vExchanges.insert( "NYSE" );
  vExchanges.insert( "NGSM" );

  typedef std::set<std::string> SymbolList_t;
  SymbolList_t setSelected;

  struct SelectSymbols {
    SelectSymbols( SymbolList_t& set ): m_selected( set ) {  };
    SymbolList_t& m_selected;
    void operator() ( ou::tf::iqfeed::InMemoryMktSymbolList::trd_t& trd ) {
      if ( ou::tf::iqfeed::MarketSymbol::Equity == trd.sc ) {
        if ( trd.bHasOptions ) {
          m_selected.insert( trd.sSymbol );
/*          std::cout 
            << iterSymbols->sSymbol << ", " 
            << iterSymbols->nNAICS << ", " 
            << iterSymbols->sExchange << ", " 
            << iterSymbols->sListedMarket 
            << std::endl; */
        }
      }
    }
  };

  list.SelectSymbolsByExchange( vExchanges.begin(), vExchanges.end(), SelectSymbols( setSelected ) );
  std::cout << "# symbols selected: " << setSelected.size() << std::endl;

  SymbolSelection selection( 30 );
  selection.SetSymbols( setSelected.begin(), setSelected.end() );
  selection.DailyBars( 30 );
  selection.Block();

  std::cout << "History Downloaded" << std::endl;
 
}