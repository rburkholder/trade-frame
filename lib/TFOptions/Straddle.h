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
 * File:    Straddle.h
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 23, 2019, 10:50 AM
 */

#ifndef STRADDLE_H
#define STRADDLE_H

#include "Combo.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Straddle: public Combo {
public:

  Straddle();
  Straddle( const Straddle& rhs ) = delete;
  Straddle& operator=( const Straddle& rhs ) = delete;
  Straddle( Straddle&& rhs );
  virtual ~Straddle();

  //virtual void Tick( bool bInTrend, double dblPriceUnderlying, ptime dt );

  //virtual void PlaceOrder( ou::tf::OrderSide::enumOrderSide ); // long strangle, or short strangle

  //void CloseItmLegForProfit( double price, EOrderSide defaultOrderSide, fBuildLeg_t&& );

  //virtual double GetNet( double price );

  //using LegDef = Combo::LegDef;
  using leg_pair_t = Combo::leg_pair_t;
  using strike_pair_t = Combo::strike_pair_t;

  virtual strike_pair_t ChooseStrikes( const Chain& chain, double price ) const; // throw Chain exceptions

  static const leg_pair_t m_legDefLong;
  static const leg_pair_t m_legDefShort;

private:

  void Init();

};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* STRADDLE_H */

