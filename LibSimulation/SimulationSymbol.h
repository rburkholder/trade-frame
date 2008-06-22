#pragma once

#include <string>

#include "symbol.h"

class CSimulationSymbol: public CSymbol {
  friend class CSimulationProvider;
public:
  CSimulationSymbol(const std::string &symbol);
  virtual ~CSimulationSymbol(void);
protected:
  void StartTradeWatch( void );
  void StopTradeWatch( void );
  void StartQuoteWatch( void );
  void StopQuoteWatch( void );
  void StartDepthWatch( void );
  void StopDepthWatch( void );
private:
};
