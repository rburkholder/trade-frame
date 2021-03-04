/************************************************************************
 * Copyright(c) 2020, One Unified. All rights reserved.                 *
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

/*
 * File:    Collar.h
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created on July 19, 2020, 05:43 PM
 */

#ifndef COLLAR_H
#define COLLAR_H

#include <TFTrading/MonitorOrder.h>

#include "Combo.h"
#include "Tracker.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Collar: public Combo {
public:

  Collar();
  Collar( Collar&& );
  Collar( const Collar& ) = delete;
  Collar& operator=( const Collar& rhs ) = delete;
  virtual ~Collar();

  static size_t LegCount();

  static void ChooseLegs( // throw Chain exceptions
    Combo::E20DayDirection direction,
    const mapChains_t& chains,
    boost::gregorian::date,
    double priceUnderlying,
    fLegSelected_t&& );

  static void FillLegNote( size_t ix, Combo::E20DayDirection, LegNote::values_t& );

  static const std::string Name( const std::string& sUnderlying, const mapChains_t& chains, boost::gregorian::date date, double price, Combo::E20DayDirection );

  virtual void Tick( double dblUnderlyingSlope, double dblUnderlyingPrice, ptime dt );

  // long by default for entry, short doesn't make much sense due to combo type
  virtual void PlaceOrder( ou::tf::OrderSide::enumOrderSide, uint32_t nOrderQuantity );
  virtual void PlaceOrder( ou::tf::OrderSide::enumOrderSide, uint32_t nOrderQuantity, LegNote::Type ); // needed?

  virtual void CancelOrders();
  virtual void GoNeutral();
  virtual void AtClose();

protected:
  virtual void Init( boost::gregorian::date, const mapChains_t* );
private:

  using fTest_t = std::function<void(double,double)>; // underlying slope, price
  using vfTest_t = std::vector<fTest_t>;

  struct CollarLeg {
    Tracker m_tracker;
    ou::tf::MonitorOrder m_monitor; // used for closing, for now
    vfTest_t vfTest; // functions to test & process leg
  };

  using mapCollarLeg_t = std::map<LegNote::Type,CollarLeg>;
  mapCollarLeg_t m_mapCollarLeg;

  CollarLeg& InitTracker(
    LegNote::Type type,
    const mapChains_t* pmapChains,
    boost::gregorian::date date,
    boost::gregorian::days days_to_expiry
  );

  void InitTrackLongOption(
    LegNote::Type type,
    const mapChains_t* pmapChains,
    boost::gregorian::date date,
    boost::gregorian::days days_to_expiry
    );

  void InitTrackShortOption(
    LegNote::Type type,
    const mapChains_t* pmapChains,
    boost::gregorian::date date,
    boost::gregorian::days days_to_expiry
    );

};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* COLLAR_H */
