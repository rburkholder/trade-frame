/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"
#include "StrategyTradeOptions.h"

StrategyTradeOptions::StrategyTradeOptions( pProvider_t pExecutionProvider, pProvider_t pData1Provider, pProvider_t pData2Provider ) {
}

StrategyTradeOptions::~StrategyTradeOptions(void) {
}

void StrategyTradeOptions::Start( const std::string& sUnderlying, boost::gregorian::date dtOptionNearDate, boost::gregorian::date dtOptionFarDate ) {
  assert( dtOptionNearDate < dtOptionFarDate );
  assert( "" != sUnderlying );
}