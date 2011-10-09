/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include "MarketActivity.h"

namespace rtd { // real time data processing

sc::result StateInitialization::react( const EvInitialize& event ) {
  return transit<StatePreMarket>();  //  can a determination be made as to which to transit to?
  //return transit<StateMarketOpen();  // 
}

}; // namespace rtd
