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
 * File:    StrategyStrangle.h
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on June 23, 2019, 9:33 PM
 */

#ifndef STRATEGYSTRANGLE_H
#define STRATEGYSTRANGLE_H

#include <TFOptions/Strangle.h>

#include "StrategyBase.h"

class StrategyStrangle: public StrategyBase {
public:
  StrategyStrangle( );
  StrategyStrangle( const StrategyStrangle&& orig );
  virtual ~StrategyStrangle( );
protected:
private:

};

#endif /* STRATEGYSTRANGLE_H */

