/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include <string>

#pragma once

namespace ou { // One Unified
namespace tf { // TradeFrame

namespace InstrumentType { // IBTWS.cpp, HDF5 Attributes depend on this order
  enum enumInstrumentTypes : char { Unknown=0, Stock, Option, Future, FuturesOption, Currency, Index, ETF, Bond, Commodity, Metal, _Count };
  const char* Name[];
}

namespace OrderSide {
  enum enumOrderSide : char { Unknown=0, Buy, Sell, SellShort, BuyMinus, SellPlus, BuyStop, SellStop, _Count };
  const char* Name[];
}

namespace OrderStatus {
  enum enumOrderStatus : char { Created=0, SendingToProvider, PreSubmission, Rejected, Submitted, 
    Filling, CancelSubmitted, FillingDuringCancel, Filled, Cancelled, CancelledWithPartialFill, OverFilled };
}

namespace OptionSide {
  enum enumOptionSide : char { Unknown=0, Put = 'P', Call = 'C', _Count = 2 };
  const char* Name[];
  const char* LongName[];
  const char* ShortName[];
}

namespace OrderType { // ib szOrderType depends upon order
  enum enumOrderType : char { Unknown=0, Market, Limit, Stop, StopLimit, MarketIfTouched, Trail, TrailLimit, MarketClose, LimitClose, Scale, _Count  };
}

namespace OrderErrors {
  enum enumOrderErrors : char { Unknown=0, Rejected=0, Cancelled, NotCancellable, InstrumentNotFound };
}

namespace TimeInForce {
  enum enumTimeInForce : char { Unknown=0, Day, GoodTillCancel, AtTheOpening, ImmediateOrCancel, FillOrKill, GoodTillCrossing, GoodTillDate, AtTheClose, _Count };
}

namespace Currency {
  // http://en.wikipedia.org/wiki/ISO_4217
  // http://en.wikipedia.org/wiki/ISO_3166-1_alpha-2
  enum enumCurrency : char { USD, GBP, CAD, CHF, HKD, JPY, EUR, KRW, LTL, AUD, CYP, CZK, DKK, EEK, NZD, HUF, ILS, _Count };
  const char* Name[];
}

namespace Trading {
  static const std::string DbFileName = "TradeFrame.db4";
}

// currency pair    base/quote(aka counter)  - depicts how many units of the counter currency are needed to buy one unit of the base currency.
// http://en.wikipedia.org/wiki/Currency_pair
// bigger number, base is stronger
// smaller number, base is weaker

} // namespace tf
} // namespace ou

