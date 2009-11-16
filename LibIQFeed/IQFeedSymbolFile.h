#pragma once

#include <string>

#include <LibTrading/TradingEnumerations.h>

// http://www.dtniq.com/product/mktsymbols.zip

// Is designed for running in debug mode at the moment.

class CIQFeedSymbolFile {
public:
  CIQFeedSymbolFile(void);
  ~CIQFeedSymbolFile(void);

  bool Load( const std::string &filename );

protected:

  static struct structExchangeInfo {
    char *szName;
    InstrumentType::enumInstrumentTypes eInstrumentType;
    //unsigned long cntInstruments;
    //structExchangeInfo( void ) : szName( NULL ), nContractType( Trading::UnknownContract ), cntContracts( 0 ) {};
  } m_rExchanges[];


private:
};
