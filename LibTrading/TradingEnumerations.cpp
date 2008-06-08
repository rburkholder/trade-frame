#include "StdAfx.h"

#include "TradingEnumerations.h"

namespace InstrumentType {
  const char *Name[] = { "Unknown", "Stock", "Option", "Future", 
    "FuturesOption", "Currency", "Index", "ETF", "Bond", "Commodity", "Metal",  };
}

namespace OptionSide {
  const char *Name[] = { "Unknown", "Put", "Call" };
}

namespace Currency {
  const char *Name[] = { "USD", "GBP", "CAD", "CHF", "HKD", "JPY", "EUR", "KRW", "LTL", "AUD", "CYP", "CZK", "DKK", "EEK", "NZD", "HUF", "ILS" };
}

namespace OrderSide {
  const char *Name[] = { "UNKN", "BUY", "SELL", "SSHORT" }; // for IB
}