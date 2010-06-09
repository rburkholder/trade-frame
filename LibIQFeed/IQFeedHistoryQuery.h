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

#pragma once

// processes historical data requests against the IQFeed API

// todo:  put parsers in separate compilation units to cut down on compile time

#define FUSION_MAX_VECTOR_SIZE 18

#include <string>
#include <sstream>
#include <vector>
#include <cassert>

#include <boost/config/warning_disable.hpp>

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

#include <boost/fusion/include/adapt_struct.hpp>

#include <LibCommon/ReusableBuffers.h>
#include <LibCommon/Network.h>

// custom on
// http://msdn.microsoft.com/en-us/library/e5ewb1h3.aspx
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
// custom off

/* If necessary to read values from a file, here is some old code to load stream formatted data from a file:
#include <fstream>
#include <stdexcept>

    string s;
    s.assign( szPrefix );
    s.append( "-" );
    s.append( szSymbol );
    s.append( ".txt" );

    char buf[ 512 ];
    ifstream file;
    file.open( s.c_str() );

    m_stateHistory = EHandleResponse;
    file.getline( buf, 511 );
    while ( 0 != *buf ) {
      OnPortMessage( buf );
      file.getline( buf, 511 );
    }
    m_stateHistory = EResponseDone;
    OnPortMessage( buf );
    file.close();

    */

namespace IQFeedHistoryStructs {

  struct structTickDataPoint {
    unsigned short Year;
    unsigned short Month;
    unsigned short Day;
    unsigned short Hour;
    unsigned short Minute;
    unsigned short Second;
    ptime DateTime;
    double Last;
    long  LastSize;
    long TotalVolume;
    double Bid;
    double Ask;
    long TickID;
    long BidSize;
    long AskSize;
    char BasisForLast;  // 'C' normal, 'E' extended
    
  };

  struct structInterval {
    unsigned short Year;
    unsigned short Month;
    unsigned short Day;
    unsigned short Hour;
    unsigned short Minute;
    unsigned short Second;
    ptime DateTime;
    double High;
    double Low;
    double Open;
    double Close;
    long TotalVolume;
    long PeriodVolume;
  };

  struct structSummary {
    unsigned short Year;
    unsigned short Month;
    unsigned short Day;
    unsigned short Hour;
    unsigned short Minute;
    unsigned short Second;
    ptime DateTime;
    double High;
    double Low;
    double Open;
    double Close;
    long PeriodVolume;
    long OpenInterest;
  };

}


BOOST_FUSION_ADAPT_STRUCT(
  IQFeedHistoryStructs::structTickDataPoint,
  (unsigned short, Year)
  (unsigned short, Month)
  (unsigned short, Day)
  (unsigned short, Hour)
  (unsigned short, Minute)
  (unsigned short, Second)
  (double, Last)
  (long, LastSize)
  (long, TotalVolume)
  (double, Bid)
  (double, Ask)
  (long, TickID)
  (long, BidSize)
  (long, AskSize)
  (char, BasisForLast)
  )

BOOST_FUSION_ADAPT_STRUCT(
  IQFeedHistoryStructs::structInterval,
  (unsigned short, Year)
  (unsigned short, Month)
  (unsigned short, Day)
  (unsigned short, Hour)
  (unsigned short, Minute)
  (unsigned short, Second) 
  (double, High)
  (double, Low)
  (double, Open)
  (double, Close)
  (long, TotalVolume)
  (long, PeriodVolume)
  )

BOOST_FUSION_ADAPT_STRUCT(
  IQFeedHistoryStructs::structSummary,
  (unsigned short, Year)
  (unsigned short, Month)
  (unsigned short, Day)
  (unsigned short, Hour)
  (unsigned short, Minute)
  (unsigned short, Second)
  (double, High)
  (double, Low)
  (double, Open)
  (double, Close)
  (long, PeriodVolume)
  (long, OpenInterest)
  )

namespace IQFeedHistoryStructs {

  template <typename Iterator>
  struct DataPointParser: qi::grammar<Iterator, structTickDataPoint()> {
    DataPointParser(): DataPointParser::base_type(start) {
      start %= 
                  qi::ushort_ >> '-' >> qi::ushort_ >> '-' >> qi::ushort_ 
        >> ' ' >> qi::ushort_ >> ':' >> qi::ushort_ >> ':' >> qi::ushort_
        >> ',' >> qi::double_ >> ',' >> qi::long_   >> ',' >> qi::long_
        >> ',' >> qi::double_ >> ',' >> qi::double_ >> ',' >> qi::long_
        >> ',' >> qi::long_   >> ',' >> qi::long_   >> ',' >> ascii::char_ 
        >> ','
        ;
    }
    qi::rule<Iterator, structTickDataPoint()> start;
  };

  template <typename Iterator>
  struct IntervalParser: qi::grammar<Iterator, structInterval()> {
    IntervalParser(): IntervalParser::base_type(start) {
      start %= 
                  qi::ushort_ >> '-' >> qi::ushort_ >> '-' >> qi::ushort_ 
        >> ' ' >> qi::ushort_ >> ':' >> qi::ushort_ >> ':' >> qi::ushort_
        >> ',' >> qi::double_ >> ',' >> qi::double_ 
        >> ',' >> qi::double_ >> ',' >> qi::double_ 
        >> ',' >> qi::long_   >> ',' >> qi::long_ 
        >> ','
        ;
    }
    qi::rule<Iterator, structInterval()> start;
  };

  template <typename Iterator>
  struct SummaryParser: qi::grammar<Iterator, structSummary()> {
    SummaryParser(): SummaryParser::base_type(start) {
      start %= 
                  qi::ushort_ >> '-' >> qi::ushort_ >> '-' >> qi::ushort_ 
        >> ' ' >> qi::ushort_ >> ':' >> qi::ushort_ >> ':' >> qi::ushort_
        >> ',' >> qi::double_ >> ',' >> qi::double_ 
        >> ',' >> qi::double_ >> ',' >> qi::double_ 
        >> ',' >> qi::long_   >> ',' >> qi::long_ 
        >> ','
        ;
    }
    qi::rule<Iterator, structSummary()> start;
  };

}

// T: CRTP inheriting class, U: type passed in for reference by inheriting class
template <typename T>
class CIQFeedHistoryQuery: public CNetwork<CIQFeedHistoryQuery<T> > {
  friend CNetwork<CIQFeedHistoryQuery<T> >;
public:

  typedef typename CNetwork<CIQFeedHistoryQuery<T> > inherited_t;

  typedef typename IQFeedHistoryStructs::structTickDataPoint structTickDataPoint;
  typedef typename IQFeedHistoryStructs::structInterval structInterval;
  typedef typename IQFeedHistoryStructs::structSummary structSummary;

  CIQFeedHistoryQuery( void );
  ~CIQFeedHistoryQuery( void );

  // start a query with one of these commands
  void RetrieveNDataPoints( const std::string& sSymbol, unsigned int n );  // HTX ticks
  void RetrieveNDaysOfDataPoints( const std::string& sSymbol, unsigned int n ); // HTD ticks

  void RetrieveNIntervals( const std::string& sSymbol, unsigned int i, unsigned int n );  // HIX i=interval in seconds  (bars)
  void RetrieveNDaysOfIntervals( const std::string& sSymbol, unsigned int i, unsigned int n ); // HID i=interval in seconds (bars)

  void RetrieveNEndOfDays( const std::string& sSymbol, unsigned int n );  // HDX  (bars)

  // once data is extracted, return the buffer for reuse
  void ReQueueTickDataPoint( structTickDataPoint* pDP ) { m_reposTickDataPoint.CheckInL( pDP ); }
  void ReQueueInterval( structInterval* pDP ) { m_reposInterval.CheckInL( pDP ); }
  void ReQueueSummary( structSummary* pDP ) { m_reposSummary.CheckInL( pDP ); }

protected:

  enum enumRetrievalState {  // activity in progress on this port
    RETRIEVE_IDLE = 0,  // no retrievals in progress
    RETRIEVE_HISTORY_DATAPOINTS,  // RequestID='D', data points are arriving
    RETRIEVE_HISTORY_INTERVALS,  // RequestID='I', interval data is arriving
    RETRIEVE_HISTORY_SUMMARY,  // RequestID='E', eod data is arriving
    RETRIEVE_DONE  // end marker arrived and is awaiting processing
  } m_stateRetrieval;

  // called by CNetwork via CRTP
  void OnNetworkConnected(void) {
    if ( &CIQFeedHistoryQuery<T>::OnHistoryConnected != &T::OnHistoryConnected ) {
      static_cast<T*>( this )->OnHistoryConnected();
    }
  };
  void OnNetworkDisconnected(void) {
    if ( &CIQFeedHistoryQuery<T>::OnHistoryDisconnected != &T::OnHistoryDisconnected ) {
      static_cast<T*>( this )->OnHistoryDisconnected();
    }
  };
  void OnNetworkError( size_t e ) {
    if ( &CIQFeedHistoryQuery<T>::OnHistoryError != &T::OnHistoryError ) {
      static_cast<T*>( this )->OnHistoryError(e);
    }
  };
  void OnNetworkSendDone(void) {
    if ( &CIQFeedHistoryQuery<T>::OnHistorySendDone != &T::OnHistorySendDone ) {
      static_cast<T*>( this )->OnHistorySendDone();
    }
  };
  void OnNetworkLineBuffer( linebuffer_t* );  // new line available for processing

  // CRTP based dummy callbacks;
  void OnHistoryConnected( void ) {};
  void OnHistoryDisconnected( void ) {};
  void OnHistoryError( size_t e ) {};
  void OnHistorySendDone( void ) {};
  void OnHistoryTickDataPoint( structTickDataPoint* pDP ) {};
  void OnHistoryIntervalData( structInterval* pDP ) {};
  void OnHistorySummaryData( structSummary* pDP ) {};
  void OnHistoryRequestDone( void ) {};

private:

  typedef typename inherited_t::linebuffer_t linebuffer_t;
  typedef typename inherited_t::linebuffer_t::const_iterator const_iterator_t;

  static const size_t m_nMillisecondsToSleep = 75;

  // used for containing parsed data and passing it on
  CBufferRepository<structTickDataPoint> m_reposTickDataPoint;
  CBufferRepository<structInterval> m_reposInterval;
  CBufferRepository<structSummary> m_reposSummary;

  IQFeedHistoryStructs::DataPointParser<const_iterator_t> m_grammarDataPoint;
  IQFeedHistoryStructs::IntervalParser<const_iterator_t> m_grammarInterval;
  IQFeedHistoryStructs::SummaryParser<const_iterator_t> m_grammarSummary;

  qi::rule<const_iterator_t> m_ruleEndMsg;
  qi::rule<const_iterator_t> m_ruleErrorInvalidSymbol;

  // Process the line
  void ProcessHistoryRetrieval( linebuffer_t* buf );

};

template <typename T>
CIQFeedHistoryQuery<T>::CIQFeedHistoryQuery( void ) 
: CNetwork<CIQFeedHistoryQuery<T> >( "127.0.0.1", 9100 ),
  m_stateRetrieval( RETRIEVE_IDLE )
{
  m_ruleEndMsg = qi::lit( "!ENDMSG!" );
  m_ruleErrorInvalidSymbol = qi::lit( "E,Invalid symbol" );
}

template <typename T>
CIQFeedHistoryQuery<T>::~CIQFeedHistoryQuery() {
}

template <typename T>
void CIQFeedHistoryQuery<T>::OnNetworkLineBuffer( linebuffer_t* buf ) {

#if defined _DEBUG
  {
    linebuffer_t::const_iterator bgn = (*buf).begin();
    linebuffer_t::const_iterator end = (*buf).end();

//    std::string str( bgn, end );
//    str += "\n";
//    OutputDebugString( str.c_str() );
  }
#endif

  switch ( m_stateRetrieval ) {
    case RETRIEVE_HISTORY_DATAPOINTS:
    case RETRIEVE_HISTORY_INTERVALS:
    case RETRIEVE_HISTORY_SUMMARY:
      ProcessHistoryRetrieval( buf );
      //ReturnLineBuffer( wParam ); 
      break;
    case RETRIEVE_DONE:
      // it is an error to land here
      OutputDebugString( "Unknown CIQFeedNewsQuery<T>::OnConnProcess RETRIEVE_DONE\n" );
      //throw std::logic_error( "RETRIEVE_DONE");
      //ReturnLineBuffer( wParam );
      break;
    case RETRIEVE_IDLE:
      // it is an error to land here
      OutputDebugString( "Unknown CIQFeedNewsQuery<T>::OnConnProcess RETRIEVE_IDLE\n" );
      //throw std::logic_error( "RETRIEVE_IDLE");
      //ReturnLineBuffer( wParam );
      break;
  }

  GiveBackBuffer( buf );
}

template <typename T>
void CIQFeedHistoryQuery<T>::RetrieveNDataPoints( const std::string& sSymbol, unsigned int n ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "CIQFeedHistoryQuery<T>::RetrieveNDataPoints: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_HISTORY_DATAPOINTS;
    std::stringstream ss;
    Sleep( m_nMillisecondsToSleep );
    ss << "HTX," << sSymbol << "," << n << ",1,D\n";
    Send( ss.str().c_str() );
  }
}

template <typename T>
void CIQFeedHistoryQuery<T>::RetrieveNDaysOfDataPoints( const std::string& sSymbol, unsigned int n ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "CIQFeedHistoryQuery<T>::RetrieveNDaysOfDataPoints: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_HISTORY_DATAPOINTS;
    std::stringstream ss;
    Sleep( m_nMillisecondsToSleep );
    ss << "HTD," << sSymbol << "," << n << ",,,,1,D\n";
    Send( ss.str().c_str() );
  }
}

template <typename T>
void CIQFeedHistoryQuery<T>::RetrieveNIntervals( const std::string& sSymbol, unsigned int i, unsigned int n ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "CIQFeedHistoryQuery<T>::RetrieveNIntervals: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_HISTORY_INTERVALS;
    std::stringstream ss;
    Sleep( m_nMillisecondsToSleep );
    ss << "HIX," << sSymbol << "," << i << "," << n << ",1,I\n";
    Send( ss.str().c_str() );
  }
}

template <typename T>
void CIQFeedHistoryQuery<T>::RetrieveNDaysOfIntervals( const std::string& sSymbol, unsigned int i, unsigned int n ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "CIQFeedHistoryQuery<T>::RetrieveNDaysOfIntervals: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_HISTORY_INTERVALS;
    std::stringstream ss;
    Sleep( m_nMillisecondsToSleep );
    ss << "HID," << sSymbol << "," << i << "," << n << ",,,,1,I\n";
    Send( ss.str().c_str() );
  }
}

template <typename T>
void CIQFeedHistoryQuery<T>::RetrieveNEndOfDays( const std::string& sSymbol, unsigned int n ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "CIQFeedHistoryQuery<T>::RetrieveNEndOfDays: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_HISTORY_SUMMARY;
    std::stringstream ss;
    Sleep( m_nMillisecondsToSleep );
    ss << "HDX," << sSymbol << "," << n << ",1,E\n";
    Send( ss.str().c_str() );
  }
}

template <typename T>
void CIQFeedHistoryQuery<T>::ProcessHistoryRetrieval( linebuffer_t* buf ) {

  linebuffer_t::const_iterator bgn = (*buf).begin();
  linebuffer_t::const_iterator end = (*buf).end();

  assert( ( end - bgn ) > 2 );
  char chRequestID = *bgn;
  bgn++;
  bgn++;
  linebuffer_t::const_iterator bgn2 = bgn;  // used for error handling

  bool b = false;
  switch ( chRequestID ) {
    case 'D': {
        assert ( RETRIEVE_HISTORY_DATAPOINTS == m_stateRetrieval );
        structTickDataPoint* pDP = m_reposTickDataPoint.CheckOutL();
        b = parse( bgn, end, m_grammarDataPoint, *pDP );
        if ( b && ( bgn == end ) ) {
          pDP->DateTime = ptime( 
            boost::gregorian::date( pDP->Year, pDP->Month, pDP->Day ), 
            boost::posix_time::time_duration( pDP->Hour, pDP->Minute, pDP->Second ) );
          if ( &CIQFeedHistoryQuery<T>::OnHistoryTickDataPoint != &T::OnHistoryTickDataPoint ) {
            static_cast<T*>( this )->OnHistoryTickDataPoint( pDP );
          }
        }
        else {
          m_reposTickDataPoint.CheckInL( pDP );
        }
      }
      break;
    case 'I': {
        assert ( RETRIEVE_HISTORY_INTERVALS == m_stateRetrieval );
        structInterval* pDP = m_reposInterval.CheckOutL();
        b = parse( bgn, end, m_grammarInterval, *pDP );
        if ( b && ( bgn == end ) ) {
          pDP->DateTime = ptime( 
            boost::gregorian::date( pDP->Year, pDP->Month, pDP->Day ), 
            boost::posix_time::time_duration( pDP->Hour, pDP->Minute, pDP->Second ) );
          if ( &CIQFeedHistoryQuery<T>::OnHistoryIntervalData != &T::OnHistoryIntervalData ) {
            static_cast<T*>( this )->OnHistoryIntervalData( pDP );
          }
        }
        else {
          m_reposInterval.CheckInL( pDP );
        }
      }
      break;
    case 'E': {
        assert ( RETRIEVE_HISTORY_SUMMARY == m_stateRetrieval );
        structSummary* pDP = m_reposSummary.CheckOutL();
        b = parse( bgn, end, m_grammarSummary, *pDP );
        if ( b && ( bgn == end ) ) {
          pDP->DateTime = ptime( 
            boost::gregorian::date( pDP->Year, pDP->Month, pDP->Day ), 
            boost::posix_time::time_duration( pDP->Hour, pDP->Minute, pDP->Second ) );
          if ( &CIQFeedHistoryQuery<T>::OnHistorySummaryData != &T::OnHistorySummaryData ) {
            static_cast<T*>( this )->OnHistorySummaryData( pDP );
          }
        }
        else {
          m_reposSummary.CheckInL( pDP );
        }
      }
      break;
    default:
      throw std::logic_error( "CIQFeedNewsQuery<T>::ProcessHistoryRetrieval unknown record");
  }

  if ( !b ) {
    if ( 'E' == *bgn2 ) { // indication of an error
      b = parse( bgn2, end, m_ruleErrorInvalidSymbol );
      if ( b ) {
        OutputDebugString( "Invalid Symbol\n" );
        m_stateRetrieval = RETRIEVE_IDLE;
          if ( &CIQFeedHistoryQuery<T>::OnHistoryRequestDone != &T::OnHistoryRequestDone ) {
            static_cast<T*>( this )->OnHistoryRequestDone();
          }
      }
    }
    else {
      b = parse( bgn2, end, m_ruleEndMsg );
      if ( b ) {
        m_stateRetrieval = RETRIEVE_IDLE;
          if ( &CIQFeedHistoryQuery<T>::OnHistoryRequestDone != &T::OnHistoryRequestDone ) {
            static_cast<T*>( this )->OnHistoryRequestDone();
          }
      }
      else {
        throw std::logic_error( "CIQFeedNewsQuery<T>::ProcessHistoryRetrieval no endmessage");
      }
    }
  }
}

