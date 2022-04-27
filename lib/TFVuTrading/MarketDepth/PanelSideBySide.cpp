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
 * File:    PanelSideBySide.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: April 27, 2022 16:38
 */

#include <wx/sizer.h>
#include <wx/tooltip.h>
#include <wx/dcclient.h>

#include <TFTimeSeries/DatedDatum.h>

#include "WinRow.hpp"
#include "PanelSideBySide.hpp"

namespace {

  //using EColour = ou::Colour::wx::EColour;
  //using EField = ou::tf::l2::DataRow::EField;

  const ou::tf::l2::WinRow::vElement_t vElement = {
  //  { (int)EField::PL,         40, "P/L",     wxRIGHT,  EColour::LightCyan,     EColour::Black, EColour::Cyan          }
  //, { (int)EField::BuyCount,   45, "Ticks",   wxCENTER, EColour::LightSkyBlue,  EColour::Black, EColour::PaleGoldenrod }
  };

} // anonymous

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth


} // market depth
} // namespace tf
} // namespace ou
