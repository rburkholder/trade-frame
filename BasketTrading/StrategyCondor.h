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
 * File:    StrategyCondor.h
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on June 23, 2019, 9:34 PM
 */

#ifndef STRATEGYCONDOR_H
#define STRATEGYCONDOR_H

#include <TFOptions/Condor.h>

#include "StrategyBase.h"

namespace Strategy {

class StrategyCondor: public Base<StrategyCondor, ou::tf::option::Condor> {
public:

  using Condor = ou::tf::option::Condor;

  StrategyCondor( );
  StrategyCondor( const StrategyCondor&& );
  virtual ~StrategyCondor( );
protected:
private:

};

} // namespace Strategy

#endif /* STRATEGYCONDOR_H */
