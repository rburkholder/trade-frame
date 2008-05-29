#include "StdAfx.h"

#include "TradingEnumerations.h"

namespace InstrumentType {
  const char *Name[] = { "Unknown", "Stock", "Option", "Future", 
    "FuturesOption", "Currency", "Commodity", "Metal", "Index", "ETF", "Bond" };
}

namespace Currency {
  const char *Name[] = { "USD", "GBP" };
}

namespace Exchange {
  const char *Name[] = { "SMART", "NYSE", "NMS", "NASDAQ", "AMEX" };
}