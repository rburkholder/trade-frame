/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    TrackOrder.cpp
 * Author:  raymond@burkholder.net
 * Project: TFBitsNPieces
 * Created: August 17, 2025 16:45:42
 */

#include "TrackOrder.hpp"

namespace ou { // namespace oneunified
namespace tf { // namespace tradeframe

TrackOrder::TrackOrder()
: TrackOrderBase()
{}

TrackOrder::TrackOrder( pPosition_t pPosition, ou::ChartDataView& cdv, int slot )
: TrackOrderBase( pPosition, cdv, slot )
{}

TrackOrder::~TrackOrder() {}

} // namespace tradeframe
} // namespace oneunified

