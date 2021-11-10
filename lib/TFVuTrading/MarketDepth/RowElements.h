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
 * File:    RowElements.h
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: November 9, 2021 16:53
 */

#include <vector>

#include <wx/window.h>

#include "WinRowElement.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

class RowElements {
public:

  enum class Field { SellVol = 0, Bid, Price, Ask, BuyVol, Ticks, Volume, Static, Dynamic };

  RowElements( wxWindow* pParent, const wxPoint& origin, int nRowHeight, bool bIsHeader );
  ~RowElements();

  static int RowWidth();

protected:
private:

  wxWindow* m_pParentWindow;

  using vElements_t = std::vector<WinRowElement*>;
  vElements_t m_vElements;

  void Clear();
  void Create( const wxPoint& origin, int nRowHeight, bool bIsHeader );

};

} // market depth
} // namespace tf
} // namespace ou
