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
 * File:    StrategyBase.h
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on June 23, 2019, 9:28 PM
 */

#ifndef STRATEGYBASE_H
#define STRATEGYBASE_H

// base class from which specific strategies inherit

class StrategyBase {
public:
  StrategyBase( );
  StrategyBase( const StrategyBase&& orig );
  virtual ~StrategyBase( );
private:

};

#endif /* STRATEGYBASE_H */

