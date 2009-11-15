#include "StdAfx.h"
#include "TradingEnumerations.h"

namespace InstrumentType {
  const char *Name[] = { "Unknown", "Stock", "Option", "Future", 
    "FuturesOption", "Currency", "Index", "ETF", "Bond", "Commodity", "Metal",  };
}

namespace OptionSide {
  const char *Name[] = { "Unknown", "Put", "Call" };
  const char *LongName[] = { "Unknown", "Put", "Call" };
  const char *ShortName[] = { "U", "P", "C" };
}

namespace Currency {
  const char *Name[] = { "USD", "GBP", "CAD", "CHF", "HKD", "JPY", "EUR", "KRW", "LTL", "AUD", "CYP", "CZK", "DKK", "EEK", "NZD", "HUF", "ILS" };
}

namespace OrderSide {
  const char *Name[] = { "UNKN", "BUY", "SELL", "SSHORT" }; // for IB
}

