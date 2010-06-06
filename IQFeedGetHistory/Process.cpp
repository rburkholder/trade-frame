/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <algorithm>

#include <boost/foreach.hpp>

#include "Process.h"
#include <LibIndicators/Darvas.h>

CProcess::CProcess(void)
: CIQFeedHistoryBulkQuery<CProcess>(),
  m_cntBars( 125 )
{
  m_vExchanges.push_back( "NYSE" );
  m_vExchanges.push_back( "NYSE_AMEX" );
  m_vExchanges.push_back( "NYSE,ARCA" );
  m_vExchanges.push_back( "NASDAQ" );
  m_vExchanges.push_back( "NASDAQ,NMS" );
  //m_vExchanges.push_back( "NASDAQ,SMCAP" );
  //m_vExchanges.push_back( "NASDAQ,OTCBB" );
  //m_vExchanges.push_back( "NASDAQ,OTC" );
}

CProcess::~CProcess(void) {
}

void CProcess::Start( void ) {

  SetExchanges( m_vExchanges );
  SetMaxSimultaneousQueries( 100 );
  DailyBars( m_cntBars );

}

void CProcess::OnBars( inherited_t::structResultBar* bars ) {

  std::stringstream ss;
  ss << "Bars for " + bars->sSymbol + ": ";

  double dblHigh = 0;
  size_t ix = 0;

  // look for index of high
  if ( m_cntBars == bars->bars.Size() ) {  // we have our bar count, so perform calc
    for ( size_t i = 0; i < bars->bars.Size(); ++i ) {
      if ( dblHigh < bars->bars[ i ]->High() ) {
        ix = i;
        dblHigh = bars->bars[ i ]->High();
      }
    }
  }

  if ( ix > ( m_cntBars - 20 ) ) {  // if high is in last 20 days, then can push through Darvas
    CDarvas darvas;
    double dblBuy = 0;
    double dblStop = 0;
    size_t ixBuy = 0;
    for ( size_t i = ( m_cntBars - 20 ); i < m_cntBars; ++i ) {
      darvas.Calc( *(bars->bars[ i ]));
      if ( darvas.SignalBuy() ) {
        ixBuy = i;
        dblBuy = bars->bars[ i ]->Close();
        dblStop = darvas.StopLevel();
      }
    }

    // emit darvas signal
    if ( ixBuy == ( m_cntBars - 1 ) ) {
      ss << " Darvas stop " << dblBuy;
      ss << std::endl;
      OutputDebugString( ss.str().c_str() );
    }
  }

  ReQueueBars( bars ); 

}

void CProcess::OnTicks( inherited_t::structResultTicks* ticks ) {
  ReQueueTicks( ticks ); 
//  OutputDebugString( "ticks for " + ticks->sSymbol + " done\n" );
}

void CProcess::OnCompletion( void ) {
  OutputDebugString( "all processing complete\n" );
}

