#pragma once

namespace InstrumentType { // IBTWS.cpp depends on this order
  enum enumInstrumentTypes : char { Unknown, Stock, Option, Future, FuturesOption, Currency, Index, ETF, Bond, Commodity, Metal, _Count };
  const char *Name[];
}

namespace OrderSide {
  enum enumOrderSide {Buy, Sell, SellShort, BuyMinus, SellPlus, BuyStop, SellStop, _Count };
  const char *Name[];
}

namespace OptionSide {
  enum enumOptionSide { Unknown, Put, Call, _Count };
  const char *Name[];
}

namespace ExecutionStatus {
  enum enumExecutionStatus { Filled, PartialFill, Cancelled, PartialCancellation, _Count };
}

namespace OrderType { // ib szOrderType depends upon order
  enum enumOrderType { Market, Limit, Stop, StopLimit, MarketIfTouched, Trail, TrailLimit, MarketClose, LimitClose, Scale, _Count  };
}

namespace TimeInForce {
  enum enumTimeInForce { Day, GoodTillCancel, AtTheOpening, ImmediateOrCancel, FillOrKill, GoodTillCrossing, GoodTillDate, AtTheClose, _Count };
}

namespace Currency {
  // http://en.wikipedia.org/wiki/ISO_4217
  // http://en.wikipedia.org/wiki/ISO_3166-1_alpha-2
  enum enumCurrency { USD, GBP, CAD, CHF, HKD, JPY, EUR, KRW, LTL, AUD, CYP, CZK, DKK, EEK, NZD, HUF, ILS, _Count };
  const char *Name[];
}

namespace Exchange {
  enum enumExchange { SMART, NYSE, NMS, NASDAQ, AMEX, OPRA, _Count };
  const char *Name[];
}

