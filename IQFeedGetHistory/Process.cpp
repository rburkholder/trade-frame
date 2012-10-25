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

#include <TFIndicators/Darvas.h>

#include <TFIQFeed/IQFeedHistoryBulkQuery.h>
#include <TFIQFeed/LoadMktSymbols.h>

#include "Process.h"

// 
// CProcessDarvas
//

class CProcessDarvas: public ou::tf::CDarvas<CProcessDarvas> {
  friend ou::tf::CDarvas<CProcessDarvas>;
public:
  CProcessDarvas( size_t ix );
  ~CProcessDarvas( void ) {};
  bool Calc( const ou::tf::Bar& );
  void Result( std::string& s );  // should only be called once
protected:
  // CRTP from CDarvas<CProcess>
  void ConservativeTrigger( void );
  void AggressiveTrigger( void );
  void SetStop( double stop ) { m_dblStop = stop; };
//  void StopTrigger( void ) {};
  void BreakOutAlert( size_t );

private:

  std::stringstream m_ss;

  size_t m_ix; // keeps track of index of trigger bar
  bool m_bTriggered;  // set when last bar has trigger
  double m_dblStop;

};

CProcessDarvas::CProcessDarvas( size_t ix ) 
: ou::tf::CDarvas<CProcessDarvas>(), 
  m_bTriggered( false ), m_dblStop( 0 ), m_ix( ix )
{
}

bool CProcessDarvas::Calc( const ou::tf::Bar& bar ) {
  ou::tf::CDarvas<CProcessDarvas>::Calc( bar );
  --m_ix;
  bool b = m_bTriggered; 
  m_bTriggered = false; 
  return b;
}

void CProcessDarvas::ConservativeTrigger( void ) {
  m_ss << " CT(" << m_ix << ")";
  m_bTriggered = true;
}

void CProcessDarvas::AggressiveTrigger( void ) {
  m_ss << " AT(" << m_ix << ")";
  m_bTriggered = true;
}

void CProcessDarvas::BreakOutAlert( size_t cnt ) {
  m_ss << " BO(" << m_ix << ")";
  m_bTriggered = true;
}

void CProcessDarvas::Result( std::string& s ) {
  m_ss << " stop(" << m_dblStop << ")";
  s = m_ss.str();
}


//
// CProcess
//

CProcess::CProcess(void)
: ou::tf::iqfeed::HistoryBulkQuery<CProcess>(), 
  m_cntBars( 200 )
{
  m_vExchanges.insert( "NYSE" );
  //m_vExchanges.push_back( "NYSE_AMEX" );
  //m_vExchanges.push_back( "NYSE,ARCA" );
  m_vExchanges.insert( "NGSM" );
  //m_vExchanges.push_back( "NASDAQ,NMS" );
  //m_vExchanges.push_back( "NASDAQ,SMCAP" );
  //m_vExchanges.push_back( "NASDAQ,OTCBB" );
  //m_vExchanges.push_back( "NASDAQ,OTC" );
}

CProcess::~CProcess(void) {
}

void CProcess::Start( void ) {

  ou::tf::iqfeed::InMemoryMktSymbolList list;
  std::cout << "Loading From File ... ";
  list.LoadFromFile( "symbols.ser" );
  std::cout << " done." << std::endl;

  typedef std::set<std::string> SymbolList_t;
  SymbolList_t setSelected;

  struct SelectSymbols {
    SelectSymbols( SymbolList_t& set ): m_selected( set ) {  };
    SymbolList_t& m_selected;
    void operator() ( const ou::tf::iqfeed::InMemoryMktSymbolList::trd_t& trd ) {
      if ( ou::tf::iqfeed::MarketSymbol::Equity == trd.sc ) {
        if ( trd.bHasOptions ) {
          m_selected.insert( trd.sSymbol );
        }
      }
    }
  };

  list.SelectSymbolsByExchange( m_vExchanges.begin(), m_vExchanges.end(), SelectSymbols( setSelected ) );
  std::cout << "# symbols selected: " << setSelected.size() << std::endl;

  SetMaxSimultaneousQueries( 15 );
  DailyBars( m_cntBars );

}

void CProcess::OnBars( inherited_t::structResultBar* bars ) {

  // warning:  this section is re-entrant from multiple threads

  std::string s;
  bool bEmit = false;
  
  s = "Bars for " +  bars->sSymbol + ": ";

  double dblHigh = 0;
  size_t ixHigh = 0;

  // look for index of high
  if ( m_cntBars == bars->bars.Size() ) {  // we have our bar count, so perform calc
    size_t ix = 0;
    for ( ou::tf::Bars::const_iterator iter = bars->bars.begin(); iter != bars->bars.end(); ++iter ) {
      if ( dblHigh < (*iter).High() ) {
        ixHigh = ix;
        dblHigh = (*iter).High();
      }
      ++ix;
    }
  }

  if ( ixHigh > ( m_cntBars - m_BarWindow ) ) {  // if high is in last n days, then can push bars through Darvas
    CProcessDarvas darvas( m_BarWindow );
    size_t ix = m_cntBars - m_BarWindow;
    bool bTrigger;  // wait for trigger on final day
    for ( ou::tf::Bars::const_iterator iter = bars->bars.at( ix ); iter != bars->bars.end(); ++iter ) {
      bTrigger = darvas.Calc( *iter );
    }

    if ( bTrigger ) {
      std::string ss;
      darvas.Result( ss );
      s += ss;
      bEmit = true;
    }

  }

  if ( bEmit ) {
    s += "\n";
    OutputDebugString( s.c_str() );
  }

  ReQueueBars( bars ); 

}

void CProcess::OnTicks( inherited_t::structResultTicks* ticks ) {
  ReQueueTicks( ticks ); 
}

void CProcess::OnCompletion( void ) {
  OutputDebugString( "all processing complete\n" );
}

