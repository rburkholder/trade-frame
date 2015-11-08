/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <boost/thread/thread.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

#include <boost/fusion/include/adapt_struct.hpp>

#include <OUCommon/ReusableBuffers.h>
#include <OUCommon/Network.h>

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


namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

namespace HistoryStructs {

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
    unsigned long TotalVolume;
    double Bid;
    double Ask;
    unsigned long TickID;
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
    unsigned long TotalVolume;
    unsigned long PeriodVolume;
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
    unsigned long PeriodVolume;
    unsigned long OpenInterest;
  };

} // namespace HistoryStructs
} // namespace iqfeed
} // namespace tf
} // namespace ou


BOOST_FUSION_ADAPT_STRUCT(
  ou::tf::iqfeed::HistoryStructs::structTickDataPoint,
  (unsigned short, Year)
  (unsigned short, Month)
  (unsigned short, Day)
  (unsigned short, Hour)
  (unsigned short, Minute)
  (unsigned short, Second)
  (double, Last)
  (long, LastSize)
  (unsigned long, TotalVolume)
  (double, Bid)
  (double, Ask)
  (unsigned long, TickID)
  (long, BidSize)
  (long, AskSize)
  (char, BasisForLast)
  )

BOOST_FUSION_ADAPT_STRUCT(
  ou::tf::iqfeed::HistoryStructs::structInterval,
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
  (unsigned long, TotalVolume)
  (unsigned long, PeriodVolume)
  )

BOOST_FUSION_ADAPT_STRUCT(
  ou::tf::iqfeed::HistoryStructs::structSummary,
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
  (unsigned long, PeriodVolume)
  (unsigned long, OpenInterest)
  )

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

namespace HistoryStructs {

  template <typename Iterator>
  struct DataPointParser: qi::grammar<Iterator, structTickDataPoint()> {
    DataPointParser(): DataPointParser::base_type(start) {
      start %= 
                  qi::ushort_ >> '-' >> qi::ushort_ >> '-' >> qi::ushort_ 
        >> ' ' >> qi::ushort_ >> ':' >> qi::ushort_ >> ':' >> qi::ushort_
        >> ',' >> qi::double_ >> ',' >> qi::long_   >> ',' >> qi::ulong_
        >> ',' >> qi::double_ >> ',' >> qi::double_ >> ',' >> qi::ulong_
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
        >> ',' >> qi::ulong_   >> ',' >> qi::ulong_ 
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
        >> ',' >> qi::ulong_   >> ',' >> qi::ulong_ 
        >> ','
        ;
    }
    qi::rule<Iterator, structSummary()> start;
  };

} // namespace HistoryStructs


// T: CRTP inheriting class, U: type passed in for reference by inheriting class
template <typename T>
class HistoryQuery: public ou::Network<HistoryQuery<T> > {
  friend ou::Network<HistoryQuery<T> >;
public:

  typedef typename ou::Network<HistoryQuery<T> > inherited_t;

  typedef typename ou::tf::iqfeed::HistoryStructs::structTickDataPoint structTickDataPoint;
  typedef typename ou::tf::iqfeed::HistoryStructs::structInterval structInterval;
  typedef typename ou::tf::iqfeed::HistoryStructs::structSummary structSummary;

  HistoryQuery( void );
  ~HistoryQuery( void );

  // start a query with one of these commands
  void RetrieveNDataPoints( const std::string& sSymbol, unsigned int n );  // HTX ticks
  void RetrieveNDaysOfDataPoints( const std::string& sSymbol, unsigned int n ); // HTD ticks
  void RetrieveDatedRangeOfDataPoints( const std::string& sSymbol, ptime dtStart, ptime dtEnd ); // HTT ticks

  void RetrieveNIntervals( const std::string& sSymbol, unsigned int i, unsigned int n );  // HIX i=interval in seconds  (bars)
  void RetrieveNDaysOfIntervals( const std::string& sSymbol, unsigned int i, unsigned int n ); // HID i=interval in seconds (bars)

  void RetrieveNEndOfDays( const std::string& sSymbol, unsigned int n );  // HDX  (bars)

  // once data is extracted, return the buffer for reuse
  void ReQueueTickDataPoint( structTickDataPoint* pDP ) { m_reposTickDataPoint.CheckInL( pDP ); }
  void ReQueueInterval( structInterval* pDP ) { m_reposInterval.CheckInL( pDP ); }
  void ReQueueSummary( structSummary* pDP ) { m_reposSummary.CheckInL( pDP ); }

protected:

  typedef typename inherited_t::linebuffer_t linebuffer_t;
  
  enum enumRetrievalState {  // activity in progress on this port
    RETRIEVE_IDLE = 0,  // no retrievals in progress
    RETRIEVE_HISTORY_DATAPOINTS,  // RequestID='D', data points are arriving
    RETRIEVE_HISTORY_INTERVALS,  // RequestID='I', interval data is arriving
    RETRIEVE_HISTORY_SUMMARY,  // RequestID='E', eod data is arriving
    RETRIEVE_DONE  // end marker arrived and is awaiting processing
  } m_stateRetrieval;

  // called by CNetwork via CRTP
  void OnNetworkConnected(void) {
    //if ( &HistoryQuery<T>::OnHistoryConnected != static_cast<T*>( this )->OnHistoryConnected ) {
      static_cast<T*>( this )->OnHistoryConnected();
    //}
  };
  void OnNetworkDisconnected(void) {
    //if ( &HistoryQuery<T>::OnHistoryDisconnected != &T::OnHistoryDisconnected ) {
      static_cast<T*>( this )->OnHistoryDisconnected();
    //}
  };
  void OnNetworkError( size_t e ) {
    //if ( &HistoryQuery<T>::OnHistoryError != &T::OnHistoryError ) {
      static_cast<T*>( this )->OnHistoryError(e);
    //}
  };
  void OnNetworkSendDone(void) {
    //if ( &HistoryQuery<T>::OnHistorySendDone != &T::OnHistorySendDone ) {
      static_cast<T*>( this )->OnHistorySendDone();
    //}
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

  typedef typename inherited_t::linebuffer_t::const_iterator const_iterator_t;

  static const size_t m_nMillisecondsToSleep = 75;

  // used for containing parsed data and passing it on
  ou::BufferRepository<structTickDataPoint> m_reposTickDataPoint;
  ou::BufferRepository<structInterval> m_reposInterval;
  ou::BufferRepository<structSummary> m_reposSummary;

  ou::tf::iqfeed::HistoryStructs::DataPointParser<const_iterator_t> m_grammarDataPoint;
  ou::tf::iqfeed::HistoryStructs::IntervalParser<const_iterator_t> m_grammarInterval;
  ou::tf::iqfeed::HistoryStructs::SummaryParser<const_iterator_t> m_grammarSummary;

  qi::rule<const_iterator_t> m_ruleEndMsg;
  qi::rule<const_iterator_t> m_ruleErrorInvalidSymbol;

  // Process the line
  void ProcessHistoryRetrieval( linebuffer_t* buf );

};

template <typename T>
HistoryQuery<T>::HistoryQuery( void ) 
: Network<HistoryQuery<T> >( "127.0.0.1", 9100 ),
  m_stateRetrieval( RETRIEVE_IDLE )
{
  m_ruleEndMsg = qi::lit( "!ENDMSG!" );
  m_ruleErrorInvalidSymbol = qi::lit( "E,Invalid symbol" );
}

template <typename T>
HistoryQuery<T>::~HistoryQuery() {
}

template <typename T>
void HistoryQuery<T>::OnNetworkLineBuffer( linebuffer_t* buf ) {

#if defined _DEBUG
  {
    typename linebuffer_t::const_iterator bgn = (*buf).begin();
    typename linebuffer_t::const_iterator end = (*buf).end();

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
      DEBUGOUT( "Unknown CIQFeedNewsQuery<T>::OnConnProcess RETRIEVE_DONE\n" );
      //throw std::logic_error( "RETRIEVE_DONE");
      //ReturnLineBuffer( wParam );
      break;
    case RETRIEVE_IDLE:
      // it is an error to land here
      DEBUGOUT( "Unknown CIQFeedNewsQuery<T>::OnConnProcess RETRIEVE_IDLE\n" );
      //throw std::logic_error( "RETRIEVE_IDLE");
      //ReturnLineBuffer( wParam );
      break;
  }

  this->GiveBackBuffer( buf );
}

template <typename T>
void HistoryQuery<T>::RetrieveNDataPoints( const std::string& sSymbol, unsigned int n ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "HistoryQuery<T>::RetrieveNDataPoints: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_HISTORY_DATAPOINTS;
    std::stringstream ss;
    boost::this_thread::sleep( boost::posix_time::milliseconds( m_nMillisecondsToSleep ) );
    ss << "HTX," << sSymbol << "," << n << ",1,D\n";
    this->Send( ss.str().c_str() );
  }
}

template <typename T>
void HistoryQuery<T>::RetrieveNDaysOfDataPoints( const std::string& sSymbol, unsigned int n ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "HistoryQuery<T>::RetrieveNDaysOfDataPoints: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_HISTORY_DATAPOINTS;
    std::stringstream ss;
    boost::this_thread::sleep( boost::posix_time::milliseconds( m_nMillisecondsToSleep ) );
    ss << "HTD," << sSymbol << "," << n << ",,,,1,D\n";
    this->Send( ss.str().c_str() );
  }
}

template <typename T>
void HistoryQuery<T>::RetrieveDatedRangeOfDataPoints( const std::string& sSymbol, ptime dtStart, ptime dtEnd ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "HistoryQuery<T>::RetrieveDatedRangeOfDataPoints: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_HISTORY_DATAPOINTS;

    // http://rhubbarb.wordpress.com/2009/10/17/boost-datetime-locales-and-facets/#more-944
    std::stringstream ss;
    //date_facet* facet( new date_facet( "&Y%m%d %H%M%S" ) );
    boost::posix_time::time_facet* facet( new boost::posix_time::time_facet );
    //ss.imbue( std::locale(ss.getloc(), facet ) );
    //ss.imbue( std::locale(std::locale::classic(), facet ) );
    std::locale special_locale (std::locale(""), facet);
    ss.imbue( special_locale );
    (*facet).format( "%Y%m%d %H%M%S" );

    boost::this_thread::sleep( boost::posix_time::milliseconds( m_nMillisecondsToSleep ) );

    ss << "HTT," << sSymbol << "," << dtStart << "," << dtEnd << ",,,,1,D\n";
    this->Send( ss.str().c_str() );
  }
}

template <typename T>
void HistoryQuery<T>::RetrieveNIntervals( const std::string& sSymbol, unsigned int i, unsigned int n ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "HistoryQuery<T>::RetrieveNIntervals: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_HISTORY_INTERVALS;
    std::stringstream ss;
    boost::this_thread::sleep( boost::posix_time::milliseconds( m_nMillisecondsToSleep ) );
    ss << "HIX," << sSymbol << "," << i << "," << n << ",1,I\n";
    this->Send( ss.str().c_str() );
  }
}

template <typename T>
void HistoryQuery<T>::RetrieveNDaysOfIntervals( const std::string& sSymbol, unsigned int i, unsigned int n ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "HistoryQuery<T>::RetrieveNDaysOfIntervals: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_HISTORY_INTERVALS;
    std::stringstream ss;
    boost::this_thread::sleep( boost::posix_time::milliseconds( m_nMillisecondsToSleep ) );
    ss << "HID," << sSymbol << "," << i << "," << n << ",,,,1,I\n";
    this->Send( ss.str().c_str() );
  }
}

template <typename T>
void HistoryQuery<T>::RetrieveNEndOfDays( const std::string& sSymbol, unsigned int n ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "HistoryQuery<T>::RetrieveNEndOfDays: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_HISTORY_SUMMARY;
    std::stringstream ss;
    boost::this_thread::sleep( boost::posix_time::milliseconds( m_nMillisecondsToSleep ) );
    ss << "HDX," << sSymbol << "," << n << ",1,E\n";
    this->Send( ss.str().c_str() );
  }
}

template <typename T>
void HistoryQuery<T>::ProcessHistoryRetrieval( linebuffer_t* buf ) {

  typename linebuffer_t::const_iterator bgn = (*buf).begin();
  typename linebuffer_t::const_iterator end = (*buf).end();

  assert( ( end - bgn ) > 2 );
  char chRequestID = *bgn;
  bgn++;
  bgn++;
  typename linebuffer_t::const_iterator bgn2 = bgn;  // used for error handling

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
          if ( &HistoryQuery<T>::OnHistoryTickDataPoint != &T::OnHistoryTickDataPoint ) {
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
          if ( &HistoryQuery<T>::OnHistoryIntervalData != &T::OnHistoryIntervalData ) {
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
          if ( &HistoryQuery<T>::OnHistorySummaryData != &T::OnHistorySummaryData ) {
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
        DEBUGOUT( "Invalid Symbol\n" );
        m_stateRetrieval = RETRIEVE_IDLE;
          if ( &HistoryQuery<T>::OnHistoryRequestDone != &T::OnHistoryRequestDone ) {
            static_cast<T*>( this )->OnHistoryRequestDone();
          }
      }
    }
    else {
      b = parse( bgn2, end, m_ruleEndMsg );
      if ( b ) {
        m_stateRetrieval = RETRIEVE_IDLE;
          if ( &HistoryQuery<T>::OnHistoryRequestDone != &T::OnHistoryRequestDone ) {
            static_cast<T*>( this )->OnHistoryRequestDone();
          }
      }
      else {
        throw std::logic_error( "CIQFeedNewsQuery<T>::ProcessHistoryRetrieval no endmessage");
      }
    }
  }
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
