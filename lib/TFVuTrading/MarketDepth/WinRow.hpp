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

#include <memory>
#include <vector>

class wxPoint;
class wxWindow;

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

class WinRowElement;

class WinRow {
public:

  WinRow( wxWindow* parent, const wxPoint& origin, int RowHeight, bool bIsHeader );
  ~WinRow();

  using pWinRow_t = std::shared_ptr<WinRow>;
  static pWinRow_t Construct( wxWindow* parent, const wxPoint& origin, int RowHeight, bool bIsHeader );

  enum class EField: int {
    BuyCount, BuyVolume,
    BidSize, Price, AskSize,
    SellVolume, SellCount,
    Ticks, Volume, Static, Dynamic
    };

  void HighlightBid( bool );
  void HighlightAsk( bool );
  void HighlightPrice( bool );

  static int RowWidth();

  WinRowElement* operator[]( EField );

protected:
private:

  using vWinRowElement_t = std::vector<WinRowElement*>;
  vWinRowElement_t m_vWinRowElement;

  void Clear();
};

} // market depth
} // namespace tf
} // namespace ou
