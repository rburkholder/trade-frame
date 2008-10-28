#pragma once

#include <string>

#include "ProviderInterface.h"
#include "SimulationSymbol.h"
#include "CrossThreadMerge.h"
#include "TimeSource.h"

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
  virtual void PlaceOrder( COrder *pOrder );
  virtual void CancelOrder( COrder *pOrder );
  virtual void AddTradeHandler( const string &sSymbol, CSymbol::tradehandler_t handler );
  virtual void RemoveTradeHandler( const string &sSymbol, CSymbol::tradehandler_t handler );
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
  CCrossThreadMerge *m_pMerge;

  static UINT __cdecl Merge( LPVOID lpParam );

  void HandleExecution( const CExecution &exec );
  //void PreSymbolDestroy( CSymbol *pSymbol );

  ptime m_dtSimStart;
  ptime m_dtSimStop;

private:
};
