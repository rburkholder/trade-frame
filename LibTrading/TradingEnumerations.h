#pragma once

namespace InstrumentType {
  enum enumInstrumentTypes { Unknown, Stock, Option, Future, FuturesOption, Currency, Commodity, Metal, Index, ETF, Bond, _Count };
}

namespace OrderSide {
  enum enumOrderSide {Buy, Sell, BuyMinus, SellPlus, SellShort, BuyStop, SellStop };
}

namespace OptionSide {
  enum enumOptionSide { Put, Call };
}

namespace ExecutionStatus {
  enum enumExecutionStatus { Filled, PartialFill, Cancelled, PartialCancellation };
}

namespace OrderType {
  enum enumOrderType { Market, Limit, Stop, StopLimit, MarketIfTouched, Trail,  };
}

namespace TimeInForce {
  enum enumTimeInForce { Day, GoodTillCancel, AtTheOpening, ImmediateOrCancel, FillOrKill, GoodTillCrossing, GoodTillDate, AtTheClose };
}

