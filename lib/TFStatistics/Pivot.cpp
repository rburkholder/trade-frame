/*
 * File:   Pivot.cpp
 * Author:    raymond@burkholder.net
 * Project:   TFStatistics
 * copyright: 2019 Raymond Burkholder
 * License:   GPL3
 *
 * Created on March 20, 2019, 2:57 PM
 */

#include <algorithm>

#include <TFIndicators/Pivots.h>

#include "Pivot.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace statistics {

Pivot::Pivot( const ou::tf::Bars& bars )
: m_dblHiLoRangeStdDev {},
  m_dblHiLoRangeAvg {}
{
  m_vrItemsOfInterest.reserve( bars.Size() );
  m_rItemsOfInterestSum = { 0, 0, 0 };

  size_t nPivots {};
  ou::tf::PivotSet ps;
  double dblHiLoRangeSum {};

  std::for_each( bars.begin(), bars.end(),
    [this,&ps,&nPivots,&dblHiLoRangeSum](const ou::tf::Bar& bar ){

      if ( 0 < nPivots ) { // current bar against previously calculated pivot set, skip first time through
        rItemsOfInterest_t rItemsOfInterest = { 0, 0, 0 };
        double pv = ps.GetPivotValue( ou::tf::PivotSet::PV );
        if ( bar.Open() > pv ) {
            rItemsOfInterest[    (size_t)EItemsOfInterest::AbovePV ]  = 1;
          m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::AbovePV ] += 1;
          if ( ( bar.High() > pv ) && ( bar.Low() < pv ) ) {
              rItemsOfInterest[    (size_t)EItemsOfInterest::AbovePV_X_Down ]  = 1;
            m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::AbovePV_X_Down ] += 1;
          }
        }

        if ( bar.Open() < pv ) {
            rItemsOfInterest[    (size_t)EItemsOfInterest::BelowPV ]  = 1;
          m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::BelowPV ] += 1;
          if ( ( bar.High() > pv ) && ( bar.Low() < pv ) ) {
              rItemsOfInterest[    (size_t)EItemsOfInterest::BelowPV_X_Up ]  = 1;
            m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::BelowPV_X_Up ] += 1;
          }
        }

        if ( ( bar.High() > pv ) && ( bar.Low() < pv ) ) {
            rItemsOfInterest[    (size_t)EItemsOfInterest::CrossPV ]  = 1;
          m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::CrossPV ] += 1;
        }

        m_vrItemsOfInterest.push_back( rItemsOfInterest );
      }

      ps.CalcPivots( bar );
      nPivots++;
      dblHiLoRangeSum += ( bar.High() - bar.Low() );

    });

  m_dblHiLoRangeAvg = dblHiLoRangeSum / (double)nPivots;

  double dblHiLoRangeDiffs {};

  std::for_each( bars.begin(), bars.end(),
    [this,&dblHiLoRangeDiffs](const ou::tf::Bar& bar){
      double diff = ( bar.High() - bar.Low() ) - m_dblHiLoRangeAvg;
      dblHiLoRangeDiffs += diff * diff;
    });

  m_dblHiLoRangeStdDev = std::sqrt( dblHiLoRangeDiffs / (double)nPivots );

}

Pivot::~Pivot( ) { }

double Pivot::ItemOfInterest( EItemsOfInterest ioi ) const {
  return (double)m_rItemsOfInterestSum[ (size_t)ioi ] / (double)m_vrItemsOfInterest.size();
}

} // namespace statistics
} // namespace tf
} // namespace ou
