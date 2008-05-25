#pragma once

namespace ContractType {
  enum enumContractTypes { Unknown, Stock, Option, Future, FuturesOption, Currency, Commodity, Metal, Index, ETF, Bond, _Count };
}

namespace OrderSide {
  enum enumOrderSide {Buy, Sell, BuyStop, SellStop };
}

namespace OptionSide {
  enum enumOptionSide { Put, Call };
}

namespace ExecutionStatus {
  enum enumExecutionStatus { Filled, PartialFill, Cancelled, PartialCancellation };
}