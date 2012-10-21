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

#include <TFIQFeed/LoadMktSymbols.h>

#include "Worker.h"

Worker::Worker(void): m_thread( boost::ref( *this ) ) {
}

Worker::~Worker(void) {
}

void Worker::operator()( void ) {
  std::cout << "running" << std::endl;

  ou::tf::iqfeed::symbols_t symbols;
//  ou::tf::iqfeed::LoadMktSymbols( symbols, ou::tf::iqfeed::MktSymbolLoadType::Download, true );
//  ou::tf::iqfeed::LoadMktSymbols( symbols, ou::tf::iqfeed::MktSymbolLoadType::LoadTextFromDisk, false );
//  symbols.SaveToFile( "symbols.ser" );
  try {
    std::cout << "Loading serialized symbols ... ";
    symbols.LoadFromFile( "symbols.ser" );
    std::cout << "done." << std::endl;
  }
  catch (...) {
    std::cout << "ouch" << std::endl;
  }

}