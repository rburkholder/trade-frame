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

#include <set>
#include <string>

#include <boost/ref.hpp>

#include <OUCommon/TimeSource.h>

#include "Worker.h"

#include "SymbolSelection.h"

Worker::Worker(void) {
  m_pThread = new boost::thread( boost::ref( *this ) );
}

Worker::~Worker(void) {
  delete m_pThread; 
}

void Worker::operator()( void ) {

  std::set<std::string> symbols;

  // last day of available data
  SymbolSelection selector( ptime( date( 2012, 12, 7 ), time_duration( 0, 0, 0 ) ) );
  selector.Process( symbols );

  std::stringstream ss;
  ss.str( "" );
  ss << ou::TimeSource::Instance().Internal();
  m_sTSDataStreamStarted = "/app/OverUnderConsole/" + ss.str();  // will need to make this generic if need some for multiple providers.

  std::cout << "Symbol List: " << std::endl;
  for ( std::set<std::string>::const_iterator iter = symbols.begin(); iter != symbols.end(); iter++ ) {
    std::cout << *iter << std::endl;
  }


}

