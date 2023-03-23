/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    Order_Bracket.hpp
 * Author:  raymond@burkholder.net
 * Project: TFTrading
 * Created: March 22, 2023 13:38:52
 */

#include "Order.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class OrderBracket {
public:

  using pOrder_t = Order::pOrder_t;

  OrderBracket();
  ~OrderBracket();

  void Add( pOrder_t );

protected:
private:
};

} // namespace tf
} // namespace ou
