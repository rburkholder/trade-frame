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
 * File:    VerticalSpread.h
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 11, 2019, 8:03 PM
 */

#ifndef VERTICALSPREAD_H
#define VERTICALSPREAD_H

#include "Combo.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class VerticalSpread: public Combo {
public:

  VerticalSpread();
  VerticalSpread( const VerticalSpread& rhs ) = delete;
  VerticalSpread& operator=( const VerticalSpread& rhs ) = delete;
  VerticalSpread( VerticalSpread&& rhs );
  virtual ~VerticalSpread();

  virtual void Tick( bool bInTrend, double dblPriceUnderlying, ptime dt );

  using pOption_t = ou::tf::option::Option::pOption_t;
  using pOptionPair_t = std::pair<pOption_t,pOption_t>;
//  bool ValidateSpread( ConstructionTools&, double price, size_t nDuration );
//  pOptionPair_t ValidatedOptions();

  virtual void PlaceOrder( ou::tf::OrderSide::enumOrderSide ); // long vertical, or short vertical

protected:
private:

};

class ShortVerticalSpread: public VerticalSpread {
public:
  ShortVerticalSpread();
  ShortVerticalSpread( const ShortVerticalSpread& rhs ) = delete;
  ShortVerticalSpread& operator=( const ShortVerticalSpread& rhs ) = delete;
  ShortVerticalSpread( ShortVerticalSpread&& rhs );
  virtual ~ShortVerticalSpread();

  virtual void Tick( bool bInTrend, double dblPriceUnderlying, ptime dt );

  virtual void PlaceOrder();  // place short vertical spread
protected:
private:
};

class LongVerticalSpread: public VerticalSpread {
public:
  LongVerticalSpread();
  LongVerticalSpread( const VerticalSpread& rhs ) = delete;
  LongVerticalSpread& operator=( const LongVerticalSpread& rhs ) = delete;
  LongVerticalSpread( LongVerticalSpread&& rhs );
  virtual ~LongVerticalSpread();

  virtual void Tick( bool bInTrend, double dblPriceUnderlying, ptime dt );

  virtual void PlaceOrder();  // place long vertical spread
protected:
private:
};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* VERTICALSPREAD_H */

