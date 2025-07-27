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
//    may not be possible based upon templating of the character type from the network buffer

#include <string>
#include <sstream>

#include <boost/log/trivial.hpp>

#include <boost/thread/thread.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
namespace posix_time = boost::posix_time;

#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/spirit/include/qi.hpp>

#include <boost/phoenix/core.hpp>

#include <OUCommon/ReusableBuffers.h>
#include <OUCommon/Network.h>

namespace qi = boost::spirit::qi;

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
    unsigned long Micro;
    posix_time::ptime DateTime;
    double Last;
    uint32_t LastSize;
    uint32_t TotalVolume;
    double Bid;
    double Ask;
    uint64_t TickID;
    char BasisForLast;  // 'C' normal, 'E' extended
    uint16_t MarketCenter;
    std::string sTradeConditions;
    uint16_t TradeAggressor;
    uint16_t DayCode;
  };

  // "I,LH,2023-06-28 00:00:00,4411.50,4411.25,4411.25,4411.50,42885,55,0,"
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
    uint32_t TotalVolume;
    uint32_t PeriodVolume;
    uint32_t NumberOfTrades;
  };

  // "O,LH,2022-08-02,4167.25,4160.00,4167.25,4160.00,3,1103,"
  struct EndOfDay {
    unsigned short Year;
    unsigned short Month;
    unsigned short Day;
    posix_time::ptime DateTime;
    double High;
    double Low;
    double Open;
    double Close;
    uint32_t PeriodVolume;
    uint32_t OpenInterest;
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
  (unsigned long, Micro)
  (double, Last)
  (uint32_t, LastSize)
  (uint32_t, TotalVolume)
  (double, Bid)
  (double, Ask)
  (uint64_t, TickID)
  (char, BasisForLast)
  (uint16_t, MarketCenter)
  (std::string, sTradeConditions)
  (uint16_t, TradeAggressor)
  (uint16_t, DayCode)
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
  (uint32_t, TotalVolume)
  (uint32_t, PeriodVolume)
  (uint32_t, NumberOfTrades)
  )

BOOST_FUSION_ADAPT_STRUCT(
  ou::tf::iqfeed::HistoryStructs::EndOfDay,
  (unsigned short, Year)
  (unsigned short, Month)
  (unsigned short, Day)
  (double, High)
  (double, Low)
  (double, Open)
  (double, Close)
  (uint32_t, PeriodVolume)
  (uint32_t, OpenInterest)
  )

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

namespace HistoryStructs {

  template <typename Iterator>
  struct DataPointParser: qi::grammar<Iterator, TickDataPoint()> {
    DataPointParser(): DataPointParser::base_type(start) {
      start %=
                  qi::lit( 'L' ) >> qi::lit( 'H' )
        >> ',' >> qi::ushort_ // year
        >> '-' >> qi::ushort_ // month
        >> '-' >> qi::ushort_ // day
        >> ' ' >> qi::ushort_ // hour
        >> ':' >> qi::ushort_ // minute
        >> ':' >> qi::ushort_ // second
        >> '.' >> qi::ulong_  // micro
        >> ',' >> qi::double_ // last
        >> ',' >> qi::ulong_  // last size
        >> ',' >> qi::ulong_  // total volume
        >> ',' >> qi::double_ // bid
        >> ',' >> qi::double_ // ask
        >> ',' >> qi::ulong_long  // tick id
        >> ',' >> qi::char_   // basis for last
        >> ',' >> qi::short_  // market center
        >> ',' >> +qi::char_( "0-9A-F" )  // trade conditions
        >> ',' >> qi::ushort_  // trade aggressor
        >> ',' >> qi::ushort_  // day code
        >> ','
        ;
    }
    qi::rule<Iterator, TickDataPoint()> start;
  };

  template <typename Iterator>
  struct IntervalParser: qi::grammar<Iterator, Interval()> {
    IntervalParser(): IntervalParser::base_type(start) {
      start %=
                  qi::lit( 'L' ) >> qi::lit( 'H' )
        >> ',' >> qi::ushort_ // year
        >> '-' >> qi::ushort_ // month
        >> '-' >> qi::ushort_ // day
        >> ' ' >> qi::ushort_ // hour
        >> ':' >> qi::ushort_ // minute
        >> ':' >> qi::ushort_ // second
        >> ',' >> qi::double_ // high
        >> ',' >> qi::double_ // low
        >> ',' >> qi::double_ // open
        >> ',' >> qi::double_ // close
        >> ',' >> qi::ulong_  // total volume
        >> ',' >> qi::ulong_  // period volume
        >> ',' >> qi::ulong_  // number of trades
        >> ','
        ;
    }
    qi::rule<Iterator, Interval()> start;
  };

  template <typename Iterator>
  struct EndOfDayParser: qi::grammar<Iterator, EndOfDay()> {
    EndOfDayParser(): EndOfDayParser::base_type(start) {
      start %=
                  qi::lit( 'L' ) >> qi::lit( 'H' )
        >> ',' >> qi::ushort_ // year
        >> '-' >> qi::ushort_ // month
        >> '-' >> qi::ushort_ // day
        >> ',' >> qi::double_ // high
        >> ',' >> qi::double_ // low
        >> ',' >> qi::double_ // open
        >> ',' >> qi::double_ // close
        >> ',' >> qi::ulong_  // period volume
        >> ',' >> qi::ulong_  // open interest
        >> ','
        ;
    }
    qi::rule<Iterator, EndOfDay()> start;
  };

} // namespace HistoryStructs

template <typename T> // T: CRTP inheriting class
class HistoryQuery: public ou::Network<HistoryQuery<T> > {
  friend ou::Network<HistoryQuery<T> >;
public:

  using TickDataPoint = ou::tf::iqfeed::HistoryStructs::TickDataPoint;
  using Interval      = ou::tf::iqfeed::HistoryStructs::Interval;
  using EndOfDay      = ou::tf::iqfeed::HistoryStructs::EndOfDay;

  HistoryQuery();
  virtual ~HistoryQuery();

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

  void RetrieveNIntervals( const std::string& sSymbol, unsigned int sec, unsigned int n );  // HIX sec=interval in seconds  (bars)
  void RetrieveNDaysOfIntervals( const std::string& sSymbol, unsigned int sec, unsigned int n ); // HID sec=interval in seconds (bars)
  void RetrieveDatedRangeOfIntervals( const std::string& sSymbol, unsigned int sec, posix_time::ptime dtStart, posix_time::ptime dtEnd ); // HIT intervals

  void RetrieveNEndOfDays( const std::string& sSymbol, unsigned int n );  // HDX  (bars)

  // once data is extracted, return the buffer for reuse
  void ReQueueTickDataPoint( TickDataPoint* pDP ) { m_reposTickDataPoint.CheckInL( pDP ); }
  void ReQueueInterval( Interval* pDP ) { m_reposInterval.CheckInL( pDP ); }
  void ReQueueEndOfDay( EndOfDay* pDP ) { m_reposEndOfDay.CheckInL( pDP ); }

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
  void OnNetworkConnected() {
    this->Send( "S,SET PROTOCOL,6.2\n" );
    if ( &HistoryQuery<T>::OnHistoryConnected != &T::OnHistoryConnected ) {
      static_cast<T*>( this )->OnHistoryConnected();
    }
  };
  void OnNetworkDisconnected() {
    if ( &HistoryQuery<T>::OnHistoryDisconnected != &T::OnHistoryDisconnected ) {
      static_cast<T*>( this )->OnHistoryDisconnected();
    }
  };
  void OnNetworkError( size_t e ) {
    if ( &HistoryQuery<T>::OnHistoryError != &T::OnHistoryError ) {
      static_cast<T*>( this )->OnHistoryError(e);
    }
  };
  void OnNetworkSendDone() {
    if ( &HistoryQuery<T>::OnHistorySendDone != &T::OnHistorySendDone ) {
      static_cast<T*>( this )->OnHistorySendDone();
    }
  };
  void OnNetworkLineBuffer( linebuffer_t* );  // new line available for processing

  // CRTP based dummy callbacks;
  void OnHistoryConnected() {};
  void OnHistoryDisconnected() {};
  void OnHistoryError( size_t e ) {};
  void OnHistorySendDone() {};
  void OnHistoryTickDataPoint( TickDataPoint* pDP ) { ReQueueTickDataPoint( pDP ); };
  void OnHistoryIntervalData( Interval* pDP ) { ReQueueInterval( pDP ); };
  void OnHistoryEndOfDayData( EndOfDay* pDP ) { ReQueueEndOfDay( pDP ); };
  void OnHistoryRequestDone( bool ) {};

private:

  using const_iterator_t = typename inherited_t::linebuffer_t::const_iterator;

  static const char c_chCmdError;
  static const char c_chCmdSystem;

  static const char c_chRidTick;
  static const char c_chRidInterval;
  static const char c_chRidEndOfDay;

  static const size_t c_nMillisecondsToSleep;

  // used for containing parsed data and passing it on
  ou::BufferRepository<TickDataPoint> m_reposTickDataPoint;
  ou::BufferRepository<Interval> m_reposInterval;
  ou::BufferRepository<EndOfDay> m_reposEndOfDay;

  ou::tf::iqfeed::HistoryStructs::DataPointParser<const_iterator_t> m_grammarDataPoint;
  ou::tf::iqfeed::HistoryStructs::IntervalParser<const_iterator_t> m_grammarInterval;
  ou::tf::iqfeed::HistoryStructs::EndOfDayParser<const_iterator_t> m_grammarEndOfDay;

  qi::rule<const_iterator_t> m_ruleEndMsg;
  qi::rule<const_iterator_t> m_ruleErrorInvalidSymbol;

  // Process the line
  void ProcessHistoryRetrieval( linebuffer_t* buf );

};

template <typename T>
HistoryQuery<T>::HistoryQuery()
: Network<HistoryQuery<T> >( "127.0.0.1", 9100 )
, m_stateRetrieval( RetrievalState::Idle )
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
      BOOST_LOG_TRIVIAL(error) << "Unknown HistoryQuery<T>::OnNetworkLineBuffer RetrievalState::Done";
      //throw std::logic_error( "RetrievalState::Done");
      //ReturnLineBuffer( wParam );
      break;
    case RetrievalState::Idle:
      switch ( *(*buf).begin() ) {
        case c_chCmdSystem: // captures the 'S,CURRENT PROTOCOL,6.2'
          break;
        default:
          // it is an error to land here
          BOOST_LOG_TRIVIAL(error) << "Unknown HistoryQuery<T>::OnNetworkLineBuffer RetrievalState::Idle";
          //throw std::logic_error( "RetrievalState::Idle");
          //ReturnLineBuffer( wParam );
          break;
      }
      break;
  }

  this->GiveBackBuffer( buf ); // 'this' is required for vtable access
}

template <typename T>
void HistoryQuery<T>::RetrieveNDataPoints( const std::string& sSymbol, unsigned int n ) {
  if ( RetrievalState::Idle != m_stateRetrieval ) {
    throw std::logic_error( "HistoryQuery<T>::RetrieveNDataPoints: not in IDLE");
  }
  else {
    m_stateRetrieval = RetrievalState::RetrieveDataPoints;
    std::stringstream ss;
    boost::this_thread::sleep( boost::posix_time::milliseconds( c_nMillisecondsToSleep ) );
    ss << "HTX," << sSymbol << "," << n << ",1," << c_chRidTick << "\n";
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
    boost::this_thread::sleep( boost::posix_time::milliseconds( c_nMillisecondsToSleep ) );
    ss << "HTD," << sSymbol << "," << n << ",,,,1," << c_chRidTick << "\n";
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

    boost::this_thread::sleep( boost::posix_time::milliseconds( c_nMillisecondsToSleep ) );

    ss << "HTT," << sSymbol << "," << dtStart << "," << dtEnd << ",,,,1," << c_chRidTick << "\n";
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
    boost::this_thread::sleep( boost::posix_time::milliseconds( c_nMillisecondsToSleep ) );
    ss << "HIX," << sSymbol << "," << i << "," << n << ",1," << c_chRidInterval << "\n";
    //BOOST_LOG_TRIVIAL(trace) << ss.str();
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
    boost::this_thread::sleep( boost::posix_time::milliseconds( c_nMillisecondsToSleep ) );
    ss << "HID," << sSymbol << "," << i << "," << n << ",,,,1," << c_chRidInterval << "\n";
    //BOOST_LOG_TRIVIAL(trace) << ss.str();
    this->Send( ss.str().c_str() );
  }
}

template <typename T>
void HistoryQuery<T>::RetrieveDatedRangeOfIntervals( const std::string& sSymbol, unsigned int sec, posix_time::ptime dtStart, posix_time::ptime dtEnd ) {
  if ( RetrievalState::Idle != m_stateRetrieval ) {
    throw std::logic_error( "HistoryQuery<T>::RetrieveDatedRangeOfIntervals: not in IDLE");
  }
  else {
    m_stateRetrieval = RetrievalState::RetrieveIntervals;

    // http://rhubbarb.wordpress.com/2009/10/17/boost-datetime-locales-and-facets/#more-944
    std::stringstream ss;
    //date_facet* facet( new date_facet( "&Y%m%d %H%M%S" ) );
    boost::posix_time::time_facet* facet( new boost::posix_time::time_facet );
    //ss.imbue( std::locale(ss.getloc(), facet ) );
    //ss.imbue( std::locale(std::locale::classic(), facet ) );
    std::locale special_locale (std::locale(""), facet);
    ss.imbue( special_locale );
    (*facet).format( "%Y%m%d %H%M%S" );

    boost::this_thread::sleep( boost::posix_time::milliseconds( c_nMillisecondsToSleep ) );

    ss << "HIT," << sSymbol << ',' << sec << ',' << dtStart << ',' << dtEnd << ",,,,," << c_chRidInterval << ",,s" << "\n";
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
    boost::this_thread::sleep( boost::posix_time::milliseconds( c_nMillisecondsToSleep ) );
    ss << "HDX," << sSymbol << "," << n << ",1," << c_chRidEndOfDay << "\n";
    //BOOST_LOG_TRIVIAL(trace) << ss.str();
    this->Send( ss.str().c_str() );
  }
}

template <typename T>
void HistoryQuery<T>::ProcessHistoryRetrieval( linebuffer_t* buf ) {

  typename linebuffer_t::const_iterator bgn = (*buf).begin();
  typename linebuffer_t::const_iterator end = (*buf).end();

  //std::string s( bgn, end );  // enable for debug
  //BOOST_LOG_TRIVIAL(trace) << "** " << s;
  // end message is like: 'T,!ENDMSG!'

  assert( ( end - bgn ) > 2 );

  typename linebuffer_t::const_iterator bgn3 = bgn;  // used for status

  char chRequestID = *bgn;
  bgn++; // skip id
  assert( ',' == *bgn );
  bgn++; // skip comma

  typename linebuffer_t::const_iterator bgn2 = bgn;  // used for error handling

  bool bParsed = false;
  switch ( chRequestID ) {
    case c_chRidTick: {
        assert ( RetrievalState::RetrieveDataPoints == m_stateRetrieval );
        TickDataPoint* pDP = m_reposTickDataPoint.CheckOutL();
        bParsed = parse( bgn, end, m_grammarDataPoint, *pDP );
        if ( bParsed && ( bgn == end ) ) {
          //pDP->DateTime = boost::posix_time::time_from_string( pDP->sDateTime );  // very very slow
          pDP->DateTime = posix_time::ptime(
            boost::gregorian::date( pDP->Year, pDP->Month, pDP->Day ),
            boost::posix_time::time_duration( pDP->Hour, pDP->Minute, pDP->Second, pDP->Micro ) );
          //if ( &HistoryQuery<T>::OnHistoryTickDataPoint != &T::OnHistoryTickDataPoint ) {
            static_cast<T*>( this )->OnHistoryTickDataPoint( pDP );
          //}
        }
        else {
          m_reposTickDataPoint.CheckInL( pDP );
        }
      }
      break;
    case c_chRidInterval: {
        assert ( RetrievalState::RetrieveIntervals == m_stateRetrieval );
        Interval* pDP = m_reposInterval.CheckOutL();
        bParsed = parse( bgn, end, m_grammarInterval, *pDP );
        if ( bParsed && ( bgn == end ) ) {
          //pDP->DateTime = boost::posix_time::time_from_string( pDP->sDateTime );  // very very slow
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
    case c_chRidEndOfDay: {
        assert ( RetrievalState::RetrieveEndOfDays == m_stateRetrieval );
        EndOfDay* pDP = m_reposEndOfDay.CheckOutL();
        bParsed = parse( bgn, end, m_grammarEndOfDay, *pDP );
        if ( bParsed && ( bgn == end ) ) {
          //pDP->DateTime = boost::posix_time::time_from_string( pDP->sDateTime );  // very very slow
          pDP->DateTime = posix_time::ptime(
            boost::gregorian::date( pDP->Year, pDP->Month, pDP->Day ),
            boost::posix_time::time_duration( 23, 59, 59 ) );
          //if ( &HistoryQuery<T>::OnHistorySummaryData != &T::OnHistorySummaryData ) {
            static_cast<T*>( this )->OnHistoryEndOfDayData( pDP );
          //}
        }
        else {
          m_reposEndOfDay.CheckInL( pDP );
        }
      }
      break;
    case c_chCmdError:
      {
        std::string s( bgn3, end );
        throw std::logic_error( "HistoryQuery<T>::ProcessHistoryRetrieval unknown error: " + s );
      }
      break;
    case c_chCmdSystem: // needs to be processed in idle state
      bParsed = true; // no parsing, but need the flag set
      std::cout << std::string( bgn3, end ) << std::endl;
      break;
    default:
      {
        std::string s( bgn3, end );
        throw std::logic_error( "HistoryQuery<T>::ProcessHistoryRetrieval unknown record: " + s );
      }
  }

  if ( !bParsed ) {
    if ( 'E' == *bgn2 ) { // indication of an error
      bParsed = parse( bgn2, end, m_ruleErrorInvalidSymbol );
      if ( bParsed ) {
        std::cout << "Invalid Symbol" << std::endl;
        m_stateRetrieval = RetrievalState::Idle;
          if ( &HistoryQuery<T>::OnHistoryRequestDone != &T::OnHistoryRequestDone ) {
            static_cast<T*>( this )->OnHistoryRequestDone( false );
          }
      }
    }
    else {
      bParsed = parse( bgn2, end, m_ruleEndMsg );
      if ( bParsed ) {
        m_stateRetrieval = RetrievalState::Idle;
          if ( &HistoryQuery<T>::OnHistoryRequestDone != &T::OnHistoryRequestDone ) {
            static_cast<T*>( this )->OnHistoryRequestDone( true );
          }
      }
      else {
        if ( &HistoryQuery<T>::OnHistoryRequestDone != &T::OnHistoryRequestDone ) {
          static_cast<T*>( this )->OnHistoryRequestDone( false );
        }
        throw std::logic_error( "HistoryQuery<T>::ProcessHistoryRetrieval no endmessage");
      }
    }
  }
}

template <typename T> const char   HistoryQuery<T>::c_chCmdError( 'E' );
template <typename T> const char   HistoryQuery<T>::c_chCmdSystem( 'S' );

template <typename T> const char   HistoryQuery<T>::c_chRidTick( 'T' );
template <typename T> const char   HistoryQuery<T>::c_chRidInterval( 'I' );
template <typename T> const char   HistoryQuery<T>::c_chRidEndOfDay( 'O' );

template <typename T> const size_t HistoryQuery<T>::c_nMillisecondsToSleep( 75 );

} // namespace iqfeed
} // namespace tf
} // namespace ou

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

