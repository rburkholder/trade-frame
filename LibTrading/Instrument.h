#pragma once

#include "TradingEnumerations.h"

class CInstrument {
public:
  CInstrument( InstrumentType::enumInstrumentTypes type = InstrumentType::Unknown );
  virtual ~CInstrument(void);
protected:
  InstrumentType::enumInstrumentTypes m_InstrumentType;
private:
};
