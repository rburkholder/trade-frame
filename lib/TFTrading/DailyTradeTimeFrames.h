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

  static boost::gregorian::date MarketOpenDate( boost::posix_time::ptime dt );
  static boost::gregorian::date AdjustTimeFrame( boost::gregorian::date date_utc, boost::posix_time::time_duration time_utc );
  static boost::posix_time::ptime Normalize( boost::gregorian::date date, boost::posix_time::time_duration time, const std::string& zone ) {
    // timezone names found in https://github.com/boostorg/date_time/blob/master/data/date_time_zonespec.csv
    // convert to https://en.wikipedia.org/wiki/Tz_database, https://nodatime.org/TimeZones at some point
    // more info: https://stackoverflow.com/questions/16086962/how-to-get-a-time-zone-from-a-location-using-latitude-and-longitude-coordinates#16086964
    // use https://github.com/HowardHinnant/date or C++20 chrono
    // https://www.iana.org/time-zones, http://www.boost.org/doc/libs/1_57_0/libs/locale/doc/html/dates_times_timezones.html
    // https://en.wikipedia.org/wiki/Tz_database
    // see lib/OUCommon/TimeSource.cpp
    // https://howardhinnant.github.io/date/tz.html, Time Zone Database Parser <- this may be the way
    return ou::TimeSource::ConvertRegionalToUtc( date, time, zone );
  }

  void InitForUSEquityExchanges( boost::gregorian::date );
  void InitFor24HourMarkets( boost::gregorian::date );

  void SetMarketOpen( boost::posix_time::ptime dtMarketOpen ) { m_dtMarketOpen = dtMarketOpen; }
  void SetRegularHoursOpen( boost::posix_time::ptime dtRHOpen ) { m_dtRHOpen = dtRHOpen; }
  void SetStartTrading( boost::posix_time::ptime dtStartTrading ) { m_dtStartTrading = dtStartTrading; }
  void SetNoon( boost::posix_time::ptime dtNoon ) { m_dtNoon = dtNoon; }
  void SetCancellation( boost::posix_time::ptime dtTimeForCancellation ) { m_dtTimeForCancellation = dtTimeForCancellation; }
  void SetGoNeutral( boost::posix_time::ptime dtGoNeutral ) { m_dtGoNeutral = dtGoNeutral; }
  void SetWaitForRegularHoursClose( boost::posix_time::ptime dtRHWaitForClose ) { m_dtWaitForRHClose = dtRHWaitForClose; }
  void SetRegularHoursClose( boost::posix_time::ptime dtRHClose ) { m_dtRHClose = dtRHClose; }
  void SetMarketClose( boost::posix_time::ptime dtMarketClose ) { m_dtMarketClose = dtMarketClose; }
  void SetSoftwareReset( boost::posix_time::ptime dtSoftwareReset ) { m_dtSoftwareReset = dtSoftwareReset; }

  boost::posix_time::ptime GetMarketOpen() const { return m_dtMarketOpen; }
  boost::posix_time::ptime GetRegularHoursOpen() const { return m_dtRHOpen; }
  boost::posix_time::ptime GetStartTrading() const { return m_dtStartTrading; }
  boost::posix_time::ptime GetNoon() const { return m_dtNoon; }
  boost::posix_time::ptime GetCancellation() const { return m_dtTimeForCancellation; }
  boost::posix_time::ptime GetGoNeutral() const { return m_dtGoNeutral; }
  boost::posix_time::ptime GetWaitForRegularHoursClose() const { return m_dtWaitForRHClose; }
  boost::posix_time::ptime GetRegularHoursClose() const { return m_dtRHClose; }
  boost::posix_time::ptime GetMarketClose() const { return m_dtMarketClose; }
  boost::posix_time::ptime GetSoftwareReset() const { return m_dtSoftwareReset; }

protected:

  enum class TimeFrame { Done, PreRH, BellHeard, PauseForQuotes, RHTrading, Cancel, Cancelling, GoNeutral, GoingNeutral, WaitForRHClose, AtRHClose, AfterRH, Closed, SoftwareReset };

  TimeFrame CurrentTimeFrame() const { return m_stateTimeFrame; }

  // per type
  template<typename DD> void HandleCommon( const DD& dd ) {}
  template<typename DD> void HandleRHTrading( const DD& dd ) {}
  template<typename DD> void HandlePauseForQuotes( const DD& dd ) {}
  template<typename DD> void HandleCancelling( const DD& dd ) {}
  template<typename DD> void HandleGoingNeutral( const DD& dd ) {}
  template<typename DD> void HandlePreOpen( const DD& dd ) {}
  template<typename DD> void HandleWaitForRHClose( const DD& dd ) {}
  template<typename DD> void HandleAfterRH( const DD& dd ) {}
  template<typename DD> void HandleEndOfMarket( const DD& dd ) {}
  template<typename DD> void HandleMarketClosed( const DD& dd ) {}
  template<typename DD> bool HandleSoftwareReset( const DD& dd ) { return false; } // restart state machine

  // event change one shots
  void HandleBellHeard( boost::gregorian::date, boost::posix_time::time_duration ) {}
  void HandleCancel( boost::gregorian::date, boost::posix_time::time_duration ) {}
  void HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration ) {}
  void HandleAtRHClose( boost::gregorian::date, boost::posix_time::time_duration ) {}
  void HandleSoftwareReset( boost::gregorian::date, boost::posix_time::time_duration ) {} // nay need time based if no data

private:
  boost::posix_time::ptime m_dtMarketOpen;
  boost::posix_time::ptime m_dtRHOpen;
  boost::posix_time::ptime m_dtStartTrading;
  boost::posix_time::ptime m_dtNoon; // used for relative day calculations
  boost::posix_time::ptime m_dtTimeForCancellation;
  boost::posix_time::ptime m_dtGoNeutral;
  boost::posix_time::ptime m_dtWaitForRHClose;
  boost::posix_time::ptime m_dtRHClose;
  boost::posix_time::ptime m_dtMarketClose;
  boost::posix_time::ptime m_dtSoftwareReset;

  TimeFrame m_stateTimeFrame;

};

template<class T>
DailyTradeTimeFrame<T>::DailyTradeTimeFrame()
: m_stateTimeFrame( TimeFrame::PreRH )
  // turn these into traits:  equities, futures, currencies
{
  InitForUSEquityExchanges( ou::TimeSource::GlobalInstance().External().date() );
};

template<class T>
DailyTradeTimeFrame<T>::DailyTradeTimeFrame( boost::gregorian::date date )
: m_stateTimeFrame( TimeFrame::PreRH )
  // turn these into traits:  equities, futures, currencies
{
  InitForUSEquityExchanges( date );
};

template<class T>
void DailyTradeTimeFrame<T>::InitForUSEquityExchanges( boost::gregorian::date date ) {
  m_dtMarketOpen          = Normalize( date, boost::posix_time::time_duration(  7,  0,  0 ), "America/New_York" );
  m_dtRHOpen              = Normalize( date, boost::posix_time::time_duration(  9, 30,  0 ), "America/New_York" );
  m_dtStartTrading        = Normalize( date, boost::posix_time::time_duration(  9, 30, 30 ), "America/New_York" );
  m_dtNoon                = Normalize( date, boost::posix_time::time_duration( 12,  0,  0 ), "America/New_York" );
  m_dtTimeForCancellation = Normalize( date, boost::posix_time::time_duration( 15, 58,  0 ), "America/New_York" );
  m_dtGoNeutral           = Normalize( date, boost::posix_time::time_duration( 15, 58, 15 ), "America/New_York" );
  m_dtWaitForRHClose      = Normalize( date, boost::posix_time::time_duration( 15, 59,  0 ), "America/New_York" );
  m_dtRHClose             = Normalize( date, boost::posix_time::time_duration( 16,  0,  0 ), "America/New_York" );
  m_dtMarketClose         = Normalize( date, boost::posix_time::time_duration( 17, 30,  0 ), "America/New_York" );
  m_dtSoftwareReset       = Normalize( date, boost::posix_time::time_duration( 17, 45,  0 ), "America/New_York" );
}

template<class T>
boost::gregorian::date DailyTradeTimeFrame<T>::MarketOpenDate( boost::posix_time::ptime dtUtcCurrent ) {
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

template<class T>
boost::gregorian::date DailyTradeTimeFrame<T>::AdjustTimeFrame( boost::gregorian::date date_utc, boost::posix_time::time_duration time_utc ) {
  //calculate a date suitable for calling InitFor24HourMarkets

  ou::TimeSource& ts( ou::TimeSource::GlobalInstance() );
  auto tz = ts.LoadTimeZone( "America/New_York" );

  // determine UTC version of market session
  static const auto dtStart( boost::posix_time::time_duration( 17, 30, 0 ) );
  const boost::local_time::local_date_time ldt_start( date_utc, dtStart, tz, boost::local_time::local_date_time::EXCEPTION_ON_ERROR );
  const boost::posix_time::time_duration residual( ldt_start.utc_time().time_of_day() );

  // may need move date back a day for proper start of market
  const auto temp_utc = ( residual <= time_utc ) ? date_utc : date_utc - boost::gregorian::date_duration( 1 );

  // convert the date to market time for use in InitFor24HourMarkets
  const boost::local_time::local_date_time ldt_new( boost::posix_time::ptime( temp_utc, residual ), tz );
  const boost::gregorian::date date_new = ldt_new.local_time().date();

  //BOOST_LOG_TRIVIAL(info)
  //  << "original=" << date_utc << ' ' << time_utc
  //  << ",start=" << ldt_start.utc_time() << "(utc)," << ldt_start.local_time() << "(local)"
  //  << ",residual=" << residual << "(utc)"
  //  << ",final=" << date_new
  //  ;

  return date_new;
}

// TODO: based upon interior notes below, may need to instance this per future
//   override this initializer, or create a specific one
// (contractDetails).tradingHours ";20211031:1700-20211101:1500"
// (contractDetails).liquidHours  "20211031:CLOSED;20211101:0830-20211101:1500"
// (contractDetails).timeZoneId   "US/Central"

template<class T> // used for forex as well
void DailyTradeTimeFrame<T>::InitFor24HourMarkets( boost::gregorian::date date ) { // date from AdjustTimeFrame
  m_dtMarketOpen          = Normalize( date                                     , boost::posix_time::time_duration( 17, 45,  0 ), "America/New_York" );
  m_dtRHOpen              = Normalize( date                                     , boost::posix_time::time_duration( 18,  0,  0 ), "America/New_York" );
  m_dtStartTrading        = Normalize( date                                     , boost::posix_time::time_duration( 18,  0, 30 ), "America/New_York" );
  m_dtNoon                = Normalize( date + boost::gregorian::date_duration(1), boost::posix_time::time_duration( 12,  0,  0 ), "America/New_York" );
  // TO incorporate: ES has 15 minute break in the afternoon (15:15-15:30, then open till 16:00)
  m_dtTimeForCancellation = Normalize( date + boost::gregorian::date_duration(1), boost::posix_time::time_duration( 16, 57,  0 ), "America/New_York" );
  m_dtGoNeutral           = Normalize( date + boost::gregorian::date_duration(1), boost::posix_time::time_duration( 16, 57,  5 ), "America/New_York" );
  m_dtWaitForRHClose      = Normalize( date + boost::gregorian::date_duration(1), boost::posix_time::time_duration( 16, 58,  0 ), "America/New_York" );
  // will need to generify this:  ES till 15:15, GC till 17:00
  m_dtRHClose             = Normalize( date + boost::gregorian::date_duration(1), boost::posix_time::time_duration( 17,  0,  0 ), "America/New_York" );
  m_dtMarketClose         = Normalize( date + boost::gregorian::date_duration(1), boost::posix_time::time_duration( 17, 15,  0 ), "America/New_York" );
  m_dtSoftwareReset       = Normalize( date + boost::gregorian::date_duration(1), boost::posix_time::time_duration( 17, 35,  0 ), "America/New_York" );
}

template<class T>
template<typename DD>
void DailyTradeTimeFrame<T>::TimeTick( const DD& dd ) {  // DD is DatedDatum

  std::stringstream ss;

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
      // https://gcc.gnu.org/legacy-ml/gcc-help/2006-04/msg00023.html doesn't fix this
      //if ( &DailyTradeTimeFrame<T>::template HandleGoingNeutral<DD> != &T::template HandleGoingNeutral<DD> ) { // need to figure out comparison to bar|quote|trade|etc (DD)
        static_cast<T*>(this)->HandleGoingNeutral( dd );
      //}

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
    //ss << dt << "," << m_dtRHOpen;
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
    if ( dt >= m_dtSoftwareReset ) {
      m_stateTimeFrame = TimeFrame::SoftwareReset;
      if ( static_cast<T*>(this)->HandleSoftwareReset( dd ) ) {
        m_stateTimeFrame = TimeFrame::PreRH;
      }
      else {
        m_stateTimeFrame = TimeFrame::Done;
      }

    }
    else {
      static_cast<T*>(this)->HandleMarketClosed( dd );
    }
    break;
  case TimeFrame::Done:
    // spin wheels
    break;
  case TimeFrame::BellHeard:
    assert( false );  // shouldn't reach here
    break;
  }
}

} // namespace tf
} // namespace ou
