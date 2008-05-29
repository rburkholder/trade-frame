#pragma once

#include "TradingEnumerations.h"

#include <string>

class CInstrument {
public:
  CInstrument( InstrumentType::enumInstrumentTypes type = InstrumentType::Unknown );
  virtual ~CInstrument(void);
protected:
  std::string m_sUnderlying;
  InstrumentType::enumInstrumentTypes m_InstrumentType;
  Currency::enumCurrency m_Currency;
  Exchange::enumExchange m_Exchange;
  unsigned short m_nYear; // future, option
  unsigned short m_nMonth; // future, option
private:
};
