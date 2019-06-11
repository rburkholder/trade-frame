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
 * File:    Condor.h
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 10, 2019, 6:24 PM
 */

#ifndef CONDOR_H
#define CONDOR_H

#include "Combo.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Condor: public Combo {
public:
  Condor();
  Condor( const Condor& rhs ) = delete;
  Condor& operator=( const Condor& rhs ) = delete;
  Condor( Condor&& rhs );
  virtual ~Condor();

  virtual void Tick( bool bInTrend, double dblPriceUnderlying, ptime dt );

  using pOption_t = ou::tf::option::Option::pOption_t;
  using pOptionPair_t = std::pair<pOption_t,pOption_t>;
//  bool ValidateSpread( ConstructionTools&, double price, size_t nDuration );
//  pOptionPair_t ValidatedOptions();

  virtual void PlaceOrder( ou::tf::OrderSide::enumOrderSide ); // long strangle, or short strangle

protected:
private:

};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* CONDOR_H */




