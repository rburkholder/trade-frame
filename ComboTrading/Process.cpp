/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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

// started 2015/11/11

#include <iostream>
#include <algorithm>

#include "Process.h"

Process::Process() {
}

Process::~Process() {
}



void Process::LoadWeeklies( void ) {
  
  bool bOk( true );
  try {
    ou::tf::cboe::ReadCboeWeeklyOptions( m_cboeExpiries, m_cboeVui );
  }
  catch(...) {
    bOk = false;
    std::cout << "error loading weeklies" << std::endl;
  }
  
  std::cout << "LoadWeeklies done." << std::endl;

  if ( bOk ) {
    std::sort( m_cboeVui.begin(), m_cboeVui.end() );
    for ( ou::tf::cboe::vUnderlyinginfo_t::const_iterator iter = m_cboeVui.begin(); m_cboeVui.end() != iter; ++iter ) {
      if ( ( "Equity" == iter->sProductType ) || ( "ETF" == iter->sProductType ) ) {
	std::cout 
		<< iter->sSymbol 
		<< "," << iter->sProductType // "Equity", "ETF"
		<< "," << iter->sDescription
		<< "," << iter->bStandardWeekly
		<< "," << iter->bExpandedWeekly
		<< std::endl;
      }
    }
  }
  
  
}