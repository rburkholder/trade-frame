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
  public CProviderInterface, public CIQFeed<CIQFeedProvider> {
public:
  CIQFeedProvider( CAppModule* pModule, m_structMessageDestinations );
  ~CIQFeedProvider(void);

  // do these need to be virtual?  use crtp?
  virtual void Connect( void );
  virtual void Disconnect( void );

  // use crtp?
  virtual CIQFeedSymbol *GetSymbol( const string &sSymbol ) { 
    CSymbol *pSym = CProviderInterface::GetSymbol( sSymbol );
    return dynamic_cast<CIQFeedSymbol *>( pSym ); 
  };

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

  CIQFeed<CIQFeedProvider>::structMessageDestinations m_structMessageDestinations;
};
