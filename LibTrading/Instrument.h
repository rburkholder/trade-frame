#pragma once

#include "TradingEnumerations.h"

#include <string>

class CInstrument {
public:
  CInstrument( const std::string &sSymbolName,
    InstrumentType::enumInstrumentTypes type = InstrumentType::Unknown );
  CInstrument( const std::string &sSymbol, 
    InstrumentType::enumInstrumentTypes type, 
    unsigned short year, unsigned short month );
  CInstrument( const std::string &sSymbol, const std::string &sUnderlying,
    InstrumentType::enumInstrumentTypes type, OptionSide::enumOptionSide side, 
    double strike, unsigned short year, unsigned short month );
  CInstrument( const CInstrument& );  // copy ctor
  virtual ~CInstrument(void);
protected:
  std::string m_sSymbolName; // main name
  std::string m_sUnderlying;  // underlying when main name is an option
  InstrumentType::enumInstrumentTypes m_InstrumentType;
  Currency::enumCurrency m_Currency;
  Exchange::enumExchange m_Exchange;
  OptionSide::enumOptionSide m_OptionSide;
  unsigned short m_nYear; // future, option
  unsigned short m_nMonth; // future, option
  double m_dblStrike;
private:
};
