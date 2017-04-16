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
// 20151115 as of 2015/feb, things should be consistent to friday now
//   futures calc was changed to reflect current month last trading day
//     could cache dates into a lookup

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

// gold future calc:
// http://www.cmegroup.com/trading/metals/files/pm264-fact-card-gold-options.pdf
// options expiry: four business days prior to end of month, not on friday, 13:30pm, assignments notify 16:30, exercise 20:00
// trading: sunday - friday 18:00 - 17:15 et
// http://www.cmegroup.com/trading/metals/precious/gold_product_calendar_futures.html

// can't find specific reference, but the futures are probably the same but 3 days back

// setUSDates is in TFTimeSeries/ExchangeHolidays.cpp

// want the year/month combination
namespace local {
  
boost::gregorian::date CalcNBusinessDaysBack( boost::gregorian::date date, unsigned int cnt, bool bAllowFriday ) {
  boost::gregorian::date dateTemp( date.year(), date.month(), 1 );
  dateTemp += boost::gregorian::months( 1 );  // add one month, go to first of month, and step backwards
  boost::gregorian::day_iterator iterDay( boost::gregorian::date( dateTemp.year(), dateTemp.month(), 1 ), 1 );
  //unsigned int cnt( 4 ); // need to move back four business days
  while ( 0 != cnt ) { // move to fourth last business day
    using namespace ou::tf::holidays::exchange;
    --iterDay;
    unsigned int dow = iterDay->day_of_week();
    setDates_t::iterator iter = setUSDates.find( *iterDay );
    if ( ( boost::gregorian::Sunday == dow ) 
      || ( boost::gregorian::Saturday == dow ) 
      || ( setUSDates.end() != iter ) 
      ) {
    }
    else {
      --cnt; // decrement when business day encountered
    }
  }
  bool bOk( false );
  do {  // fixup for found day
    using namespace ou::tf::holidays::exchange;
    setDates_t::iterator iter = setUSDates.find( *iterDay );
    unsigned int dow = iterDay->day_of_week();
    if ( 
     ( ( ( boost::gregorian::Friday == dow ) && !bAllowFriday ) ) // future option uses this
      || ( boost::gregorian::Sunday == dow ) 
      || ( boost::gregorian::Saturday == dow )
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

} // namespace local

boost::gregorian::date FuturesExpiry( boost::gregorian::date date ) {
  return local::CalcNBusinessDaysBack( date, 3, true );
}

boost::gregorian::date FuturesOptionExpiry( boost::gregorian::date date ) {
  return local::CalcNBusinessDaysBack( date, 4, false );
}



} // namespace option
} // namespace tf
} // namespace ou

