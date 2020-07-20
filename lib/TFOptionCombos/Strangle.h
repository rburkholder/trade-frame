/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
 * File:    Strangle.h
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created on May 25, 2019, 10:56 PM
 */

#ifndef STRANGLE_H
#define STRANGLE_H

#include "Combo.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Strangle: public Combo {
public:

  Strangle();
  Strangle( const Strangle& rhs );
  Strangle& operator=( const Strangle& rhs ) = delete;
  Strangle( const Strangle&& rhs );
  virtual ~Strangle();

  virtual void Tick( bool bInTrend, double dblPriceUnderlying, ptime dt );

  virtual void PlaceOrder( ou::tf::OrderSide::enumOrderSide ); // long strangle, or short strangle

  void CloseItmLegForProfit( double price, Combo::EOrderSide defaultOrderSide, fBuildLeg_t&& );

  virtual double GetNet( double price );

  static void ChooseLegs(
    const mapChains_t& chains, boost::gregorian::date, double price, fLegSelected_t&& ); // throw Chain exceptions

  static size_t LegCount();

  static const std::string Name( const std::string& sUnderlying, const mapChains_t& chains, boost::gregorian::date date, double price );

private:

  void CheckStop( double price );

};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* STRANGLE_H */
