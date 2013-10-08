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

#include "StdAfx.h"

#include <algorithm>

#include "PivotGroup.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PivotGroup::PivotGroup(void) 
 {
}

PivotGroup::PivotGroup( Bars* pBars ) {
  CalculatePivotSets( pBars );
}

PivotGroup::~PivotGroup(void) {
  m_mmPivots.clear();
}

void PivotGroup::AddToMap( PivotSet &set ) {
  for ( unsigned short ix = 0; ix < PivotSet::PivotCount; ++ix ) {
    m_mmPivots.insert( set[ ix ] );
  }
}

void PivotGroup::CalculatePivotSets(Bars* pBars) {

  const Bar& bar0( *pBars->Last() );

  //stringstream ss;
  date dtThisDay = bar0.DateTime().date();
  //ss << "This Day: " << dtThisDay;
  date dtPrevDay = ( boost::date_time::Monday == dtThisDay.day_of_week() ) 
    ? dtThisDay - days( 3 )
    : dtThisDay - days( 1 );
  //ss << ", Prev Day: " << dtPrevDay;
  date dtPrevMonday = dtThisDay + days( boost::date_time::Monday - dtThisDay.day_of_week() ) - days( 7 );
  //ss << ", Prev Monday: " << dtPrevMonday;
  date dtPrevWeekFriday = dtPrevMonday + days( 4 );
  //ss << ", Prev Friday: " << dtPrevWeekFriday;
  date dtPrevMonthEnd = dtThisDay - days( dtThisDay.day().as_number() );
  //ss << ", Prev Month End: " << dtPrevMonthEnd;
  date dtPrevMonth = dtPrevMonthEnd - days( dtPrevMonthEnd.day().as_number() - 1 );
  //ss << ", Prev Month: " << dtPrevMonth;
  date dtMonthAgo = dtThisDay - days( 30 );
  //ss << ", Month ago: " << dtMonthAgo;
  date dtSixMonthsAgo = dtThisDay - months( 6 );
  //ss << ", Six Months ago: " << dtSixMonthsAgo;
  //date dt200DaysAgo = dtThisDay - days( 42 * 7 - 2 );
  date dt200BarsAgo = dtThisDay - days( ( ( 200 /*bars wanted*/ / 5 /*days/wk*=#wks*/ ) * 7 /*days/wk*/ ) + 10 /*trade holidays*/ ); // ensures 200 bars is within reasonable time frame
  //ss << ", 200 Days ago: " << dt200DaysAgo;

  PivotSet Pivot1Day( "pv1Dy", bar0.High(), bar0.Low(), bar0.Close() );
  AddToMap( Pivot1Day );

  double day3hi = bar0.High();
  double day3lo = bar0.Low();
  double day3cl = bar0.Close();

  if ( pBars->Size() >= 3 ) {

    const Bar& bar1( pBars->Ago( 1 ) );
    day3hi = std::max<double>( day3hi, bar1.High() );
    day3lo = std::min<double>( day3lo, bar1.Low() );

    const Bar& bar2( pBars->Ago( 2 ) );
    day3hi = std::max<double>( day3hi, bar2.High() );
    day3lo = std::min<double>( day3lo, bar2.Low() );

    PivotSet Pivot3Day( "pv3Dy", day3hi, day3lo, day3cl );
    AddToMap( Pivot3Day );
  }

  Bars* pBarsForPeriod;

  if ( pBars->Size() >= 10 ) {
    pBarsForPeriod = pBars->Subset( ptime( dtPrevMonday ), 5 );
    PivotSet PivotWeek( "pvWk", pBarsForPeriod );
    AddToMap( PivotWeek );
    delete pBarsForPeriod;
  }
  if ( pBars->Size() >= 20 ) {
    pBarsForPeriod = pBars->Subset( ptime( dtMonthAgo ), 20 );
    PivotSet Pivot20Bars( "pv20B", pBarsForPeriod );
    AddToMap( Pivot20Bars );
    delete pBarsForPeriod;
  }
  if ( pBars->Size() >= 42 ) {
    pBarsForPeriod = pBars->Subset( ptime( dtPrevMonth ), 20 ); 
    PivotSet PivotMonth( "pvMn", pBarsForPeriod );
    AddToMap( PivotMonth );
    delete pBarsForPeriod;
  }
  if ( pBars->Size() >= 200 ) {
    pBarsForPeriod = pBars->Subset( ptime( dt200BarsAgo ), 200 );
    PivotSet Pivot200Bars( "pv200B", pBarsForPeriod );
    AddToMap( Pivot200Bars );
    delete pBarsForPeriod;
  }

}

} // namespace tf
} // namespace ou
