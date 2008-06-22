#pragma once

#include <string>

#include "providerinterface.h"

class CSimulationProvider: public CProviderInterface {
public:
  CSimulationProvider(void);
  virtual ~CSimulationProvider(void);
  virtual void Connect( void );
  virtual void Disconnect( void );
  void SetGroupDirectory( const std::string sGroupDirectory ) { m_sGroupDirectory = sGroupDirectory; };  // eg /basket/20080620
protected:
  virtual CSymbol *NewCSymbol( const std::string &sSymbolName );
  virtual void StartQuoteWatch( CSymbol *pSymbol );
  virtual void StopQuoteWatch( CSymbol *pSymbol );
  virtual void StartTradeWatch( CSymbol *pSymbol );
  virtual void StopTradeWatch( CSymbol *pSymbol );
  virtual void StartDepthWatch( CSymbol *pSymbol );
  virtual void StopDepthWatch( CSymbol *pSymbol );

  std::string m_sGroupDirectory;
private:
};
