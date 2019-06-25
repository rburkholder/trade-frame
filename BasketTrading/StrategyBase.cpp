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
 * File:    StrategyBase.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on June 23, 2019, 9:28 PM
 */

#include "StrategyBase.h"

StrategyBase::StrategyBase( )
{
}

StrategyBase::StrategyBase( const StrategyBase&& rhs )
{
}

StrategyBase::~StrategyBase( ) {
}

StrategyBase::citerChain_t StrategyBase::SelectChain( const mapChains_t& mapChains, boost::gregorian::date date, boost::gregorian::days daysToExpiry ) {
  citerChain_t citerChain = std::find_if( mapChains.begin(), mapChains.end(),
    [date,daysToExpiry](const mapChains_t::value_type& vt)->bool{
      return daysToExpiry <= ( vt.first - date );  // first chain where trading date less than expiry date
  } );
  if ( mapChains.end() == citerChain ) {
    throw exception_chain_no_found( "StrategyBase::SelectChain" );
  }
  return citerChain;
}