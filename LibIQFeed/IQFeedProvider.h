/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once

#include "LibTrading/ProviderInterface.h"

#include "IQFeed.h"
#include "IQFeedSymbol.h"

class CIQFeedProvider :
  public CProviderInterface<CIQFeedProvider,CIQFeedSymbol>, 
  public CIQFeed<CIQFeedProvider> 
{
public:

  typedef CProviderInterface<CIQFeedProvider,CIQFeedSymbol> ProviderInterface_t;

  CIQFeedProvider( void );
  ~CIQFeedProvider( void );

  // do these need to be virtual?  use crtp?
  virtual void Connect( void );
  virtual void Disconnect( void );

  // use crtp?
  virtual CIQFeedSymbol* GetSymbol( const string &sSymbol ) { 
    CIQFeedSymbol* pSym = ProviderInterface_t::GetSymbol( sSymbol );
    return pSym; 
  };

protected:

  void StartQuoteTradeWatch( CIQFeedSymbol *pSymbol );
  void StopQuoteTradeWatch( CIQFeedSymbol *pSymbol );

  // overridden from ProviderInterface, called when application adds/removes watches
  virtual void StartQuoteWatch( CIQFeedSymbol* pSymbol ) ;
  virtual void  StopQuoteWatch( CIQFeedSymbol* pSymbol );

  virtual void StartTradeWatch( CIQFeedSymbol* pSymbol );
  virtual void  StopTradeWatch( CIQFeedSymbol* pSymbol );

  virtual void StartDepthWatch( CIQFeedSymbol* pSymbol ) {};
  virtual void  StopDepthWatch( CIQFeedSymbol* pSymbol ) {};

  virtual CIQFeedSymbol* NewCSymbol( const std::string &sSymbolName );  // used by Add/Remove x handlers in base class

  virtual void HandleQMessage( CIQFUpdateMessage *pMsg );
  virtual void HandlePMessage( CIQFSummaryMessage *pMsg );
  virtual void HandleFMessage( CIQFFundamentalMessage *pMsg );
  virtual void HandleNMessage( CIQFNewsMessage *pMsg );
  virtual void HandleTMessage( CIQFTimeMessage *pMsg );
  virtual void HandleSMessage( CIQFSystemMessage *pMsg );

private:

};
