#pragma once

#include <string>

#include "Instrument.h"
#include "InstrumentFile.h"
//#include "Singleton.h"
#include "ManagerBase.h"

class CInstrumentManager: public ManagerBase<CInstrumentManager, std::string, CInstrument> {
public:
  CInstrumentManager(void);
  virtual ~CInstrumentManager(void);
  CInstrument* GetIQFeedInstrument( const std::string& sName );
  CInstrument* GetIQFeedInstrument( const std::string& sName, const std::string& sAlternateName );
protected:
  CInstrumentFile file;
private:
};

