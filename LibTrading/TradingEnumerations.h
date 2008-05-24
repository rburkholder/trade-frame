#pragma once

namespace Trading {
  enum enumOrderSide { OrderBuy, OrderSell, OrderBuyStop, OrderSellStop };
  enum enumOptionSide { OptionPut, OptionCall };
  enum enumExecutionStatus { OrderFilled, OrderPartialFill, OrderCancelled, OrderPartialCancellation };
  enum enumContractTypes { UnknownContract, Stock, Option, Future, FuturesOption, Currency, Commodity, Metals, Index, ETF, Bond, ContractTypeCount };
  
}