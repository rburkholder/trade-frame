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

#include <boost/ref.hpp>

//#include <TFIQFeed/LoadMktSymbols.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>

#include "Worker.h"

#include "SymbolSelection.h"

Worker::Worker(void): m_thread( boost::ref( *this ) ) {
}

Worker::~Worker(void) {
}

void Worker::operator()( void ) {

  std::cout << "running" << std::endl;

  ou::tf::HDF5IterateGroups groups;
  groups.SetOnHandleObject( MakeDelegate( this, &Worker::ProcessGroupItem ) );
  try {
    int result = groups.Start( "/bar/86400/" );
  }
  catch (...) {
    std::cout << "ouch" << std::endl;
  }


  /*
  SymbolSelection selection( 30 );
  selection.SetSymbols( setSelected.begin(), setSelected.end() );
  selection.DailyBars( 30 );
  selection.Block();
  */

  std::cout << "History Scanned." << std::endl;
 
}

void Worker::ProcessGroupItem( const std::string& sObjectPath, const std::string& sObjectName ) {
  std::cout << sObjectName << std::endl;
}