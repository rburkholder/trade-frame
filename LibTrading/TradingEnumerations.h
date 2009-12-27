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

#pragma once

namespace InstrumentType { // IBTWS.cpp depends on this order
  enum enumInstrumentTypes : char { Unknown=0, Stock, Option, Future, FuturesOption, Currency, Index, ETF, Bond, Commodity, Metal, _Count };
  const char *Name[];
}

namespace OrderSide {
  enum enumOrderSide { Unknown=0, Buy, Sell, SellShort, BuyMinus, SellPlus, BuyStop, SellStop, _Count };
  const char *Name[];
}

namespace OrderStatus {
  enum enumOrderStatus { Created=0, SendingToProvider, PreSubmission, Rejected, Submitted, 
    Filling, CancelSubmitted, FillingDuringCancel, Filled, Cancelled, CancelledWithPartialFill, OverFilled };
}

namespace OptionSide {
  enum enumOptionSide { Unknown=0, Put, Call, _Count };
  const char *Name[];
  const char *LongName[];
  const char *ShortName[];
}

namespace OrderType { // ib szOrderType depends upon order
  enum enumOrderType { Unknown=0, Market, Limit, Stop, StopLimit, MarketIfTouched, Trail, TrailLimit, MarketClose, LimitClose, Scale, _Count  };
}

namespace OrderErrors {
  enum enumOrderErrors { Unknown=0, Rejected=0, Cancelled, NotCancellable, InstrumentNotFound };
}

namespace TimeInForce {
  enum enumTimeInForce { Unknown=0, Day, GoodTillCancel, AtTheOpening, ImmediateOrCancel, FillOrKill, GoodTillCrossing, GoodTillDate, AtTheClose, _Count };
}

namespace Currency {
  // http://en.wikipedia.org/wiki/ISO_4217
  // http://en.wikipedia.org/wiki/ISO_3166-1_alpha-2
  enum enumCurrency { USD, GBP, CAD, CHF, HKD, JPY, EUR, KRW, LTL, AUD, CYP, CZK, DKK, EEK, NZD, HUF, ILS, _Count };
  const char *Name[];
}

// currency pair    base/quote(aka counter)  - depicts how many units of the counter currency are needed to buy one unit of the base currency.
// http://en.wikipedia.org/wiki/Currency_pair
// bigger number, base is stronger
// smaller number, base is weaker

