#pragma once

namespace InstrumentType {
  enum enumInstrumentTypes { Unknown, Stock, Option, Future, FuturesOption, Currency, Commodity, Metal, Index, ETF, Bond, _Count };
  const char *Name[];
}

namespace OrderSide {
  enum enumOrderSide {Buy, Sell, BuyMinus, SellPlus, SellShort, BuyStop, SellStop, _Count };
}

namespace OptionSide {
  enum enumOptionSide { Unknown, Put, Call, _Count };
  const char *Name[];
}

namespace ExecutionStatus {
  enum enumExecutionStatus { Filled, PartialFill, Cancelled, PartialCancellation, _Count };
}

namespace OrderType {
  enum enumOrderType { Market, Limit, Stop, StopLimit, MarketIfTouched, Trail, _Count  };
}

namespace TimeInForce {
  enum enumTimeInForce { Day, GoodTillCancel, AtTheOpening, ImmediateOrCancel, FillOrKill, GoodTillCrossing, GoodTillDate, AtTheClose, _Count };
}

namespace Currency {
  enum enumCurrency { USD, GBP, _Count };
  const char *Name[];
}

namespace Exchange {
  enum enumExchange { SMART, NYSE, NMS, NASDAQ, AMEX, OPRA, _Count };
  const char *Name[];
}

