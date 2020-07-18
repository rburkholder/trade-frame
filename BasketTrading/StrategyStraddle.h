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
 * File:    StrategyStraddle.h
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on July 4, 2019, 11:09 AM
 */

#ifndef STRATEGYSTRADDLE_H
#define STRATEGYSTRADDLE_H

#include <TFOptions/Straddle.h>

#include "StrategyBase.h"

namespace Strategy {

class Straddle: public Base<Straddle, ou::tf::option::Straddle> {
public:

  Straddle( );
  Straddle( const Straddle&& );
  virtual ~Straddle( );

protected:
private:

  //using leg_pair_t = Combo::leg_pair_t;
  //using strike_pair_t = ou::tf::option::Combo::strike_pair_t;

  //static const leg_pair_t m_legDefLong;
  //static const leg_pair_t m_legDefShort;

};

} // namespace Strategy

#endif /* STRATEGYSTRADDLE_H */
