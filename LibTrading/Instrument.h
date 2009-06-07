#pragma once

#include "TradingEnumerations.h"

#include <string>
#include "boost/shared_ptr.hpp"

class CInstrument {
public:
  CInstrument( const std::string &sSymbolName, const std::string &sExchangeName,
    InstrumentType::enumInstrumentTypes type = InstrumentType::Unknown );
  CInstrument( const std::string &sSymbolName, const std::string &sExchangeName, 
    InstrumentType::enumInstrumentTypes type, 
    unsigned short year, unsigned short month );
  CInstrument( const std::string &sSymbolName, const std::string &sExchangeName,
    InstrumentType::enumInstrumentTypes type, 
    unsigned short year, unsigned short month,
    const std::string &sUnderlying,
    OptionSide::enumOptionSide side, 
    double strike );
  CInstrument( const CInstrument& );  // copy ctor
  virtual ~CInstrument(void);

  typedef boost::shared_ptr<CInstrument> pInstrument_t;

  void SetCurrency( Currency::enumCurrency eCurrency ) { m_Currency = eCurrency; };
  const std::string &GetSymbolName( void ) { return m_sSymbolName; };
  const std::string &GetUnderlyingName( void ) { return m_sUnderlying; }
  const char *GetCurrencyName( void ) { return Currency::Name[ m_Currency ]; };
  const std::string *GetExchangeName( void ) { return &m_sExchange; };
  InstrumentType::enumInstrumentTypes GetInstrumentType( void ) { return m_InstrumentType; };
  double GetStrike( void ) { return m_dblStrike; };
  unsigned short GetExpiryYear( void ) { return m_nYear; };
  unsigned short GetExpiryMonth( void ) { return m_nMonth; };
  OptionSide::enumOptionSide GetOptionSide( void ) { return m_OptionSide; };
  void SetContract( long id ) { m_nContract = id; };
  long GetContract( void ) { return m_nContract; };
protected:
  std::string m_sSymbolName; // main name
  std::string m_sUnderlying;  // underlying when main name is an option
  InstrumentType::enumInstrumentTypes m_InstrumentType;
  Currency::enumCurrency m_Currency;
  //Exchange::enumExchange m_Exchange;
  std::string m_sExchange;
  OptionSide::enumOptionSide m_OptionSide;
  unsigned short m_nYear; // future, option
  unsigned short m_nMonth; // future, option
  double m_dblStrike;
  long m_nContract;
private:
};
