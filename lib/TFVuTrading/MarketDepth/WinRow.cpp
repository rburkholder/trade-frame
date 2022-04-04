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
 * File:    WinRow.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created on April 4, 2022  12:14
 */

 #include "WinRow.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

WinRow::WinRow() {
}

WinRow::~WinRow() {
}

void WinRow::Construct( wxWindow* parent, const wxPoint& origin, int RowHeight, bool bIsHeader ) {
}

void WinRow::HighlightBid( bool ) {
}

void WinRow::HighlightAsk( bool ) {
}

void WinRow::HighlightPrice( bool ) {
}

int WinRow::RowWidth() {
  return 0;
}

void WinRow::ConstructElement( wxWindow* parent, WinRowElement&, bool bIsHeader, const wxPoint&, const wxSize& ) {
}

} // market depth
} // namespace tf
} // namespace ou
