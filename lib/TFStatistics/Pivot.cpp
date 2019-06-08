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
  m_dblHiLoRangeAvg {},
  m_dblR2 {}, m_dblR1 {}, m_dblPV {}, m_dblS1 {}, m_dblS2 {}
{
  m_vrItemsOfInterest.reserve( bars.Size() );

  size_t nPivots {};
  ou::tf::PivotSet ps;
  double dblHiLoRangeSum {};

  std::for_each( bars.begin(), bars.end(),
    [this,&ps,&nPivots,&dblHiLoRangeSum](const ou::tf::Bar& bar ){

      if ( 0 < nPivots ) { // current bar against previously calculated pivot set, skip first time through
        rItemsOfInterest_t rItemsOfInterest = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        m_dblR2 = ps.GetPivotValue( ou::tf::PivotSet::R2 );
        m_dblR1 = ps.GetPivotValue( ou::tf::PivotSet::R1 );
        m_dblPV = ps.GetPivotValue( ou::tf::PivotSet::PV );
        m_dblS1 = ps.GetPivotValue( ou::tf::PivotSet::S1 );
        m_dblS2 = ps.GetPivotValue( ou::tf::PivotSet::S2 );

        bool bCrossR1 = ( bar.High() > m_dblR1 ) && ( bar.Low() < m_dblR1 );
        bool bCrossPV = ( bar.High() > m_dblPV ) && ( bar.Low() < m_dblPV );
        bool bCrossS1 = ( bar.High() > m_dblS1 ) && ( bar.Low() < m_dblS1 );

        bool bAbovePV = bar.Open() > m_dblPV;
        bool bBelowPV = bar.Open() < m_dblPV;

        bool bBtwnR1PV = bAbovePV && ( bar.Open() < m_dblR1 );
        bool bBtwnS1PV = bBelowPV && ( bar.Open() > m_dblS1 );

        m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::AbovePV ].nPopulation += 1;
        if ( bAbovePV ) {
            rItemsOfInterest[    (size_t)EItemsOfInterest::AbovePV ] = 1;
          m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::AbovePV ].nEncountered += 1;

          m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::AbovePV_X_Down ].nPopulation += 1;
          if ( bCrossPV ) {
              rItemsOfInterest[    (size_t)EItemsOfInterest::AbovePV_X_Down ] = 1;
            m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::AbovePV_X_Down ].nEncountered += 1;
          }

          m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::AbovePV_X_S1 ].nPopulation += 1;
          if ( bCrossPV && bCrossS1 ) {
              rItemsOfInterest[    (size_t)EItemsOfInterest::AbovePV_X_S1 ] = 1;
            m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::AbovePV_X_S1 ].nEncountered += 1;
          }

          if ( bBtwnR1PV ) {
            m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::BtwnPVR1_X_Down ].nPopulation += 1;
            if ( bCrossPV ) {
                rItemsOfInterest[    (size_t)EItemsOfInterest::BtwnPVR1_X_Down ] = 1;
              m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::BtwnPVR1_X_Down ].nEncountered += 1;
            }
            m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::BtwnPVR1_X_Up ].nPopulation += 1;
            if ( bCrossR1 ) {
                rItemsOfInterest[    (size_t)EItemsOfInterest::BtwnPVR1_X_Up ] = 1;
              m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::BtwnPVR1_X_Up ].nEncountered += 1;
            }
          }
        }

        m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::BelowPV ].nPopulation += 1;
        if ( bBelowPV ) {
            rItemsOfInterest[    (size_t)EItemsOfInterest::BelowPV ] = 1;
          m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::BelowPV ].nEncountered += 1;

          m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::BelowPV_X_Up ].nPopulation += 1;
          if ( bCrossPV ) {
              rItemsOfInterest[    (size_t)EItemsOfInterest::BelowPV_X_Up ] = 1;
            m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::BelowPV_X_Up ].nEncountered += 1;
          }

          m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::BelowPV_X_R1 ].nPopulation += 1;
          if ( bCrossPV && bCrossR1 ) {
              rItemsOfInterest[    (size_t)EItemsOfInterest::BelowPV_X_R1 ] = 1;
            m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::BelowPV_X_R1 ].nEncountered += 1;
          }

          if ( bBtwnS1PV ) {
            m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::BtwnPVS1_X_Up ].nPopulation += 1;
            if ( bCrossPV ) {
                rItemsOfInterest[    (size_t)EItemsOfInterest::BtwnPVS1_X_Up ] = 1;
              m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::BtwnPVS1_X_Up ].nEncountered += 1;
            }
            m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::BtwnPVS1_X_Down ].nPopulation += 1;
            if ( bCrossS1 ) {
                rItemsOfInterest[    (size_t)EItemsOfInterest::BtwnPVS1_X_Down ] = 1;
              m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::BtwnPVS1_X_Down ].nEncountered += 1;
            }
          }
        }

        m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::CrossPV ].nPopulation += 1;
        if ( bCrossPV ) {
            rItemsOfInterest[    (size_t)EItemsOfInterest::CrossPV ] = 1;
          m_rItemsOfInterestSum[ (size_t)EItemsOfInterest::CrossPV ].nEncountered += 1;
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

void Pivot::Points( double& dblR2, double& dblR1, double& dblPV, double& dblS1, double& dblS2 ) {
  dblR2 = m_dblR2;
  dblR1 = m_dblR1;
  dblPV = m_dblPV;
  dblS1 = m_dblS1;
  dblS2 = m_dblS2;
}

double Pivot::ItemOfInterest( EItemsOfInterest ioi ) const {
  const ItemOfInterestRaw& ioir( m_rItemsOfInterestSum[ (size_t)ioi ] );
  return 0 == ioir.nPopulation ? 0 : (double)ioir.nEncountered / (double)ioir.nPopulation;
}

} // namespace statistics
} // namespace tf
} // namespace ou
