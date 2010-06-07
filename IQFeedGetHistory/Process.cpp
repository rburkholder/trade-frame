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

#include <LibIndicators/Darvas.h>

#include "Process.h"

// 
// CProcessDarvas
//

class CProcessDarvas: public CDarvas<CProcessDarvas> {
  friend CDarvas<CProcess>;
public:
  CProcessDarvas( void );
  ~CProcessDarvas( void ) {};
  void Calc( void );
  const std::string& Result( void ) { 
    // emit darvas signal
    if ( m_bTriggered ) {
      m_ss << " stop(" << m_dblStop << ")";
      m_ss << std::endl;
      //OutputDebugString( m_ss.str().c_str() );
      append( m_ss.str() );
    }
    return m_ss.str(); 
  };
protected:
  // CRTP from CDarvas<CProcess>
//  void ConservativeTrigger( void ) {};
  void AggressiveTrigger( void );
  void SetStop( double stop ) { m_dblStop = stop; };
//  void StopTrigger( void ) {};
  void BreakOutAlert( size_t );

private:
  size_t m_cntBars;

  double m_dblStop;

  std::stringstream m_ss;

  static const size_t m_BarWindow = 20;  // number of bars to examine
  size_t m_ixRelative;
  bool m_bTriggered;

};

void CProcessDarvas::CProcessDarvas( void ) 
: CDarvas<CProcessDarvas>()
{
    m_bTriggered = false;
    double dblBuy = 0;
    double dblStop = 0;
    size_t ixBuy = 0;
    size_t ix = m_cntBars - m_BarWindow;
    m_ixRelative = m_BarWindow;
    //for ( size_t i = ( m_cntBars - 20 ); i < m_cntBars; ++i ) {
}

void CProcessDarvas::Calc( const CBar& bar ) {
  CDarvas<CProcessDarvas>::Calc( bar );
  --m_ixRelative;

}

void CProcessDarvas::AggressiveTrigger( void ) {
  std::stringstream m_ss;
  m_ss << " AT(" << m_ixRelative << ")";
  if ( 1 == m_ixRelative ) {
    m_bTriggered = true;
  }
}

void CProcessDarvas::BreakOutAlert( size_t cnt ) {
  std::stringstream m_ss;
  m_ss << " BO(" << m_ixRelative << ")";
  if ( 1 == m_ixRelative ) {
    m_bTriggered = true;
  }
}


//
// CProcess
//

CProcess::CProcess(void)
: CIQFeedHistoryBulkQuery<CProcess>(), 
  m_cntBars( 125 ), m_b( false )
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

void CProcess::OnBars( inherited_t::structResultBar* bars ) {

  // warning:  this section is re-entrant from multiple threads

  std::stringstream m_ss;
  //m_ss.str(std::string());
  
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

  if ( ixHigh > ( m_cntBars - 20 ) ) {  // if high is in last 20 days, then can push bars through Darvas
    CProcessDarvas darvas;
    bool bTrigger;  // wait for trigger on final day
    for ( CBars::const_iterator iter = bars->bars.at( ix ); iter != bars->bars.end(); ++iter ) {
      bTrigger = CProcess::Calc( *iter );
    }

    if ( bTrigger ) {
      // output results
    }

  }

//  m_ss << std::endl;
//  OutputDebugString( m_ss.str().c_str() );

  ReQueueBars( bars ); 

}

void CProcess::OnTicks( inherited_t::structResultTicks* ticks ) {
  ReQueueTicks( ticks ); 
//  OutputDebugString( "ticks for " + ticks->sSymbol + " done\n" );
}

void CProcess::OnCompletion( void ) {
  OutputDebugString( m_s.c_str() );
  OutputDebugString( "all processing complete\n" );
}

