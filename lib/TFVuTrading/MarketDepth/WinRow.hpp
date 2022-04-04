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
 * File:    WinRow.hpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created on April 4, 2022  12:14
 */

#pragma once

#include <vector>

#include <OUCommon/Colour.h>

#include "WinRowElement.hpp"

class wxPoint;
class wxWindow;

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

class WinRow {
public:

  WinRow();
  ~WinRow();

  void Construct( wxWindow* parent, const wxPoint& origin, int RowHeight, bool bIsHeader );

  void HighlightBid( bool );
  void HighlightAsk( bool );
  void HighlightPrice( bool );

  static int RowWidth();

protected:
private:

  WinRowElement m_wreAcctPL;
  WinRowElement m_wreBidQuantity;
  WinRowElement m_wrePrice;
  WinRowElement m_wreAskQuantity;
  WinRowElement m_wreTicks;
  WinRowElement m_wreVolume;
  WinRowElement m_wreBuyVolume;
  WinRowElement m_wreSellVolume;
  WinRowElement m_wreStatic;
  WinRowElement m_wreDynamic;

  using vWinRowElement_t = std::vector<WinRowElement*>;
  vWinRowElement_t m_vWinRowElement;

  void ConstructElement( wxWindow* parent, WinRowElement&, bool bIsHeader, const wxPoint&, const wxSize& );

};

} // market depth
} // namespace tf
} // namespace ou
