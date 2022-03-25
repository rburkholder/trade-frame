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

#include "TradingEnumerations.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

namespace InstrumentType {
  const char* Name[] = { "Unknown", "Stock", "Option", "Future",
    "FuturesOption", "Currency", "Index", "ETF", "Bond", "Commodity", "Metal",  };
}

namespace OptionSide {  // this doesn't work well with the existing enumerations
  const char* Name[] = { "Unknown", "Put", "Call" };
  const char* LongName[] = { "Unknown", "Put", "Call" };
  const char* ShortName[] = { "U", "P", "C" };
}

namespace Currency {
  const char* Name[] = { "USD", "GBP", "CAD", "CHF", "HKD", "JPY", "EUR", "KRW", "LTL", "AUD", "CZK", "DKK", "NZD", "HUF", "ILS" };
}

namespace OrderSide {
  const char* Name[] = { "UNKN", "BUY", "SELL", "SSHORT" }; // for IB
}

namespace TimeInForce {
  const char* Name[] = { "UNKN", "DAY", "GTC", "ATO", "IOC", "FOK", "GTC", "GTD", "GAC", "ATC", "AUC", "OPG" };
}

} // namespace tf
} // namespace ou
