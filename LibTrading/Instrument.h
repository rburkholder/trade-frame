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

// should a provider be included?  No, because this allows the same instrument
//  to be used to provide details to any provider.

#pragma once

#include <string>

#include "boost/shared_ptr.hpp"

#include "TradingEnumerations.h"

class CInstrument {
public:

  typedef boost::shared_ptr<CInstrument> pInstrument_t;
  typedef const pInstrument_t& pInstrument_ref;

  CInstrument( const std::string& sSymbolName, const std::string& sExchangeName, // generic
    InstrumentType::enumInstrumentTypes type = InstrumentType::Unknown );
  CInstrument( const std::string& sSymbolName, const std::string& sExchangeName,  // future
    InstrumentType::enumInstrumentTypes type, 
    unsigned short year, unsigned short month );
  CInstrument( const std::string& sSymbolName, const std::string& sExchangeName,  // option with yymm
    InstrumentType::enumInstrumentTypes type, 
    unsigned short year, unsigned short month,
    const std::string &sUnderlying,
    OptionSide::enumOptionSide side, 
    double strike ); 
  CInstrument( const std::string& sSymbolName, const std::string& sExchangeName,  // option with yymmdd
    InstrumentType::enumInstrumentTypes type, 
    unsigned short year, unsigned short month, unsigned short day,
    const std::string &sUnderlying,
    OptionSide::enumOptionSide side, 
    double strike ); 
  CInstrument( const std::string& sSymbolName, const std::string& sUnderlyingName, // currency
    InstrumentType::enumInstrumentTypes type, 
    Currency::enumCurrency base, Currency::enumCurrency counter );
    
  CInstrument( const CInstrument& );  // copy ctor
  virtual ~CInstrument(void);

  void SetCurrency( Currency::enumCurrency eCurrency ) { m_Currency = eCurrency; };
  const std::string &GetSymbolName( void ) { return m_sSymbolName; };
  const std::string &GetUnderlyingName( void ) { return m_sUnderlying; }
  const char *GetCurrencyName( void ) { return Currency::Name[ m_Currency ]; };
  const std::string& GetExchangeName( void ) { return m_sExchange; };
  InstrumentType::enumInstrumentTypes GetInstrumentType( void ) { return m_InstrumentType; };
  double GetStrike( void ) { return m_dblStrike; };
  unsigned short GetExpiryYear( void ) { return m_nYear; };
  unsigned short GetExpiryMonth( void ) { return m_nMonth; };
  unsigned short GetExpiryDay( void ) { return m_nDay; };
  OptionSide::enumOptionSide GetOptionSide( void ) { return m_OptionSide; };
  void SetContract( long id ) { m_nContract = id; };
  long GetContract( void ) { return m_nContract; };

protected:

  std::string m_sSymbolName; // main name
  std::string m_sUnderlying;  // underlying when main name is an option
  InstrumentType::enumInstrumentTypes m_InstrumentType;
  Currency::enumCurrency m_Currency;  // base currency - http://en.wikipedia.org/wiki/Currency_pair
  Currency::enumCurrency m_CurrencyCounter; // quote/counter currency -  - depicts how many units of the counter currency are needed to buy one unit of the base currency
  //Exchange::enumExchange m_Exchange;
  std::string m_sExchange;
  OptionSide::enumOptionSide m_OptionSide;
  unsigned short m_nYear; // future, option
  unsigned short m_nMonth; // future, option
  unsigned short m_nDay; // future, option
  double m_dblStrike;
  long m_nContract;

private:
};
