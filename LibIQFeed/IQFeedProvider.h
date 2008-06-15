#pragma once
#include <providerinterface.h>
#include "IQFeed.h"
#include "IQFeedSymbol.h"

class CIQFeedProvider :
  public CProviderInterface, public CIQFeed {
public:
  CIQFeedProvider(void);
  virtual ~CIQFeedProvider(void);

  virtual void Connect( void );
  virtual void Disconnect( void );

  virtual CIQFeedSymbol *GetSymbol( const string &sSymbol ) { 
    CSymbol *pSym = CProviderInterface::GetSymbol( sSymbol );
    return dynamic_cast<CIQFeedSymbol *>( pSym ); 
  };

  //virtual void StartQuoteTradeWatch( CSymbol *pSymbol );
  //virtual void StopQuoteTradeWatch( CSymbol *pSymbol );
protected:

  // overridden from ProviderInterface, called when application adds/removes watches
  virtual void StartQuoteWatch( CSymbol *pSymbol ) ;
  virtual void StopQuoteWatch( CSymbol *pSymbol );
  virtual void StartTradeWatch( CSymbol *pSymbol );
  virtual void StopTradeWatch( CSymbol *pSymbol );
  virtual void StartDepthWatch( CSymbol *pSymbol ) {};
  virtual void StopDepthWatch( CSymbol *pSymbol ) {};

  virtual CSymbol *NewCSymbol( const std::string &sSymbolName );  // used by Add/Remove x handlers in base class

  virtual void HandleQMessage( CIQFUpdateMessage *pMsg );
  virtual void HandlePMessage( CIQFSummaryMessage *pMsg );
  virtual void HandleFMessage( CIQFFundamentalMessage *pMsg );
  virtual void HandleNMessage( CIQFNewsMessage *pMsg );
  virtual void HandleTMessage( CIQFTimeMessage *pMsg );
  virtual void HandleSMessage( CIQFSystemMessage *pMsg );

private:
};
