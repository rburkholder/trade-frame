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
using namespace boost::gregorian;

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
  DailyTradeTimeFrame( boost::gregorian::date );  // simluation date
  virtual ~DailyTradeTimeFrame(void) {};

  template<typename DD>  // DD is DatedDatum construct
  void TimeTick( DD& dd );

  void SetMarketOpen( time_duration tdMarketOpen ) { m_tdMarketOpen = tdMarketOpen; };
  void SetRegularHoursOpen( time_duration tdRHOpen ) { m_tdRHOpen = tdRHOpen; };
  void SetStartTrading( time_duration tdStartTrading ) { m_tdStartTrading = tdStartTrading; };
  void SetCancellation( time_duration tdTimeForCancellation ) { m_tdTimeForCancellation = tdTimeForCancellation; };
  void SetGoNeutral( time_duration tdGoNeutral ) { m_tdGoNeutral = tdGoNeutral; };
  void SetRegularHoursClose( time_duration tdRHClose ) { m_tdRHClose = tdRHClose; };
  void SetMarketClose( time_duration tdMarketClose ) { m_tdMarketClose = tdMarketClose; };

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
  time_duration m_tdMarketOpen;
  time_duration m_tdRHOpen;
  time_duration m_tdStartTrading;
  time_duration m_tdTimeForCancellation;
  time_duration m_tdGoNeutral;
  time_duration m_tdWaitForRHClose;
  time_duration m_tdRHClose;
  time_duration m_tdMarketClose;

  TimeFrame::enumTimeFrame m_stateTimeFrame;

  void Init( boost::gregorian::date );
  ptime Normalize( boost::gregorian::date date, time_duration time, const std::string& zone ) {
    return ou::TimeSource::Instance().ConvertRegionalToUtc( date, time, zone, true );
  }

};

template<class T>
DailyTradeTimeFrame<T>::DailyTradeTimeFrame( void ) 
  : m_stateTimeFrame( TimeFrame::Closed )
  // turn these into traits:  equities, futures, currencies
{
  Init( ou::TimeSource::Instance().External().date() );
};

template<class T>
DailyTradeTimeFrame<T>::DailyTradeTimeFrame( boost::gregorian::date date ) 
  : m_stateTimeFrame( TimeFrame::Closed )
  // turn these into traits:  equities, futures, currencies
{
  Init( date );
};

template<class T>
void DailyTradeTimeFrame<T>::Init( boost::gregorian::date date ) {
  m_tdMarketOpen          = Normalize( date, time_duration(  7,  0,  0 ), "America/New_York" ).time_of_day();
  m_tdRHOpen              = Normalize( date, time_duration(  9, 30,  0 ), "America/New_York" ).time_of_day();
  m_tdStartTrading        = Normalize( date, time_duration(  9, 30, 30 ), "America/New_York" ).time_of_day();
  m_tdTimeForCancellation = Normalize( date, time_duration( 15, 57,  0 ), "America/New_York" ).time_of_day();
  m_tdGoNeutral           = Normalize( date, time_duration( 15, 57, 15 ), "America/New_York" ).time_of_day();
  m_tdWaitForRHClose      = Normalize( date, time_duration( 15, 58,  0 ), "America/New_York" ).time_of_day();
  m_tdRHClose             = Normalize( date, time_duration( 16,  0,  0 ), "America/New_York" ).time_of_day();
  m_tdMarketClose         = Normalize( date, time_duration( 17, 30,  0 ), "America/New_York" ).time_of_day();
}

template<class T>
template<typename DD>
void DailyTradeTimeFrame<T>::TimeTick( DD& dd ) {  // DD is DatedDatum

  time_duration td( dd.DateTime().time_of_day() );

  static_cast<T*>(this)->HandleCommon( dd );

  switch ( m_stateTimeFrame ) {
  case TimeFrame::RHTrading:
    if ( td >= m_tdTimeForCancellation ) {  // any problems crossing midnight for futures type trading?
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
    if ( td >= m_tdStartTrading ) {
      m_stateTimeFrame = TimeFrame::RHTrading;
      static_cast<T*>(this)->HandleRHTrading( dd );
    }
    else {
      static_cast<T*>(this)->HandlePauseForQuotes( dd );
    }
    break;
  case TimeFrame::Cancelling:
    if ( td >= m_tdGoNeutral ) {
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
    if ( td >= m_tdWaitForRHClose ) {
      m_stateTimeFrame = TimeFrame::WaitForRHClose;
      static_cast<T*>(this)->HandleWaitForRHClose( dd );
    }
    else {
      static_cast<T*>(this)->HandleGoingNeutral( dd );
    }
    break;
  case TimeFrame::WaitForRHClose:
    if ( td >= m_tdRHClose ) {
      m_stateTimeFrame = TimeFrame::AfterRH;
      static_cast<T*>(this)->HandleAfterRH( dd );
    }
    else {
      static_cast<T*>(this)->HandleWaitForRHClose( dd );
    }
    break;
  case TimeFrame::AfterRH:
    if ( td >= m_tdMarketClose ) {
      m_stateTimeFrame = TimeFrame::Closed;
      static_cast<T*>(this)->HandleEndOfMarket( dd );
    }
    else {
      static_cast<T*>(this)->HandleAfterRH( dd );
    }
    break;
  case TimeFrame::PreRH:
    if ( td >= m_tdRHOpen ) {
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
    if ( ( ( m_tdMarketClose > m_tdMarketOpen ) && ( td >= m_tdMarketOpen ) && ( td < m_tdMarketClose ) ) // same day window
      || ( ( m_tdMarketClose < m_tdMarketOpen ) && ( td >= m_tdMarketOpen ) && ( td > m_tdMarketClose ) ) // crosses a day
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
