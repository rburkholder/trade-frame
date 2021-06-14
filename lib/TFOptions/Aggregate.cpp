/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    Aggregate.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on May 29, 2021, 20:09
 */

#include "Aggregate.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

Aggregate::Aggregate(
  pWatch_t pWatchUnderlying
)
: m_pWatchUnderlying( pWatchUnderlying )
{
}

void Aggregate::LoadChains( fGatherOptionDefinitions_t& f ) {
  ou::tf::option::PopulateMap<mapChains_t>( m_mapChains, m_pWatchUnderlying->GetInstrument()->GetInstrumentName(), f );
}

} // namespace option
} // namespace tf
} // namespace ou
