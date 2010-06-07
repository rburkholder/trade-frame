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

CProcess::CProcess(void)
: CIQFeedHistoryBulkQuery<CProcess>(), 
  CDarvas<CProcess>(),
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
  SetMaxSimultaneousQueries( 15 );
  DailyBars( m_cntBars );

}

void CProcess::AggressiveTrigger( void ) {
  m_bTriggered = true;
  m_ss << " AT(" << m_ixRelative << ")";
}

void CProcess::BreakOutAlert( size_t cnt ) {
  m_bTriggered = true;
  m_ss << " BO(" << m_ixRelative << ")";
}

void CProcess::OnBars( inherited_t::structResultBar* bars ) {

  m_ss.str() = "";
  
  m_ss << "Bars for " << bars->sSymbol << ": ";

  double dblHigh = 0;
  size_t ixHigh = 0;

  // look for index of high
  if ( m_cntBars == bars->bars.Size() ) {  // we have our bar count, so perform calc
    size_t ix = 0;
    for ( CBars::const_iterator iter = bars->bars.begin(); iter != bars->bars.end(); ++iter ) {
      if ( dblHigh < (*iter).High() ) {
        ixHigh = ix;
        dblHigh = (*iter).High();
      }
      ++ix;
    }
  }

  if ( ixHigh > ( m_cntBars - 20 ) ) {  // if high is in last 20 days, then can push through Darvas
    CDarvas<CProcess>::Clear();
    m_bTriggered = false;
    double dblBuy = 0;
    double dblStop = 0;
    size_t ixBuy = 0;
    size_t ix = m_cntBars - m_BarWindow;
    m_ixRelative = m_BarWindow;
    //for ( size_t i = ( m_cntBars - 20 ); i < m_cntBars; ++i ) {
    for ( CBars::const_iterator iter = bars->bars.at( ix ); iter != bars->bars.end(); ++iter ) {
      CDarvas<CProcess>::Calc( *iter );
//      if ( darvas.SignalBuy() ) {
//        ixBuy = ix;
//        dblBuy = (*iter).Close();
//        dblStop = darvas.StopLevel();
//      }
//      ++ix;
      --m_ixRelative;
    }

    // emit darvas signal
    if ( m_bTriggered ) {
      m_ss << " stop(" << m_dblStop << ")";
    }
  }

  m_ss << std::endl;
  OutputDebugString( m_ss.str().c_str() );

  ReQueueBars( bars ); 

}

void CProcess::OnTicks( inherited_t::structResultTicks* ticks ) {
  ReQueueTicks( ticks ); 
//  OutputDebugString( "ticks for " + ticks->sSymbol + " done\n" );
}

void CProcess::OnCompletion( void ) {
  OutputDebugString( "all processing complete\n" );
}

