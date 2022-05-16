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

#include <boost/thread/thread.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
namespace posix_time = boost::posix_time;

#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

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

  struct TickDataPoint {
    unsigned short Year;
    unsigned short Month;
    unsigned short Day;
    unsigned short Hour;
    unsigned short Minute;
    unsigned short Second;
    posix_time::ptime DateTime;
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

  struct Interval {
    unsigned short Year;
    unsigned short Month;
    unsigned short Day;
    unsigned short Hour;
    unsigned short Minute;
    unsigned short Second;
    posix_time::ptime DateTime;
    double High;
    double Low;
    double Open;
    double Close;
    unsigned long TotalVolume;
    unsigned long PeriodVolume;
  };

  struct Summary {
    unsigned short Year;
    unsigned short Month;
    unsigned short Day;
    unsigned short Hour;
    unsigned short Minute;
    unsigned short Second;
    posix_time::ptime DateTime;
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
  ou::tf::iqfeed::HistoryStructs::TickDataPoint,
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
  ou::tf::iqfeed::HistoryStructs::Interval,
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
  ou::tf::iqfeed::HistoryStructs::Summary,
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
  struct DataPointParser: qi::grammar<Iterator, TickDataPoint()> {
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
    qi::rule<Iterator, TickDataPoint()> start;
  };

  template <typename Iterator>
  struct IntervalParser: qi::grammar<Iterator, Interval()> {
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
    qi::rule<Iterator, Interval()> start;
  };

  template <typename Iterator>
  struct SummaryParser: qi::grammar<Iterator, Summary()> {
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
    qi::rule<Iterator, Summary()> start;
  };

} // namespace HistoryStructs

// T: CRTP inheriting class
template <typename T>
class HistoryQuery: public ou::Network<HistoryQuery<T> > {
  friend ou::Network<HistoryQuery<T> >;
public:

  using structTickDataPoint = ou::tf::iqfeed::HistoryStructs::TickDataPoint;
  using structInterval =  ou::tf::iqfeed::HistoryStructs::Interval;
  using structSummary =  ou::tf::iqfeed::HistoryStructs::Summary;

  HistoryQuery( void );
  ~HistoryQuery( void );

  // http://www.iqfeed.net/dev/api/docs/docsBeta/HistoricalviaTCPIP.cfm
  // HTX: Retrieves up to [MaxDatapoints] number of trades for the specified [Symbol].
  // HTD: Retrieves trades for the previous [Days] days for the specified [Symbol].
  // HTT: Retrieves trade data between [BeginDate BeginTime] and [EndDate EndTime] for the specified [Symbol].
  // HIX: Retrieves [MaxDatapoints] number of Intervals of data for the specified [Symbol].
  // HID: Retrieves [Days] days of interval data for the specified [Symbol].
  // HDX: Retrieves up to [MaxDatapoints] number of End-Of-Day Data for the specified [Symbol].

  // start a query with one of these commands
  void RetrieveNDataPoints( const std::string& sSymbol, unsigned int n );  // HTX ticks
  void RetrieveNDaysOfDataPoints( const std::string& sSymbol, unsigned int n ); // HTD ticks
  void RetrieveDatedRangeOfDataPoints( const std::string& sSymbol, posix_time::ptime dtStart, posix_time::ptime dtEnd ); // HTT ticks

  void RetrieveNIntervals( const std::string& sSymbol, unsigned int i, unsigned int n );  // HIX i=interval in seconds  (bars)
  void RetrieveNDaysOfIntervals( const std::string& sSymbol, unsigned int i, unsigned int n ); // HID i=interval in seconds (bars)

  void RetrieveNEndOfDays( const std::string& sSymbol, unsigned int n );  // HDX  (bars)

  // once data is extracted, return the buffer for reuse
  void ReQueueTickDataPoint( structTickDataPoint* pDP ) { m_reposTickDataPoint.CheckInL( pDP ); }
  void ReQueueInterval( structInterval* pDP ) { m_reposInterval.CheckInL( pDP ); }
  void ReQueueSummary( structSummary* pDP ) { m_reposSummary.CheckInL( pDP ); }

protected:

  using inherited_t = typename ou::Network<HistoryQuery<T> >;
  using linebuffer_t = typename inherited_t::linebuffer_t;

  enum RetrievalState {  // activity in progress on this port 
    Idle = 0,  // no retrievals in progress
    RetrieveDataPoints,  // RequestID='D', data points are arriving
    RetrieveIntervals,  // RequestID='I', interval data is arriving
    RetrieveEndOfDays,  // RequestID='E', eod data is arriving 
    Done  // end marker arrived and is awaiting processing  
  } m_stateRetrieval;

  // called by Network via CRTP
  void OnNetworkConnected(void) {
    if ( &HistoryQuery<T>::OnHistoryConnected != &T::OnHistoryConnected ) {
      static_cast<T*>( this )->OnHistoryConnected();
    }
  };
  void OnNetworkDisconnected(void) {
    if ( &HistoryQuery<T>::OnHistoryDisconnected != &T::OnHistoryDisconnected ) {
      static_cast<T*>( this )->OnHistoryDisconnected();
    }
  };
  void OnNetworkError( size_t e ) {
    if ( &HistoryQuery<T>::OnHistoryError != &T::OnHistoryError ) {
      static_cast<T*>( this )->OnHistoryError(e);
    }
  };
  void OnNetworkSendDone(void) {
    if ( &HistoryQuery<T>::OnHistorySendDone != &T::OnHistorySendDone ) {
      static_cast<T*>( this )->OnHistorySendDone();
    }
  };
  void OnNetworkLineBuffer( linebuffer_t* );  // new line available for processing

  // CRTP based dummy callbacks;
  void OnHistoryConnected( void ) {};
  void OnHistoryDisconnected( void ) {};
  void OnHistoryError( size_t e ) {};
  void OnHistorySendDone( void ) {};
  void OnHistoryTickDataPoint( structTickDataPoint* pDP ) { ReQueueTickDataPoint( pDP ); };
  void OnHistoryIntervalData( structInterval* pDP ) { ReQueueInterval( pDP ); };
  void OnHistorySummaryData( structSummary* pDP ) { ReQueueSummary( pDP ); };
  void OnHistoryRequestDone( void ) {};

private:

  using const_iterator_t = typename inherited_t::linebuffer_t::const_iterator;

  static const size_t m_nMillisecondsToSleep;

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
  m_stateRetrieval( RetrievalState::Idle )
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
    case RetrievalState::RetrieveDataPoints:
    case RetrievalState::RetrieveIntervals:
    case RetrievalState::RetrieveEndOfDays:
      ProcessHistoryRetrieval( buf );
      //ReturnLineBuffer( wParam );
      break;
    case RetrievalState::Done:
      // it is an error to land here
      DEBUGOUT( "Unknown CIQFeedNewsQuery<T>::OnConnProcess RetrievalState::Done\n" );
      //throw std::logic_error( "RetrievalState::Done");
      //ReturnLineBuffer( wParam );
      break;
    case RetrievalState::Idle:
      // it is an error to land here
      DEBUGOUT( "Unknown CIQFeedNewsQuery<T>::OnConnProcess RetrievalState::Idle\n" );
      //throw std::logic_error( "RetrievalState::Idle");
      //ReturnLineBuffer( wParam );
      break;
  }

  this->GiveBackBuffer( buf );
}

template <typename T>
void HistoryQuery<T>::RetrieveNDataPoints( const std::string& sSymbol, unsigned int n ) {
  if ( RetrievalState::Idle != m_stateRetrieval ) {
    throw std::logic_error( "HistoryQuery<T>::RetrieveNDataPoints: not in IDLE");
  }
  else {
    m_stateRetrieval = RetrievalState::RetrieveDataPoints;
    std::stringstream ss;
    boost::this_thread::sleep( boost::posix_time::milliseconds( m_nMillisecondsToSleep ) );
    ss << "HTX," << sSymbol << "," << n << ",1,D\n";
    this->Send( ss.str().c_str() );
  }
}

template <typename T>
void HistoryQuery<T>::RetrieveNDaysOfDataPoints( const std::string& sSymbol, unsigned int n ) {
  if ( RetrievalState::Idle != m_stateRetrieval ) {
    throw std::logic_error( "HistoryQuery<T>::RetrieveNDaysOfDataPoints: not in IDLE");
  }
  else {
    m_stateRetrieval = RetrievalState::RetrieveDataPoints;
    std::stringstream ss;
    boost::this_thread::sleep( boost::posix_time::milliseconds( m_nMillisecondsToSleep ) );
    ss << "HTD," << sSymbol << "," << n << ",,,,1,D\n";
    this->Send( ss.str().c_str() );
  }
}

template <typename T>
void HistoryQuery<T>::RetrieveDatedRangeOfDataPoints( const std::string& sSymbol, posix_time::ptime dtStart, posix_time::ptime dtEnd ) {
  if ( RetrievalState::Idle != m_stateRetrieval ) {
    throw std::logic_error( "HistoryQuery<T>::RetrieveDatedRangeOfDataPoints: not in IDLE");
  }
  else {
    m_stateRetrieval = RetrievalState::RetrieveDataPoints;

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
  if ( RetrievalState::Idle != m_stateRetrieval ) {
    throw std::logic_error( "HistoryQuery<T>::RetrieveNIntervals: not in IDLE");
  }
  else {
    m_stateRetrieval = RetrievalState::RetrieveIntervals;
    std::stringstream ss;
    boost::this_thread::sleep( boost::posix_time::milliseconds( m_nMillisecondsToSleep ) );
    ss << "HIX," << sSymbol << "," << i << "," << n << ",1,I\n";
    this->Send( ss.str().c_str() );
  }
}

template <typename T>
void HistoryQuery<T>::RetrieveNDaysOfIntervals( const std::string& sSymbol, unsigned int i, unsigned int n ) {
  if ( RetrievalState::Idle != m_stateRetrieval ) {
    throw std::logic_error( "HistoryQuery<T>::RetrieveNDaysOfIntervals: not in IDLE");
  }
  else {
    m_stateRetrieval = RetrievalState::RetrieveIntervals;
    std::stringstream ss;
    boost::this_thread::sleep( boost::posix_time::milliseconds( m_nMillisecondsToSleep ) );
    ss << "HID," << sSymbol << "," << i << "," << n << ",,,,1,I\n";
    this->Send( ss.str().c_str() );
  }
}

template <typename T>
void HistoryQuery<T>::RetrieveNEndOfDays( const std::string& sSymbol, unsigned int n ) {
  if ( RetrievalState::Idle != m_stateRetrieval ) {
    throw std::logic_error( "HistoryQuery<T>::RetrieveNEndOfDays: not in IDLE");
  }
  else {
    m_stateRetrieval = RetrievalState::RetrieveEndOfDays;
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
        assert ( RetrievalState::RetrieveDataPoints == m_stateRetrieval );
        structTickDataPoint* pDP = m_reposTickDataPoint.CheckOutL();
        b = parse( bgn, end, m_grammarDataPoint, *pDP );
        if ( b && ( bgn == end ) ) {
          pDP->DateTime = posix_time::ptime(
            boost::gregorian::date( pDP->Year, pDP->Month, pDP->Day ),
            boost::posix_time::time_duration( pDP->Hour, pDP->Minute, pDP->Second ) );
          //if ( &HistoryQuery<T>::OnHistoryTickDataPoint != &T::OnHistoryTickDataPoint ) {
            static_cast<T*>( this )->OnHistoryTickDataPoint( pDP );
          //}
        }
        else {
          m_reposTickDataPoint.CheckInL( pDP );
        }
      }
      break;
    case 'I': {
        assert ( RetrievalState::RetrieveIntervals == m_stateRetrieval );
        structInterval* pDP = m_reposInterval.CheckOutL();
        b = parse( bgn, end, m_grammarInterval, *pDP );
        if ( b && ( bgn == end ) ) {
          pDP->DateTime = posix_time::ptime(
            boost::gregorian::date( pDP->Year, pDP->Month, pDP->Day ),
            boost::posix_time::time_duration( pDP->Hour, pDP->Minute, pDP->Second ) );
          //if ( &HistoryQuery<T>::OnHistoryIntervalData != &T::OnHistoryIntervalData ) {
            static_cast<T*>( this )->OnHistoryIntervalData( pDP );
          //}
        }
        else {
          m_reposInterval.CheckInL( pDP );
        }
      }
      break;
    case 'E': {
        assert ( RetrievalState::RetrieveEndOfDays == m_stateRetrieval );
        structSummary* pDP = m_reposSummary.CheckOutL();
        b = parse( bgn, end, m_grammarSummary, *pDP );
        if ( b && ( bgn == end ) ) {
          pDP->DateTime = posix_time::ptime(
            boost::gregorian::date( pDP->Year, pDP->Month, pDP->Day ),
            boost::posix_time::time_duration( pDP->Hour, pDP->Minute, pDP->Second ) );
          //if ( &HistoryQuery<T>::OnHistorySummaryData != &T::OnHistorySummaryData ) {
            static_cast<T*>( this )->OnHistorySummaryData( pDP );
          //}
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
        m_stateRetrieval = RetrievalState::Idle;
          if ( &HistoryQuery<T>::OnHistoryRequestDone != &T::OnHistoryRequestDone ) {
            static_cast<T*>( this )->OnHistoryRequestDone();
          }
      }
    }
    else {
      b = parse( bgn2, end, m_ruleEndMsg );
      if ( b ) {
        m_stateRetrieval = RetrievalState::Idle;
          if ( &HistoryQuery<T>::OnHistoryRequestDone != &T::OnHistoryRequestDone ) {
            static_cast<T*>( this )->OnHistoryRequestDone();
          }
      }
      else {
        throw std::logic_error( "HistoryQuery<T>::ProcessHistoryRetrieval no endmessage");
      }
    }
  }
}

template <typename T> const size_t HistoryQuery<T>::m_nMillisecondsToSleep = 75;

} // namespace iqfeed
} // namespace tf
} // namespace ou
