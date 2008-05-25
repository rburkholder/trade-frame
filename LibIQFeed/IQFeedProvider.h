#pragma once
#include "k:\data\projects\tradingsoftware\libtrading\providerinterface.h"
#include "IQFeed.h"

class CIQFeedProvider :
  public CProviderInterface, public CIQFeed {
public:
  CIQFeedProvider(void);
  virtual ~CIQFeedProvider(void);

  virtual void Connect( void );
  virtual void Disconnect( void );
protected:

  // overridden from ProviderInterface, called when application adds/removes watches
  virtual void StartQuoteWatch( CSymbol *pSymbol );
  virtual void StopQuoteWatch( CSymbol *pSymbol );
  virtual void StartTradeWatch( CSymbol *pSymbol );
  virtual void StopTradeWatch( CSymbol *pSymbol );
  virtual void StartQuoteTradeWatch( CSymbol *pSymbol );
  virtual void StopQuoteTradeWatch( CSymbol *pSymbol );
  virtual void StartDepthWatch( CSymbol *pSymbol );
  virtual void StopDepthWatch( CSymbol *pSymbol );

  virtual CSymbol *NewCSymbol( const std::string &sSymbolName );  // used by Add/Remove x handlers in base class

private:
};
