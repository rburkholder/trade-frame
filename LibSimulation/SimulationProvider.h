#pragma once

#include <string>

#include "ProviderInterface.h"
#include "SimulationSymbol.h"
#include "MergeDatedDatums.h"

// simulation provider needs to send an open event on each symbol it does
//  will need to be based upon time
// looks like CMergeDatedDatums will need an OnOpen event simulated

class CSimulationProvider: public CProviderInterface {
public:
  CSimulationProvider(void);
  virtual ~CSimulationProvider(void);
  virtual void Connect( void );
  virtual void Disconnect( void );
  void SetGroupDirectory( const std::string sGroupDirectory );  // eg /basket/20080620
  const std::string &GetGroupDirectory( void ) { return m_sGroupDirectory; };
  void Run( void );
  void Stop( void );
protected:
  virtual CSymbol *NewCSymbol( const std::string &sSymbolName );
  virtual void StartQuoteWatch( CSymbol *pSymbol );
  virtual void StopQuoteWatch( CSymbol *pSymbol );
  virtual void StartTradeWatch( CSymbol *pSymbol );
  virtual void StopTradeWatch( CSymbol *pSymbol );
  virtual void StartDepthWatch( CSymbol *pSymbol );
  virtual void StopDepthWatch( CSymbol *pSymbol );

  std::string m_sGroupDirectory;

  CWinThread *m_pMergeThread;
  CMergeDatedDatums *m_pMerge;

  static UINT __cdecl Merge( LPVOID lpParam );

private:
};
