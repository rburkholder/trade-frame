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
 * File:    SyntheticLong.h
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 23, 2019, 10:49 AM
 */

#ifndef SYNTHETICLONG_H
#define SYNTHETICLONG_H

#include "Combo.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options
namespace spread { // spread

class SyntheticLong: public ou::tf::option::Combo {
public:
  SyntheticLong( );
  SyntheticLong( const SyntheticLong& rhs ) = delete;
  SyntheticLong& operator=( const SyntheticLong& rhs ) = delete;
  SyntheticLong( SyntheticLong&& rhs );
  virtual ~SyntheticLong( );

  //using LegDef = Combo::LegDef;
  //using leg_pair_t = Combo::leg_pair_t;
  //using strike_pair_t = Combo::strike_pair_t;

  //using pOption_t = ou::tf::option::Option::pOption_t;
  //using pOptionPair_t = std::pair<pOption_t,pOption_t>; // higher lower

//  virtual void Tick( bool bInTrend, double dblPriceUnderlying, ptime dt );

  virtual strike_pair_t ChooseStrikes( const Chain& chain, double price ); // call, put: throw Chain exceptions

  //static const leg_pair_t m_legDefLong;
  //static const leg_pair_t m_legDefShort;

protected:
private:

};

} // namespace spread
} // namespace option
} // namespace tf
} // namespace ou


#endif /* SYNTHETICLONG_H */

