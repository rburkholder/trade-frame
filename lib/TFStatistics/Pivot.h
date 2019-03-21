/*
 * File:   Pivot.h
 * Author:    raymond@burkholder.net
 * Project:   TFStatistics
 * copyright: 2019 Raymond Burkholder
 * License:   GPL3
 *
 * Created on March 20, 2019, 2:57 PM
 */

// goal:
//   given pivots, calculate what happens with following bar within the pivot
//   then based upon the opening,
//     check against the history to determine probability of a successful directional trade

#ifndef PIVOT_H
#define PIVOT_H

#include <array>
#include <vector>

#include <TFTimeSeries/TimeSeries.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace statistics {

class Pivot {
public:

  enum class EItemsOfInterest {
    AbovePV = 0,    // open above PV
    BelowPV,        // open below PV
    AbovePV_X_Down, // start above PV crossing downwards
    CrossPV,        // cross PV during session
    BelowPV_X_Up,   // start below PV crossing upwards
    Count
  };

  Pivot( const ou::tf::Bars& );
  virtual ~Pivot( );

  double ItemOfInterest( EItemsOfInterest ) const;  // normalized [0.0 .. 1.0]

protected:
private:

  using ts_size_t = ou::tf::Bars::size_type;

  using rItemsOfInterest_t = std::array<unsigned char,(size_t)EItemsOfInterest::Count>;

  std::vector<rItemsOfInterest_t> m_vrItemsOfInterest;

  rItemsOfInterest_t m_rItemsOfInterestSum;

  double m_dblHiLoRangeAvg;  // use as trailing stop?
  double m_dblHiLoRangeStdDev; // use to calculate range percentiles (indicates relative distance between pivot markers

};

} // namespace statistics
} // namespace tf
} // namespace ou

#endif /* PIVOT_H */

