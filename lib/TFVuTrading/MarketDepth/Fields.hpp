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
 * File:    Fields.hpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: December 12, 2022 12:50:57
 */

#pragma once

#include <string>
#include <vector>

#include <OUCommon/Colour.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

using EColour = ou::Colour::wx::EColour;

struct Element { // definition of an element in a gui row
  int field;
  int width;
  std::string header;
  long alignment; // passed as style
  EColour colourBackground; // box fill
  EColour colourForeground; // text
  EColour colourHighlight;  // box fill to highlight
};

using vElement_t = std::vector<Element>;  // TODO: convert to std::array?

namespace rung { // columns/elements/fields in a row/rung of the ladder

// PanelTrade Columns/Fields/Elements

enum class EField: int { // may need a different namespace or name to reflect panel trade only
  PL,
  BuyCount, BuyVolume,
  BidSize,
  BidOrder, Price, AskOrder,
  AskSize,
  SellVolume, SellCount,
  Ticks, Volume, Static, Dynamic
};

extern const vElement_t vElement;

} // namespace rung
} // namespace market depth
} // namespace tf
} // namespace ou
