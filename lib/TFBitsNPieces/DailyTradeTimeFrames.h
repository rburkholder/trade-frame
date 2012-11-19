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

namespace ou { // One Unified
namespace tf { // TradeFrame

namespace TimeFrame {
  enum enumTimeFrame { Closed, PreRH, BellHeard, PauseForQuotes, RHTrading, Cancel, Cancelling, GoNeutral, GoingNeutral, WaitForRHClose, AfterRH };
}

template<class T> // CRTP type call for the overrides
class DailyTradeTimeFrame {
public:

  DailyTradeTimeFrame(void);
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
  template<typename DD> void HandleCommon( const DD& dd ) {};
  template<typename DD> void HandleRHTrading( const DD& dd ) {};
  template<typename DD> void HandleBellHeard( const DD& dd ) {};
  template<typename DD> void HandlePauseForQuotes( const DD& dd ) {};
  template<typename DD> void HandleCancel( const DD& dd ) {};
  template<typename DD> void HandleCancelling( const DD& dd ) {};
  template<typename DD> void HandleGoNeutral( const DD& dd ) {};
  template<typename DD> void HandleGoingNeutral( const DD& dd ) {};
  template<typename DD> void HandlePreOpen( const DD& dd ) {};
  template<typename DD> void HandleWaitForRHClose( const DD& dd ) {};
  template<typename DD> void HandleAfterRH( const DD& dd ) {};
  template<typename DD> void HandleEndOfMarket( const DD& dd ) {};
  template<typename DD> void HandleMarketClosed( const DD& dd ) {};
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

};

template<class T>
DailyTradeTimeFrame<T>::DailyTradeTimeFrame( void ) 
  :
  m_tdMarketOpen( time_duration( 8, 0, 0 ) ),
  m_tdRHOpen( time_duration( 10, 30, 0 ) ),
  m_tdStartTrading( time_duration( 10, 30, 30 ) ),
  m_tdTimeForCancellation( time_duration( 16, 50, 0 ) ), 
  m_tdGoNeutral( time_duration( 16, 52, 0 ) ),
  m_tdWaitForRHClose( time_duration( 16, 58, 0 ) ),
  m_tdRHClose( time_duration( 17, 0, 0 ) ),
  m_tdMarketClose( time_duration( 18, 30, 0 ) ), 
  m_stateTimeFrame( TimeFrame::Closed )
{
};

template<class T>
template<typename DD>
void DailyTradeTimeFrame<T>::TimeTick( DD& dd ) {  // DD is DatedDatum

  time_duration td( dd.DateTime().time_of_day() );

  static_cast<T*>(this)->HandleCommon( dd );

  switch ( m_stateTimeFrame ) {
  case TimeFrame::RHTrading:
    if ( td >= m_tdTimeForCancellation ) {  // any problems crossing midnight for futures type trading?
      m_stateTimeFrame = TimeFrame::Cancel;
      static_cast<T*>(this)->HandleCancel( dd );
      m_stateTimeFrame = TimeFrame::Cancelling;
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
      static_cast<T*>(this)->HandleGoNeutral( dd );
      m_stateTimeFrame = TimeFrame::GoingNeutral;
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
      static_cast<T*>(this)->HandleBellHeard( dd );
      m_stateTimeFrame = TimeFrame::PauseForQuotes;
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
