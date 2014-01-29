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

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

#include <OUCommon/TimeSource.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

namespace TimeFrame {
  enum enumTimeFrame { Closed, PreRH, BellHeard, PauseForQuotes, RHTrading, Cancel, Cancelling, GoNeutral, GoingNeutral, WaitForRHClose, AfterRH };
}

// 20121223 note a weakness in the statemachine: assumes continuous quotes to update statemachine
//     will a period time injection be required to step the machine given a lack of quotes?

// 20121223
//  during one shots:  emit event for all DD types
//  during periods:  simply call the crtp method

template<class T> // CRTP type call for the overrides
class DailyTradeTimeFrame {
public:

  DailyTradeTimeFrame(void); // uses today's date
  DailyTradeTimeFrame( boost::gregorian::date );  // simulation date
  virtual ~DailyTradeTimeFrame(void) {};

  template<typename DD>  // DD is DatedDatum construct
  void TimeTick( DD& dd );

  boost::gregorian::date NormalizeDate( ptime dt );
  ptime Normalize( boost::gregorian::date date, time_duration time, const std::string& zone ) {
    return ou::TimeSource::Instance().ConvertRegionalToUtc( date, time, zone, true );
  }

  void InitForUS24HourFutures( boost::gregorian::date date );

  void SetMarketOpen( ptime dtMarketOpen ) { m_dtMarketOpen = dtMarketOpen; };
  void SetRegularHoursOpen( ptime dtRHOpen ) { m_dtRHOpen = dtRHOpen; };
  void SetStartTrading( ptime dtStartTrading ) { m_dtStartTrading = dtStartTrading; };
  void SetCancellation( ptime dtTimeForCancellation ) { m_dtTimeForCancellation = dtTimeForCancellation; };
  void SetGoNeutral( ptime dtGoNeutral ) { m_dtGoNeutral = dtGoNeutral; };
  void SetRegularHoursClose( ptime dtRHClose ) { m_dtRHClose = dtRHClose; };
  void SetMarketClose( ptime dtMarketClose ) { m_dtMarketClose = dtMarketClose; };

protected:
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
  void HandleBellHeard( void ) {};
  void HandleCancel( void ) {};
  void HandleGoNeutral( void ) {};
private:
  ptime m_dtMarketOpen;
  ptime m_dtRHOpen;
  ptime m_dtStartTrading;
  ptime m_dtTimeForCancellation;
  ptime m_dtGoNeutral;
  ptime m_dtWaitForRHClose;
  ptime m_dtRHClose;
  ptime m_dtMarketClose;

  TimeFrame::enumTimeFrame m_stateTimeFrame;

  void InitForUSEquityExchanges( boost::gregorian::date );

};

template<class T>
DailyTradeTimeFrame<T>::DailyTradeTimeFrame( void ) 
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
  m_dtMarketOpen          = Normalize( date, time_duration(  7,  0,  0 ), "America/New_York" );
  m_dtRHOpen              = Normalize( date, time_duration(  9, 30,  0 ), "America/New_York" );
  m_dtStartTrading        = Normalize( date, time_duration(  9, 30, 30 ), "America/New_York" );
  m_dtTimeForCancellation = Normalize( date, time_duration( 15, 57,  0 ), "America/New_York" );
  m_dtGoNeutral           = Normalize( date, time_duration( 15, 57, 15 ), "America/New_York" );
  m_dtWaitForRHClose      = Normalize( date, time_duration( 15, 58,  0 ), "America/New_York" );
  m_dtRHClose             = Normalize( date, time_duration( 16,  0,  0 ), "America/New_York" );
  m_dtMarketClose         = Normalize( date, time_duration( 17, 30,  0 ), "America/New_York" );
}

template<class T>
boost::gregorian::date DailyTradeTimeFrame<T>:: NormalizeDate( ptime dt ) {
  ptime dtTransition = Normalize( dt.date(), time_duration( 17, 30,  0 ), "America/New_York" );  // market transition time
  boost::gregorian::date date;
  if ( dt.time_of_day() < dtTransition.time_of_day() ) {  // morning side
    date = dt.date() - date_duration(1);
  }
  else {  // evening side
    date = dt.date();
  }
  return date;
}

template<class T>
void DailyTradeTimeFrame<T>::InitForUS24HourFutures( boost::gregorian::date date ) { // needs normalized date
  m_dtMarketOpen          = Normalize( date                   , time_duration( 17, 45,  0 ), "America/New_York" );
  m_dtRHOpen              = Normalize( date + date_duration(1), time_duration(  9, 30,  0 ), "America/New_York" );
  m_dtStartTrading        = Normalize( date + date_duration(1), time_duration(  9, 30, 30 ), "America/New_York" );
  m_dtTimeForCancellation = Normalize( date + date_duration(1), time_duration( 15, 57,  0 ), "America/New_York" );
  m_dtGoNeutral           = Normalize( date + date_duration(1), time_duration( 15, 57, 15 ), "America/New_York" );
  m_dtWaitForRHClose      = Normalize( date + date_duration(1), time_duration( 15, 58,  0 ), "America/New_York" );
  m_dtRHClose             = Normalize( date + date_duration(1), time_duration( 16,  0,  0 ), "America/New_York" );
  m_dtMarketClose         = Normalize( date + date_duration(1), time_duration( 17, 15,  0 ), "America/New_York" );
}

template<class T>
template<typename DD>
void DailyTradeTimeFrame<T>::TimeTick( DD& dd ) {  // DD is DatedDatum

  std::stringstream ss;

  //time_duration td( dd.DateTime().time_of_day() );
  ptime dt( dd.DateTime() );

  static_cast<T*>(this)->HandleCommon( dd );

  switch ( m_stateTimeFrame ) {
  case TimeFrame::RHTrading:
    if ( dt >= m_dtTimeForCancellation ) {  // any problems crossing midnight for futures type trading?
      m_stateTimeFrame = TimeFrame::Cancel;
      static_cast<T*>(this)->HandleCancel();  // one shot
      m_stateTimeFrame = TimeFrame::Cancelling;
      static_cast<T*>(this)->HandleCancelling( dd );
    }
    else {
      static_cast<T*>(this)->HandleRHTrading( dd );
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
      static_cast<T*>(this)->HandleGoNeutral();  // one shot
      m_stateTimeFrame = TimeFrame::GoingNeutral;
      static_cast<T*>(this)->HandleGoingNeutral( dd );
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
    if ( dt >= m_dtRHOpen ) {
      m_stateTimeFrame = TimeFrame::BellHeard;
      static_cast<T*>(this)->HandleBellHeard();  // one shot
      m_stateTimeFrame = TimeFrame::PauseForQuotes;
      static_cast<T*>(this)->HandlePauseForQuotes( dd );
    }
    else {
      static_cast<T*>(this)->HandlePreOpen( dd );
    }
    break;
  case TimeFrame::Closed:
    ss << dt << "," << m_dtMarketOpen;
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
