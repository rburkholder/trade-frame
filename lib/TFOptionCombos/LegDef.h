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
 * File:    LegDef.h
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on July 4, 2019, 7:32 PM
 */

#ifndef LEGDEF_H
#define LEGDEF_H

#include "LegNote.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

struct LegDef { // generic definition of each leg of the combo

  LegNote::Type type;
  LegNote::Side side;
  LegNote::Option option;
  uint32_t quantity;

  LegDef( uint32_t quantity_, LegNote::Type type_, LegNote::Side side_, LegNote::Option option_ )
  : quantity( quantity_ ), type( type_ ), side( side_ ), option( option_ ) {}

};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* LEGDEF_H */

