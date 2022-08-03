/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    Symbol.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFPhemex
 * Created: July 24, 2022 12:59:37
 */

#include <OUCommon/TimeSource.h>

#include <TFTimeSeries/DatedDatum.h>

#include "Symbol.hpp"

namespace ou {
namespace tf {
namespace phemex {

Symbol::Symbol( const idSymbol_t& sSymbol, pInstrument_t pInstrument )
: ou::tf::Symbol<Symbol>( pInstrument, sSymbol )
{
}

Symbol::~Symbol() {
}

void Symbol::HandleTrade( const gateway::trades::trade& trade ) {
  boost::posix_time::ptime dt( ou::TimeSource::GlobalInstance().External() );
  ou::tf::Trade tf_trade( dt, (double)trade.price, trade.quantity );
  m_OnTrade( tf_trade );
}

} // namespace phemex
} // namespace tf
} // namespace ou
