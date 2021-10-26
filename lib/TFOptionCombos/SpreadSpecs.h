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
 * File:    SpreadSpecs.h
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created: October 25, 2021, 19:53
 */

#pragma once

#include <boost/date_time/gregorian/gregorian_types.hpp>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class SpreadSpecs {
public:

  using days_t = boost::gregorian::days;

  double dblEntrySpreadFront;
  double dblEntrySpreadBack;
  boost::gregorian::days nDaysFront;
  boost::gregorian::days nDaysBack;

  SpreadSpecs() // default
  : dblEntrySpreadFront {}, dblEntrySpreadBack {},
    nDaysFront {}, nDaysBack {}
  {}

  SpreadSpecs(
    double dblEntrySpreadFront_, double dblEntrySpreadBack_,
    int nDaysFront_, int nDaysBack__)
  : dblEntrySpreadFront( dblEntrySpreadFront_ ), dblEntrySpreadBack( dblEntrySpreadBack_ ),
    nDaysFront( days_t( nDaysFront_ ) ), nDaysBack( days_t( nDaysBack__ ) )
  {}

protected:
private:
};

} // namespace option
} // namespace tf
} // namespace ou
