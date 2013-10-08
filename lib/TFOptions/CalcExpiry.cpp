/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <TFTimeSeries/ExchangeHolidays.h>

#include "CalcExpiry.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

// 2013/08/24 changed < to <= so can tell today is expiry
// be aware IQFeed has expiry on one day (Saturday I think)
//    Interactive Brokers uses Friday, I think, will need to confirm

boost::gregorian::months onemonth( 1 );

boost::gregorian::date CurrentFrontMonthExpiry( boost::gregorian::date date ) {

  typedef boost::gregorian::nth_day_of_the_week_in_month nth_dom;
  boost::gregorian::date dExpiry;

  nth_dom domExpiry1( nth_dom::third, boost::gregorian::Friday, date.month() ); 
  dExpiry = domExpiry1.get_date( date.year() );
  if ( date <= dExpiry ) { // use calculated expiry  (this is the difference from Next3rdFriday)
    // is upcoming in current month
  }
  else { // expiry is in next month
    date = boost::gregorian::date( date.year(), date.month(), 1 ) + onemonth;
    nth_dom domExpiry2( nth_dom::third, boost::gregorian::Friday, date.month() ); 
    dExpiry = domExpiry2.get_date( date.year() );
  }
  return dExpiry;
}

boost::gregorian::date Next3rdFriday( boost::gregorian::date date ) {

  typedef boost::gregorian::nth_day_of_the_week_in_month nth_dom;
  boost::gregorian::date dExpiry;

  nth_dom domExpiry1( nth_dom::third, boost::gregorian::Friday, date.month() ); 
  dExpiry = domExpiry1.get_date( date.year() );
  if ( date < dExpiry ) { // use calculated expiry 
    // is upcoming in current month
  }
  else { // expiry is in next month
    date = boost::gregorian::date( date.year(), date.month(), 1 ) + onemonth;
    nth_dom domExpiry2( nth_dom::third, boost::gregorian::Friday, date.month() ); 
    dExpiry = domExpiry2.get_date( date.year() );
  }
  return dExpiry;
}

boost::gregorian::date FuturesOptionExpiry( boost::gregorian::date date ) {
  boost::gregorian::day_iterator iterDay( boost::gregorian::date( date.year(), date.month(), 1 ), 1 );
  //--iterDay; // move to last day of previous month
  unsigned int cnt( 0 ); // need to move back four business days
  while ( 4 != cnt ) { // move to fourth last business day
    --iterDay;
    unsigned int dow = iterDay->day_of_week();
    if ( ( boost::gregorian::Sunday == dow ) || ( boost::gregorian::Saturday == dow ) ) {
    }
    else {
      --cnt; // decrement when business day encountered
    }
  }
  bool bOk( false );
  do {
    using namespace ou::tf::holidays::exchange;
    setDates_t::iterator iter = setUSDates.find( *iter );
    unsigned int dow = iterDay->day_of_week();
    if ( ( boost::gregorian::Friday == dow )
      || ( boost::gregorian::Sunday == dow ) || ( boost::gregorian::Saturday == dow )
      || ( setUSDates.end() != iter ) 
      ) {
        --iterDay;
    }
    else {
      bOk = true;
    }
  } while ( !bOk );
  return *iterDay;
}

} // namespace option
} // namespace tf
} // namespace ou

