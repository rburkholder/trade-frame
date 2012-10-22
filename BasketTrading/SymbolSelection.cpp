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

#include "StdAfx.h"

#include "SymbolSelection.h"

SymbolSelection::SymbolSelection( size_t nBarsExpected ): 
  ou::tf::iqfeed::HistoryBulkQuery<SymbolSelection>(),
  m_nBarsExpected( nBarsExpected )
  {
}

SymbolSelection::~SymbolSelection(void) {
}

void SymbolSelection::OnHistoryRequestDone( structQueryState* pqs ) {

  struct CalcAverageVolume {
    typedef size_t return_type;
    CalcAverageVolume( void ): m_totalVolume( 0 ), m_nValues( 0 ) {};
    void operator()( const ou::tf::Bar& bar ) {
      m_totalVolume += bar.Volume();
      m_nValues++;
    }
    operator return_type() { return m_totalVolume / m_nValues; };
  private:
    size_t m_totalVolume;
    size_t m_nValues;
  };

  boost::mutex::scoped_lock lock( m_mutexProcessBarsScopeLock );
  ou::tf::Bars& bars( pqs->bars->bars );
  if ( m_nBarsExpected == bars.Size() ) {
    size_t avgVolume = bars.ForEach( CalcAverageVolume() );
    if ( 1000000 < avgVolume ) {
    }
  }
}

void SymbolSelection::OnCompletion( void ) {
}