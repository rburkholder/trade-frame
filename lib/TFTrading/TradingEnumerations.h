/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#pragma once

#include <string>

namespace ou { // One Unified
namespace tf { // TradeFrame

namespace InstrumentType { // IBTWS.cpp, HDF5 Attributes depend on this order
  enum enumInstrumentType { Unknown=0, Stock, Option, Future, FuturesOption, Currency, Index, ETF, Bond, Commodity, Metal, _Count };
  extern const char* Name[];
}

namespace OrderSide {
  enum enumOrderSide { Unknown=0, Buy, Sell, SellShort, BuyMinus, SellPlus, BuyStop, SellStop, _Count };
  extern const char* Name[];
}

namespace OrderStatus {
  enum enumOrderStatus { Created=0, SendingToProvider, PreSubmission, Rejected, Submitted,
    Filling, CancelSubmitted, FillingDuringCancel, Filled, Cancelled, CancelledWithPartialFill, OverFilled };
}

namespace OptionSide {
  enum enumOptionSide { Unknown=0, Put = 'P', Call = 'C', _Count = 2 };
  extern const char* Name[];
  extern const char* LongName[];
  extern const char* ShortName[];
}

namespace OptionStyle {
  enum enumOptionStyle { American, European, _Count };
}

namespace OrderType { // ib szOrderType depends upon order
  enum enumOrderType { Unknown=0, Market, Limit, Stop, StopLimit, MarketIfTouched, Trail, TrailLimit, MarketClose, LimitClose, Scale, _Count  };
}

namespace OrderError {
  enum enumOrderError { Unknown=0, Rejected=0, Cancelled, NotCancellable, InstrumentNotFound };
}

namespace TimeInForce {
  enum enumTimeInForce { Unknown=0, Day, GoodTillCancel, AtTheOpening, ImmediateOrCancel, FillOrKill, GoodTillCrossing, GoodTillDate, AtTheClose, _Count };
}

namespace Currency {
  // http://en.wikipedia.org/wiki/ISO_4217
  // http://en.wikipedia.org/wiki/ISO_3166-1_alpha-2
  typedef std::string type;
  // enumeration is subject to change, so do not use enumeration in persistent storage
  enum enumCurrency { USD, GBP, CAD, CHF, HKD, JPY, EUR, KRW, LTL, AUD, CZK, DKK, NZD, HUF, ILS, _Count };
  extern const char* Name[];
}

  // * instrument dialog may be limited to a subset of instruments
  // * menu tree presents certain options depending upon what is allowed
  // * need to get this out of here for more generic use, maybe in the enumerations file
namespace Allowed {
  enum enumInstrument { All, Basic, Options, FuturesOptions, None };
}

// currency pair    base/quote(aka counter)  - depicts how many units of the counter currency are needed to buy one unit of the base currency.
// http://en.wikipedia.org/wiki/Currency_pair
// bigger number, base is stronger
// smaller number, base is weaker

} // namespace tf
} // namespace ou
