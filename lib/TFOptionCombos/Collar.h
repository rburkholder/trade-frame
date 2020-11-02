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

#include "Combo.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Collar: public Combo {
public:

  Collar();
  Collar( const Collar& rhs );
  Collar& operator=( const Collar& rhs ) = delete;
  Collar( const Collar&& rhs );
  virtual ~Collar();

  static size_t LegCount();

  static void ChooseLegs( // throw Chain exceptions
    double slope,
    const mapChains_t& chains,
    boost::gregorian::date,
    double priceUnderlying,
    fLegSelected_t&& );

  static const std::string Name( const std::string& sUnderlying, const mapChains_t& chains, boost::gregorian::date date, double price, double slope );

  virtual void Tick( bool bInTrend, double dblPriceUnderlying, ptime dt );

  virtual void PlaceOrder( double slope20Day, ou::tf::OrderSide::enumOrderSide );
  // long by default for entry,
  // short doesn't make much sense at this point for entry

  virtual double GetNet( double price );

protected:
private:

  ou::tf::Leg m_itmCall;
  ou::tf::Leg m_itmPut;

};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* COLLAR_H */
