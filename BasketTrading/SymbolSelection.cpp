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

#include "stdafx.h"

#include <iostream>
#include <set>
#include <vector>
#include <math.h>

#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

#include <TFIndicators/Darvas.h>
#include <TFIndicators/Pivots.h>

#include "SymbolSelection.h"

SymbolSelection::SymbolSelection( ptime dtLast )
  : m_dtLast( dtLast ), m_dm( ou::tf::HDF5DataManager::RO ), m_nMinPivotBars( 20 )
{

  m_dtEnd = m_dtLast + date_duration( 1 );
  m_dtOneYearAgo = m_dtLast - date_duration( 52 * 7 );
  m_dt26WeeksAgo = m_dtLast - date_duration( 26 * 7 );
  m_dtDateOfFirstBar = m_dt26WeeksAgo;
  m_dtDarvasTrigger = m_dtLast - date_duration( 8 );

}

SymbolSelection::~SymbolSelection(void) {
}

void SymbolSelection::Process( setInstrumentInfo_t& selected ) {

  m_psetSymbols = &selected;

  std::cout << "Running" << std::endl;

  ou::tf::HDF5IterateGroups groups;

  groups.SetOnHandleObject( MakeDelegate( this, &SymbolSelection::ProcessGroupItem ) );
  try {
    int result = groups.Start( "/bar/86400/" );
  }
  catch (...) {
    std::cout << "ouch" << std::endl;
  }

//  WrapUp10Percent(selected);
//  WrapUpVolatility(selected);
//  WrapUpPivots(selected);

  std::cout << "History Scanned." << std::endl;

}

struct AverageVolume {
private:
  ou::tf::Bar::volume_t m_nTotalVolume;
  unsigned long m_nNumberOfValues;
protected:
public:
  AverageVolume() : m_nTotalVolume( 0 ), m_nNumberOfValues( 0 ) {};
  void operator() ( const ou::tf::Bar& bar ) {
    m_nTotalVolume += bar.Volume();
    ++m_nNumberOfValues;
  }
  operator ou::tf::Bar::volume_t() { return m_nTotalVolume / m_nNumberOfValues; };
};

class AveragePrice {
private:
  double m_dblSumOfPrices;
  size_t m_nNumberOfValues;
protected:
public:
  AveragePrice() : m_dblSumOfPrices( 0 ), m_nNumberOfValues( 0 ) {};
  void operator() ( const ou::tf::Bar& bar ) {
    m_dblSumOfPrices += bar.Close();
    ++m_nNumberOfValues;
  }
  operator double() { return m_dblSumOfPrices / m_nNumberOfValues; };
};

void SymbolSelection::ProcessGroupItem( const std::string& sObjectPath, const std::string& sObjectName ) {
//  std::cout << sObjectName << std::endl;
  ou::tf::HDF5TimeSeriesContainer<ou::tf::Bar> barRepository( m_dm, sObjectPath );
  ou::tf::HDF5TimeSeriesContainer<ou::tf::Bar>::iterator begin, end;
  begin = lower_bound( barRepository.begin(), barRepository.end(), m_dtDateOfFirstBar );
  end = lower_bound( begin, barRepository.end(), m_dtEnd ); 
  hsize_t cnt = end - begin;
  if ( m_nMinPivotBars <= cnt ) {
    ptime dttmp = (*(end-1)).DateTime();
//    std::cout << sObjectName << m_dtLast << ", " << dttmp << std::endl;
      ou::tf::Bars bars;
      bars.Resize( cnt );
      barRepository.Read( begin, end, &bars );
      ou::tf::Bars::const_iterator iterVolume = bars.end() - m_nMinPivotBars;
      ou::tf::Bar::volume_t volAverage = std::for_each( iterVolume, bars.end(), AverageVolume() );
      if ( ( 1000000 < volAverage ) 
        && ( 15.0 <= bars.Last()->Close() )
        && ( 90.0 >= bars.Last()->Close() ) 
        && ( m_dtLast.date() == bars.Last()->DateTime().date() )
        ) {
          InstrumentInfo ii( sObjectName, *bars.Last() );
//          if ( ( 120 < cnt ) && ( dttmp.date() == m_dtLast.date() ) ) {
//            CheckForDarvas( ii, bars.begin(), bars.end() );
//          }
          CheckFor10Percent( ii, bars.end() - 20, bars.end() );
//          CheckForVolatility( ii, bars.end() - 20, bars.end() );
//          CheckForPivots( ii, bars.end() - m_nMinPivotBars, bars.end() );
//          CheckForRange( ii, bars.end() - m_nMinPivotBars, bars.end() );
      }
  }
}

void SymbolSelection::CheckForRange( const InstrumentInfo& ii, citer begin, citer end ) {
  citer iter1( begin );
  int cnt( 0 );
  int cntAbove( 0 );
  int cntBelow( 0 );
  int cntRising( 0 );
  int cntFalling( 0 );
  double dblAbove( 0 );
  double dblBelow( 0 );
  double dblRange( 0 );
  while ( iter1 != end ) {
    dblRange += ( iter1->High() - iter1->Low() );
    if ( iter1->High() > iter1->Open() ) {
      ++cntAbove;
      dblAbove += ( iter1->High() - iter1->Open() );
    }
    if ( iter1->Low() < iter1->Open() ) {
      ++cntBelow;
      dblBelow += ( iter1->Open() - iter1->Low() );
    }
    if ( iter1->Open() > iter1->Close() ) ++cntFalling;
    if ( iter1->Open() < iter1->Close() ) ++cntRising;
    ++cnt;
    ++iter1;
  }

  double avgAbove = dblAbove / cntAbove;
  double avgBelow = dblBelow / cntBelow;

  int diffCnt = cntAbove - cntBelow;  // minimize this
  double dblRatioAboveBelow = 0.5 - ( avgAbove / ( avgAbove + avgBelow ) ); // minimize this

  //m_mapRangeRanking.insert( mapRangeRanking_t::value_type( diffCnt, ii ) );
  std::cout 
    << ii.sName << ","
    << diffCnt << ","
    << dblRatioAboveBelow << ","
    << avgAbove << ","
    << avgBelow << ","
//    << avgAbove + avgBelow << ","
    << dblRange
    << std::endl;
}

struct CalcMaxDate: public std::unary_function<ou::tf::Bar&, void> {
public:
  CalcMaxDate( void ) : dtMax( boost::date_time::special_values::min_date_time ), dblMax( 0 ) { };
  void operator() ( const ou::tf::Bar &bar ) {
    if ( bar.Close() >= dblMax ) {
      dblMax = bar.Close();
      dtMax = bar.DateTime();
    }
  }
  operator ptime() { return dtMax; };
private:
  ptime dtMax;
  double dblMax;
};

//
// ProcessPivots
//

// taken from scanner

void SymbolSelection::CheckForPivots( const InstrumentInfo& ii, citer begin, citer end ) {
  ou::tf::Bar::volume_t nAverageVolume = std::for_each( begin, end, AverageVolume() );
//  std::cout << sObject << ": " << bars.Last()->DateTime() << " - " << m_dtLast << std::endl;
//      Info info( sObjectName, *bars.Last() );
//      m_mapInfoRankedByVolume.insert( pairInfoRankedByVolume_t( volAverage, info ) );
      //std::cout << sObject << " vol=" << volAverage << std::endl;
  citer iter1, iter2;
  iter2 = end;
  iter1 = iter2 - m_nMinPivotBars;
  iter2 = iter1;
  ++iter2;

  size_t nPVCrossings( 0 );
  size_t nUpAndR1Crossings( 0 );
  size_t nDnAndS1Crossings( 0 );
  size_t nPVAndR1Crossings( 0 );
  size_t nPVAndS1Crossings( 0 );

  while ( end != iter2 ) {
    ou::tf::PivotSet pivot( "pv", *iter1 );
    double pv = pivot.GetPivotValue( ou::tf::PivotSet::PV );
    if ( ( pv <= iter2->High() ) && ( pv >= iter2->Low() ) ) {
      ++(nPVCrossings);
    }

    if ( iter2->Open() < pv ) {
      double r1 = pivot.GetPivotValue( ou::tf::PivotSet::R1 );
      if ( ( r1 <= iter2->High() ) && ( r1 >= iter2->Low() ) ) ++(nPVAndR1Crossings);
    }
    if ( iter2->Open() > pv ) {
      double s1 = pivot.GetPivotValue( ou::tf::PivotSet::S1 );
      if ( ( s1 <= iter2->High() ) && ( s1 >= iter2->Low() ) ) ++(nPVAndS1Crossings);
    }

    if ( iter2->Open() > pv ) {
      double r1 = pivot.GetPivotValue( ou::tf::PivotSet::R1 );
      if ( ( r1 <= iter2->High() ) && ( r1 >= iter2->Low() ) ) ++(nUpAndR1Crossings);
    }
    if ( iter2->Open() < pv ) {
      double s1 = pivot.GetPivotValue( ou::tf::PivotSet::S1 );
      if ( ( s1 <= iter2->High() ) && ( s1 >= iter2->Low() ) ) ++(nDnAndS1Crossings);
    }

    ++iter1;
    ++iter2;
  }

  boost::uint32_t ttlR1S1 = nUpAndR1Crossings + nDnAndS1Crossings;
  boost::uint32_t ttlR1PvS1 = nPVAndR1Crossings + nPVAndS1Crossings;
  boost::uint32_t ttlOutside = ttlR1S1 + ttlR1PvS1;
  boost::uint32_t rank = ( 100 * ( ( 100 * ttlOutside ) + ttlR1S1 ) ) + ttlR1PvS1;

  m_mapPivotRanking.insert( mapPivotRanking_t::value_type( rank, ii ) );

}

void SymbolSelection::WrapUpPivots( setInstrumentInfo_t& selected ) {
  int cnt( 35 );  // select 35 symbols ( => < 3% weighting by symbol )
  for ( mapPivotRanking_t::const_reverse_iterator iter = m_mapPivotRanking.rbegin(); iter!= m_mapPivotRanking.rend(); ++iter ) {
    selected.insert( iter->second );
    --cnt;
    if ( 0 == cnt ) break;
  }
  m_mapPivotRanking.clear();
}

// 
// ProcessDarvas
//

class ProcessDarvas: public ou::tf::Darvas<ProcessDarvas> {
  friend ou::tf::Darvas<ProcessDarvas>;
public:
  ProcessDarvas( std::stringstream& ss, size_t ix );
  ~ProcessDarvas( void ) {};
  bool Calc( const ou::tf::Bar& );
  void StopValue( void );  // should only be called once
protected:
  // CRTP from CDarvas<CProcess>
  void ConservativeTrigger( void );
  void AggressiveTrigger( void );
  void SetStop( double stop ) { m_dblStop = stop; };
//  void StopTrigger( void ) {};
  void BreakOutAlert( size_t );

private:

  std::stringstream& m_ss;

  size_t m_ix; // keeps track of index of trigger bar
  bool m_bTriggered;  // set when last bar has trigger
  double m_dblStop;

};

ProcessDarvas::ProcessDarvas( std::stringstream& ss, size_t ix ) 
: ou::tf::Darvas<ProcessDarvas>(), m_ss( ss ),
  m_bTriggered( false ), m_dblStop( 0 ), m_ix( ix )
{
}

bool ProcessDarvas::Calc( const ou::tf::Bar& bar ) {
  ou::tf::Darvas<ProcessDarvas>::Calc( bar );
  --m_ix;
  bool b = m_bTriggered; 
  m_bTriggered = false; 
  return b;
}

void ProcessDarvas::ConservativeTrigger( void ) {
  m_ss << " CT(" << m_ix << ")";
  m_bTriggered = true;
}

void ProcessDarvas::AggressiveTrigger( void ) {
  m_ss << " AT(" << m_ix << ")";
  m_bTriggered = true;
}

void ProcessDarvas::BreakOutAlert( size_t cnt ) {
  m_ss << " BO(" << m_ix << ")";
  m_bTriggered = true;
}

void ProcessDarvas::StopValue( void ) {
  m_ss << " stop(" << m_dblStop << ")";
}

void SymbolSelection::CheckForDarvas( const InstrumentInfo& ii, citer begin, citer end ) {
  size_t nTriggerWindow( 10 );
  ptime dtDayOfMax = std::for_each( begin, end, CalcMaxDate() );
  citer iterLast( end - 1 );
  if ( dtDayOfMax >= m_dtDarvasTrigger ) {
    std::stringstream ss;
    ss << "Darvas max for " << ii.sName 
      << " on " << dtDayOfMax 
      << ", close=" << iterLast->Close()
      << ", volume=" << iterLast->Volume();

    ProcessDarvas darvas( ss, nTriggerWindow );
    //size_t ix = end - nTriggerWindow;
    citer iterTriggerBegin = end - nTriggerWindow;
    bool bTrigger;  // wait for trigger on final day
    for ( citer iter = iterTriggerBegin; iter != end; ++iter ) {
      bTrigger = darvas.Calc( *iter );  // final day only is needed
    }

    if ( bTrigger ) {
      m_psetSymbols->insert( ii );
      darvas.StopValue();
      std::cout << ss.str() << std::endl;
    }

  }
}

void SymbolSelection::CheckFor10Percent( const InstrumentInfo& ii, citer begin, citer end ) {
  double dblAveragePrice = std::for_each( begin, end, AveragePrice() );
  citer iterLast( end - 1 );
  if ( 25.0 < dblAveragePrice ) {
    mapRankingPos_t::iterator iterPos;
    mapRankingNeg_t::iterator iterNeg;
    //double dblReturn = ( m_bars.Last()->m_dblClose - m_bars.Last()->m_dblOpen ) / m_bars.Last()->m_dblClose;
    double dblReturn = ( iterLast->Close() - iterLast->Open() ) / iterLast->Close();
    if ( m_nMaxInList > m_mapMaxPositives.size() ) {
      m_mapMaxPositives.insert( pairRanking_t( dblReturn, ii ) );  // need to insert the path instead
    }
    else {
      iterPos = m_mapMaxPositives.begin();
      if ( dblReturn > iterPos->first ) {
        m_mapMaxPositives.erase( iterPos );
        m_mapMaxPositives.insert( pairRanking_t( dblReturn, ii ) ); // need to insert the path instead
      }
    }
    if ( m_nMaxInList > m_mapMaxNegatives.size() ) {
      m_mapMaxNegatives.insert( pairRanking_t( dblReturn, ii ) );// need to insert the path instead
    }
    else {
      iterNeg = m_mapMaxNegatives.begin();
      if ( dblReturn < iterNeg->first ) {
        m_mapMaxNegatives.erase( iterNeg );
        m_mapMaxNegatives.insert( pairRanking_t( dblReturn, ii ) );// need to insert the path instead
      }
    }
  }
}

void SymbolSelection::WrapUp10Percent( setInstrumentInfo_t& selected ) {
  std::cout << "Positives: " << std::endl;
  mapRankingPos_t::iterator iterPos;
  for ( iterPos = m_mapMaxPositives.begin(); iterPos != m_mapMaxPositives.end(); ++iterPos ) {
//    size_t pos = (iterPos->second).find_last_of( "/" );
//    std::string sSymbolName = (iterPos->second).substr( pos + 1 );
//    std::cout << " " << sSymbolName << "=" << iterPos->first << std::endl;
    std::cout << iterPos->second.sName << ": " << iterPos->first << std::endl;
//    if ( NULL != OnAddSymbol ) OnAddSymbol( sSymbolName, iterPos->second, "10%+" );
    selected.insert( iterPos->second );
  }
  m_mapMaxPositives.clear();

  std::cout << "Negatives: " << std::endl;
  mapRankingNeg_t::iterator iterNeg;
  for ( iterNeg = m_mapMaxNegatives.begin(); iterNeg != m_mapMaxNegatives.end(); ++iterNeg ) {
//    size_t pos = (iterNeg->second).find_last_of( "/" );
//    std::string sSymbolName = (iterNeg->second).substr( pos + 1 );
//    std::cout << " " << sSymbolName << "=" << iterNeg->first << std::endl;
    std::cout << " " << iterNeg->second.sName << ": " << iterNeg->first << std::endl;
//    if ( NULL != OnAddSymbol ) OnAddSymbol( sSymbolName, iterNeg->second, "10%-" );
    selected.insert( iterNeg->second );
  }
  m_mapMaxNegatives.clear();
}

class AverageVolatility {  
  // volatility is a misnomer, in actual fact, looking for measure highest returns in period of time
  // therefore may want to maximize the mean, and minimize the standard deviation
  // therefore normalize based upon pg  86 of Black Scholes and Beyond, 
  // then choose the one with 
private:
  double m_dblSumReturns;
  size_t m_nNumberOfValues;
//  std::vector<double> m_returns;
protected:
public:
  AverageVolatility() : m_dblSumReturns( 0 ), m_nNumberOfValues( 0 ) {};
  void operator() ( const ou::tf::Bar& bar ) {
    //m_dblVolatility += ( bar.High() - bar.Low() ) / bar.Close;
    //m_dblVolatility += ( bar.High() - bar.Low() ) / bar.Low();
    //m_dblVolatility += ( bar.Open() - bar.Close() ) / bar.Close();
    //m_dblVolatility += ( bar.m_dblHigh - bar.m_dblLow ) ;
    double return_ = log( bar.Close() / bar.Open() );
//    m_returns.push_back( return_ );
    m_dblSumReturns += return_;
    ++m_nNumberOfValues;
  }
  operator double() { 
    double mean = m_dblSumReturns / ( m_nNumberOfValues - 1 );
//    double dblDiffsSquared = 0;
//    double diff;
//    for ( std::vector<double>::const_iterator iter = m_returns.begin(); iter != m_returns.end(); iter++ ) {
//      diff = *iter - mean;
//      dblDiffsSquared += mean * mean;
//    }
//    double volatility = sqrt( dblDiffsSquared / m_nNumberOfValues );
//    return mean * volatility; // a measure of best trading range
    return mean;
  };
};

void SymbolSelection::CheckForVolatility( const InstrumentInfo& ii, citer begin, citer end ) {
  double dblAveragePrice = std::for_each( begin, end, AveragePrice() );
  citer iterLast( end - 1 );
  if ( 25.0 < dblAveragePrice ) {
    double dblAverageVolatility = std::for_each( begin, end, AverageVolatility() );
    mapRankingPos_t::iterator iterPos;
    if ( m_nMaxInList > m_mapMaxVolatility.size() ) {
      m_mapMaxVolatility.insert( pairRanking_t( dblAverageVolatility, ii ) );
    }
    else {
      iterPos = m_mapMaxVolatility.begin();
      if ( dblAverageVolatility > iterPos->first ) {
        m_mapMaxVolatility.erase( iterPos );
        m_mapMaxVolatility.insert( pairRanking_t( dblAverageVolatility, ii ) );
      }
    }
  }
}

void SymbolSelection::WrapUpVolatility( setInstrumentInfo_t& selected ) {
  std::cout << "Volatiles: " << std::endl;
  mapRankingPos_t::iterator iterPos;
  for ( iterPos = m_mapMaxVolatility.begin(); iterPos != m_mapMaxVolatility.end(); ++iterPos ) {
//    size_t pos = (iterPos->second).find_last_of( "/" );
//    std::string sSymbolName = (iterPos->second).substr( pos + 1 );;
//    cout << " " << sSymbolName << "=" << iterPos->first << endl;
    std::cout << " " << iterPos->second.sName << ": " << iterPos->first << std::endl;
//    if ( NULL != OnAddSymbol ) OnAddSymbol( sSymbolName, iterPos->second, "Volatile" );
    selected.insert( iterPos->second );
  }
  m_mapMaxVolatility.clear();
}