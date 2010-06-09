/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "IQFeedHistoryCollector.h"

#include <cassert>

#include <LibHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <LibHDF5TimeSeries/HDF5DataManager.h>

//
// CHistoryCollector
// 

CHistoryCollector::CHistoryCollector( CIQFeedProvider *pProvider, const char *szSymbol, unsigned long nCount ) 
: m_pProvider( pProvider ) 
{
  m_sSymbol.assign( szSymbol );
  m_nCount = nCount;
}

CHistoryCollector::~CHistoryCollector() {
}

void CHistoryCollector::Start( void ) {
  // m_phistory will be assigned by sub classes
  assert( NULL != m_phistory );
  m_phistory->LiveRequest( m_sSymbol.c_str(), m_nCount );
}

void CHistoryCollector::OnCompletion( IQFeedHistory *pHistory ) {
  if ( NULL != OnRetrievalComplete ) OnRetrievalComplete( this );
}

//
// CHistoryCollectorDaily
//

CHistoryCollectorDaily::CHistoryCollectorDaily( CIQFeedProvider *pProvider, const char *szSymbol, unsigned long nCount ) :
  CHistoryCollector( pProvider, szSymbol, nCount ) {
  m_phistory = new IQFeedHistoryHD( m_pProvider, &m_bars );
  FinalizeCreation();
}

CHistoryCollectorDaily::~CHistoryCollectorDaily( void ) {
  delete m_phistory;
}

void CHistoryCollectorDaily::Start( void ) {
  m_bars.Clear();
  CHistoryCollector::Start();
}

void CHistoryCollectorDaily::WriteData( void ) {
  if ( 0 != m_bars.Count() ) {

    assert( m_sSymbol.length() > 0 );

    std::string sPathName;
    CHDF5DataManager::DailyBarPath( m_sSymbol, sPathName );

    CHDF5WriteTimeSeries<CBars, CBar> wts;
    wts.Write( sPathName, &m_bars );
  }
}

//
// CHistoryCollectorTicks
//

CHistoryCollectorTicks::CHistoryCollectorTicks( CIQFeedProvider *pProvider, const char *szSymbol, unsigned long nCount ) :
  CHistoryCollector( pProvider, szSymbol, nCount ) {
  m_phistory = new IQFeedHistoryHT( m_pProvider, &m_quotes, &m_trades );
  FinalizeCreation();
}

CHistoryCollectorTicks::~CHistoryCollectorTicks( void ) {
  delete m_phistory;
}

void CHistoryCollectorTicks::Start( void ) {
  m_trades.Clear();
  m_quotes.Clear();
  CHistoryCollector::Start();
}

void CHistoryCollectorTicks::WriteData( void ) {

  assert( m_sSymbol.length() > 0 );

  string sPathName;

  if ( 0 != m_trades.Count() ) {
    sPathName = "/trade/" + m_sSymbol;
    CHDF5WriteTimeSeries<CTrades, CTrade> wtst;
    wtst.Write( sPathName, &m_trades );
  }

  if ( 0 != m_quotes.Count() ) {
    sPathName = "/quote/" + m_sSymbol;
    CHDF5WriteTimeSeries<CQuotes, CQuote> wtsq;
    wtsq.Write( sPathName, &m_quotes );
  }
}

