/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#pragma once

// Started 2013/09/23

#include <TFTrading/DailyTradeTimeFrames.h>

class Trader: public ou::tf::DailyTradeTimeFrame<Trader> {
  friend ou::tf::DailyTradeTimeFrame<Trader>; 
public:
  Trader(void);
  ~Trader(void);
protected:
private:
};

