#pragma once

#include <string>

#include "TradingEnumerations.h"
#include "InstrumentFile.h"

class CIQFeedSymbolFile : public CInstrumentFile {
public:
  CIQFeedSymbolFile(void);
  virtual ~CIQFeedSymbolFile(void);
  bool Load( const string &filename );
protected:
  static struct structExchangeInfo {
    char *szName;
    InstrumentType::enumInstrumentTypes eInstrumentType;
    unsigned long cntInstruments;
    //structExchangeInfo( void ) : szName( NULL ), nContractType( Trading::UnknownContract ), cntContracts( 0 ) {};
  } m_rExchanges[];

private:
};
