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
 * File:    Colours.hpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: December 7, 2022 12:30:37
 */

#pragma once

#include <OUCommon/Colour.h>

using EColour = ou::Colour::wx::EColour;

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

struct Colours {

  EColour bg; // box fill
  EColour fg; // text
  EColour hi; // box fill to highlight

  Colours( EColour bg_, EColour fg_, EColour hi_)
  : bg( bg_ ), fg( fg_ ), hi( hi_ ) {}

  //Colours( EColour bg_, EColour fg_ ): Colours( bg_, fg_ , EColour::DimGray ) {}
  Colours(): Colours( EColour::White, EColour::Black, EColour::DimGray ) {}

  const Colours& operator=( const Colours& rhs ) {
    if ( this != &rhs ) {
      bg = rhs.bg; fg = rhs.fg; hi = rhs.hi;
    }
    return *this;
  }

};

} // namespace market depth
} // namespace tf
} // namespace ou
