#pragma once

namespace Trading {
  enum enumOrderSide { OrderBuy, OrderSell, OrderBuyStop, OrderSellStop };
  enum enumOptionSide { OptionCall, OptionPut };
  enum enumExecutionStatus { OrderFilled, OrderPartialFill, OrderCancelled, OrderPartialCancellation };
}