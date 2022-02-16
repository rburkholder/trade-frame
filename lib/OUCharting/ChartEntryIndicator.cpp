/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include "ChartEntryIndicator.h"

namespace ou { // One Unified

ChartEntryIndicator::ChartEntryIndicator()
: ChartEntryPrice()
{}

ChartEntryIndicator::ChartEntryIndicator( ChartEntryIndicator&& rhs )
: ChartEntryPrice( std::move( rhs ) )
{}

//ChartEntryIndicator::ChartEntryIndicator( size_type nSize )
//: ChartEntryBaseWithTime( nSize )
//{
//}

ChartEntryIndicator::~ChartEntryIndicator() {
}

} // namespace ou
