#pragma once

#include "Symbol.h"
#include "Delegate.h"

#include <map>
#include <string>

// need to include a check that callbacks and virtuals are in the correct thread
// in IB, processMsg may be best place to have in cross thread management, if it isn't already

class CProviderInterface {
public:
  CProviderInterface(void);
  virtual ~CProviderInterface(void);

  virtual void Connect( void );
  Delegate<int> OnConnected;
  virtual void Disconnect( void );
  Delegate<int> OnDisconnected;

  virtual void AddQuoteHandler( const string &sSymbol, CSymbol::quotehandler_t handler );
  virtual void RemoveQuoteHandler( const string &sSymbol, CSymbol::quotehandler_t handler );
  virtual void AddTradeHandler( const string &sSymbol, CSymbol::tradehandler_t handler );
  virtual void RemoveTradeHandler( const string &sSymbol, CSymbol::tradehandler_t handler );
  virtual void AddDepthHandler( const string &sSymbol, CSymbol::depthhandler_t handler );
  virtual void RemoveDepthHandler( const string &sSymbol, CSymbol::depthhandler_t handler );

  virtual CSymbol* GetSymbol( const string &sSymbol );

  const std::string &Name( void ) { return m_sName; };
  unsigned short ID( void ) { assert( 0 != m_nID ); return m_nID; };
  bool Connected( void ) { return m_bConnected; };
protected:
  std::string m_sName;  // name of provider
  unsigned short m_nID;
  bool m_bConnected;
  std::map<std::string, CSymbol*> m_mapSymbols;
  virtual void StartQuoteWatch( CSymbol *pSymbol ) {};
  virtual void StopQuoteWatch( CSymbol *pSymbol ) {};
  virtual void StartTradeWatch( CSymbol *pSymbol ) {};
  virtual void StopTradeWatch( CSymbol *pSymbol ) {};
  virtual void StartDepthWatch( CSymbol *pSymbol ) {};
  virtual void StopDepthWatch( CSymbol *pSymbol ) {};
  virtual CSymbol *NewCSymbol( const std::string &sSymbolName ) = 0; // override for deriving different inherited version
private:
};
