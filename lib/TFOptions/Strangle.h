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
 * Project: TFOptions
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
  Strangle( const Strangle& rhs ) = delete;
  Strangle& operator=( const Strangle& rhs ) = delete;
  Strangle( Strangle&& rhs );
  virtual ~Strangle();

  virtual void Tick( bool bInTrend, double dblPriceUnderlying, ptime dt );

  using pOption_t = ou::tf::option::Option::pOption_t;
  using pOptionPair_t = std::pair<pOption_t,pOption_t>;
  bool ValidateSpread( ConstructionTools&, double price, size_t nDuration );
  pOptionPair_t ValidatedOptions();

  virtual void PlaceOrder( ou::tf::OrderSide::enumOrderSide ); // long strangle, or short strangle

  void CloseItmLegForProfit( double price, EOrderSide defaultOrderSide, fBuildLeg_t&& );

private:

  void Init();

};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* STRANGLE_H */
