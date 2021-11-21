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

#pragma once

// timezone reference:
// https://en.wikipedia.org/wiki/List_of_tz_database_time_zones

#include <boost/date_time/posix_time/posix_time.hpp>

#include <OUCommon/TimeSource.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

// 20121223 note a weakness in the statemachine: assumes continuous quotes to update statemachine
//     will a period time injection be required to step the machine given a lack of quotes?

// 20121223
//  during one shots:  emit event for all DD types
//  during periods:  simply call the crtp method

template<class T> // CRTP type call for the overrides
class DailyTradeTimeFrame {
public:

  DailyTradeTimeFrame(); // uses today's date
  DailyTradeTimeFrame( boost::gregorian::date );  // simulation date
  virtual ~DailyTradeTimeFrame() {};

  template<typename DD>  // DD is DatedDatum construct
  void TimeTick( const DD& dd );

  boost::gregorian::date MarketOpenDate( boost::posix_time::ptime dt ) const;
  boost::posix_time::ptime Normalize( boost::gregorian::date date, boost::posix_time::time_duration time, const std::string& zone ) const {
    return ou::TimeSource::Instance().ConvertRegionalToUtc( date, time, zone, true );
  }

  // used to override default InitForUSEquityExchanges
  void InitForUS24HourFutures( boost::gregorian::date date );

  void SetMarketOpen( boost::posix_time::ptime dtMarketOpen ) { m_dtMarketOpen = dtMarketOpen; };
  void SetRegularHoursOpen( boost::posix_time::ptime dtRHOpen ) { m_dtRHOpen = dtRHOpen; };
  void SetStartTrading( boost::posix_time::ptime dtStartTrading ) { m_dtStartTrading = dtStartTrading; };
  void SetCancellation( boost::posix_time::ptime dtTimeForCancellation ) { m_dtTimeForCancellation = dtTimeForCancellation; };
  void SetGoNeutral( boost::posix_time::ptime dtGoNeutral ) { m_dtGoNeutral = dtGoNeutral; };
  void SetRegularHoursClose( boost::posix_time::ptime dtRHClose ) { m_dtRHClose = dtRHClose; };
  void SetMarketClose( boost::posix_time::ptime dtMarketClose ) { m_dtMarketClose = dtMarketClose; };

  boost::posix_time::ptime GetMarketOpen( void ) const { return m_dtMarketOpen; };
  boost::posix_time::ptime GetRegularHoursOpen( void ) const { return m_dtRHOpen; };
  boost::posix_time::ptime GetStartTrading( void ) const { return m_dtStartTrading; };
  boost::posix_time::ptime GetCancellation( void ) const { return m_dtTimeForCancellation; };
  boost::posix_time::ptime GetGoNeutral( void ) const { return m_dtGoNeutral; };
  boost::posix_time::ptime GetRegularHoursClose( void ) const { return m_dtRHClose; };
  boost::posix_time::ptime GetMarketClose( void ) const { return m_dtMarketClose; };

protected:

  enum class TimeFrame { Closed, PreRH, BellHeard, PauseForQuotes, RHTrading, Cancel, Cancelling, GoNeutral, GoingNeutral, WaitForRHClose, AtRHClose, AfterRH };

  TimeFrame CurrentTimeFrame() const { return m_stateTimeFrame; }

  // per type
  template<typename DD> void HandleCommon( const DD& dd ) {};
  template<typename DD> void HandleRHTrading( const DD& dd ) {};
  template<typename DD> void HandlePauseForQuotes( const DD& dd ) {};
  template<typename DD> void HandleCancelling( const DD& dd ) {};
  template<typename DD> void HandleGoingNeutral( const DD& dd ) {};
  template<typename DD> void HandlePreOpen( const DD& dd ) {};
  template<typename DD> void HandleWaitForRHClose( const DD& dd ) {};
  template<typename DD> void HandleAfterRH( const DD& dd ) {};
  template<typename DD> void HandleEndOfMarket( const DD& dd ) {};
  template<typename DD> void HandleMarketClosed( const DD& dd ) {};

  // event change one shots
  void HandleBellHeard( boost::gregorian::date, boost::posix_time::time_duration ) {}
  void HandleCancel( boost::gregorian::date, boost::posix_time::time_duration ) {}
  void HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration ) {}
  void HandleAtRHClose( boost::gregorian::date, boost::posix_time::time_duration ) {}

private:
  boost::posix_time::ptime m_dtMarketOpen;
  boost::posix_time::ptime m_dtRHOpen;
  boost::posix_time::ptime m_dtStartTrading;
  boost::posix_time::ptime m_dtTimeForCancellation;
  boost::posix_time::ptime m_dtGoNeutral;
  boost::posix_time::ptime m_dtWaitForRHClose;
  boost::posix_time::ptime m_dtRHClose;
  boost::posix_time::ptime m_dtMarketClose;

  TimeFrame m_stateTimeFrame;

  // used by default
  void InitForUSEquityExchanges( boost::gregorian::date );

};

template<class T>
DailyTradeTimeFrame<T>::DailyTradeTimeFrame()
  : m_stateTimeFrame( TimeFrame::Closed )
  // turn these into traits:  equities, futures, currencies
{
  InitForUSEquityExchanges( ou::TimeSource::Instance().External().date() );
};

template<class T>
DailyTradeTimeFrame<T>::DailyTradeTimeFrame( boost::gregorian::date date )
  : m_stateTimeFrame( TimeFrame::Closed )
  // turn these into traits:  equities, futures, currencies
{
  InitForUSEquityExchanges( date );
};

template<class T>
void DailyTradeTimeFrame<T>::InitForUSEquityExchanges( boost::gregorian::date date ) {
  m_dtMarketOpen          = Normalize( date, boost::posix_time::time_duration(  7,  0,  0 ), "America/New_York" );
  m_dtRHOpen              = Normalize( date, boost::posix_time::time_duration(  9, 30,  0 ), "America/New_York" );
  m_dtStartTrading        = Normalize( date, boost::posix_time::time_duration(  9, 30, 30 ), "America/New_York" );
  m_dtTimeForCancellation = Normalize( date, boost::posix_time::time_duration( 15, 56,  0 ), "America/New_York" );
  m_dtGoNeutral           = Normalize( date, boost::posix_time::time_duration( 15, 56, 15 ), "America/New_York" );
  m_dtWaitForRHClose      = Normalize( date, boost::posix_time::time_duration( 15, 58,  0 ), "America/New_York" );
  m_dtRHClose             = Normalize( date, boost::posix_time::time_duration( 16,  0,  0 ), "America/New_York" );
  m_dtMarketClose         = Normalize( date, boost::posix_time::time_duration( 17, 30,  0 ), "America/New_York" );
}

template<class T>
boost::gregorian::date DailyTradeTimeFrame<T>:: MarketOpenDate( boost::posix_time::ptime dtUtcCurrent ) const {
  boost::posix_time::ptime dtUtcTransition
    = Normalize( dtUtcCurrent.date(), boost::posix_time::time_duration( 17, 0, 0 ), "America/New_York" );  // market transition time
  boost::gregorian::date date;
  if ( dtUtcCurrent.time_of_day() < dtUtcTransition.time_of_day() ) {  // morning side
    date = dtUtcCurrent.date() - boost::gregorian::date_duration(1);
  }
  else {  // evening side
    date = dtUtcCurrent.date();
  }
  return date;
}

// TODO: based upon interior notes below, may need to instance this per future
//   override this initializer, or create a specific one
// (contractDetails).tradingHours ";20211031:1700-20211101:1500"
// (contractDetails).liquidHours  "20211031:CLOSED;20211101:0830-20211101:1500"
// (contractDetails).timeZoneId   "US/Central"

template<class T>
void DailyTradeTimeFrame<T>::InitForUS24HourFutures( boost::gregorian::date date ) { // needs normalized date
  m_dtMarketOpen          = Normalize( date                                     , boost::posix_time::time_duration( 17, 45,  0 ), "America/New_York" );
  m_dtRHOpen              = Normalize( date                                     , boost::posix_time::time_duration( 18,  0,  0 ), "America/New_York" );
  m_dtStartTrading        = Normalize( date                                     , boost::posix_time::time_duration( 18,  0, 30 ), "America/New_York" );
  // TO incorporate: ES has 15 minute break in the afternoon (15:15-15:30, then open till 16:00)
  m_dtTimeForCancellation = Normalize( date + boost::gregorian::date_duration(1), boost::posix_time::time_duration( 16, 57,  0 ), "America/New_York" );
  m_dtGoNeutral           = Normalize( date + boost::gregorian::date_duration(1), boost::posix_time::time_duration( 16, 57,  5 ), "America/New_York" );
  m_dtWaitForRHClose      = Normalize( date + boost::gregorian::date_duration(1), boost::posix_time::time_duration( 16, 58,  0 ), "America/New_York" );
  // will need to generify this:  ES till 15:15, GC till 17:00
  m_dtRHClose             = Normalize( date + boost::gregorian::date_duration(1), boost::posix_time::time_duration( 17,  0,  0 ), "America/New_York" );
  m_dtMarketClose         = Normalize( date + boost::gregorian::date_duration(1), boost::posix_time::time_duration( 17, 15,  0 ), "America/New_York" );
}

template<class T>
template<typename DD>
void DailyTradeTimeFrame<T>::TimeTick( const DD& dd ) {  // DD is DatedDatum

  std::stringstream ss;

  //time_duration td( dd.DateTime().time_of_day() );
  boost::posix_time::ptime dt( dd.DateTime() );

  static_cast<T*>(this)->HandleCommon( dd );

  switch ( m_stateTimeFrame ) {
  case TimeFrame::RHTrading:
    if ( dt >= m_dtTimeForCancellation ) {  // any problems crossing midnight for futures type trading?
      m_stateTimeFrame = TimeFrame::Cancel;
      boost::gregorian::date date( dt.date() );
      boost::posix_time::time_duration time( dt.time_of_day() );
      static_cast<T*>(this)->HandleCancel( date, time );  // one shot
      m_stateTimeFrame = TimeFrame::Cancelling;
      static_cast<T*>(this)->HandleCancelling( dd );
    }
    else {
      // need to disambiguate DD on each side of !=
      //if ( &DailyTradeTimeFrame<T>::HandleRHTrading != &T::HandleRHTrading ) {
        static_cast<T*>(this)->HandleRHTrading( dd );
      //}
    }
    break;
  case TimeFrame::PauseForQuotes:
    if ( dt >= m_dtStartTrading ) {
      m_stateTimeFrame = TimeFrame::RHTrading;
      static_cast<T*>(this)->HandleRHTrading( dd );
    }
    else {
      static_cast<T*>(this)->HandlePauseForQuotes( dd );
    }
    break;
  case TimeFrame::Cancelling:
    if ( dt >= m_dtGoNeutral ) {
      m_stateTimeFrame = TimeFrame::GoNeutral;
      boost::gregorian::date date( dt.date() );
      boost::posix_time::time_duration time( dt.time_of_day() );
      static_cast<T*>(this)->HandleGoNeutral( date, time );  // one shot
      m_stateTimeFrame = TimeFrame::GoingNeutral;
//      if ( &DailyTradeTimeFrame<T>::HandleGoingNeutral<DD> != &T::HandleGoingNeutral ) { // need to figure out comparison to bar|quote|trade|etc (DD)
        static_cast<T*>(this)->HandleGoingNeutral( dd );
//      }

    }
    else {
      static_cast<T*>(this)->HandleCancelling( dd );
    }
    break;
  case TimeFrame::GoingNeutral:
    if ( dt >= m_dtWaitForRHClose ) {
      m_stateTimeFrame = TimeFrame::WaitForRHClose;
      static_cast<T*>(this)->HandleWaitForRHClose( dd );
    }
    else {
      static_cast<T*>(this)->HandleGoingNeutral( dd );
    }
    break;
  case TimeFrame::WaitForRHClose:
    if ( dt >= m_dtRHClose ) {
      m_stateTimeFrame = TimeFrame::AtRHClose;
      boost::gregorian::date date( dt.date() );
      boost::posix_time::time_duration time( dt.time_of_day() );
      static_cast<T*>(this)->HandleAtRHClose( date, time );  // one shot
      m_stateTimeFrame = TimeFrame::AfterRH;
      static_cast<T*>(this)->HandleAfterRH( dd );
    }
    else {
      static_cast<T*>(this)->HandleWaitForRHClose( dd );
    }
    break;
  case TimeFrame::AfterRH:
    if ( dt >= m_dtMarketClose ) {
      m_stateTimeFrame = TimeFrame::Closed;
      static_cast<T*>(this)->HandleEndOfMarket( dd );
    }
    else {
      static_cast<T*>(this)->HandleAfterRH( dd );
    }
    break;
  case TimeFrame::PreRH:
    ss << dt << "," << m_dtRHOpen;
    if ( dt >= m_dtRHOpen ) {
      m_stateTimeFrame = TimeFrame::BellHeard;
      boost::gregorian::date date( dt.date() );
      boost::posix_time::time_duration time( dt.time_of_day() );
      static_cast<T*>(this)->HandleBellHeard( date, time );  // one shot
      m_stateTimeFrame = TimeFrame::PauseForQuotes;
      static_cast<T*>(this)->HandlePauseForQuotes( dd );
    }
    else {
      static_cast<T*>(this)->HandlePreOpen( dd );
    }
    break;
  case TimeFrame::Closed:
    //ss << dt << "," << m_dtMarketOpen;
    if ( (  dt >= m_dtMarketOpen )
      ) {
        m_stateTimeFrame = TimeFrame::PreRH;
        static_cast<T*>(this)->HandlePreOpen( dd );
    }
    else {
      static_cast<T*>(this)->HandleMarketClosed( dd );
    }
    break;
  case TimeFrame::BellHeard:
    assert( false );  // shouldn't reach here
    break;
  }
}

} // namespace tf
} // namespace ou
