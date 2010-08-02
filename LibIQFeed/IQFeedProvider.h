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

#include <boost/shared_ptr.hpp>

#include "LibTrading/ProviderInterface.h"

#include "IQFeed.h"
#include "IQFeedSymbol.h"

class CIQFeedProvider :
  public CProviderInterface<CIQFeedProvider,CIQFeedSymbol>, 
  public CIQFeed<CIQFeedProvider> 
{
public:

  typedef boost::shared_ptr<CIQFeedProvider> pProvider_t;
  typedef CProviderInterface<CIQFeedProvider,CIQFeedSymbol> ProviderInterface_t;
  typedef ProviderInterface_t::symbol_id_t symbol_id_t;
  typedef CIQFeedSymbol::pInstrument_t pInstrument_t;
  typedef CIQFeed<CIQFeedProvider>  IQFeed_t;

  CIQFeedProvider( void );
  ~CIQFeedProvider( void );

  // do these need to be virtual?  use crtp?
  virtual void Connect( void );
  virtual void Disconnect( void );

  // use crtp?
  virtual CIQFeedSymbol* GetSymbol( symbol_id_t id ) { 
    CIQFeedSymbol* pSym = ProviderInterface_t::GetSymbol( id );
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

  virtual CIQFeedSymbol* NewCSymbol( pInstrument_t pInstrument );  // used by Add/Remove x handlers in base class

  virtual void HandleQMessage( CIQFUpdateMessage *pMsg );
  virtual void HandlePMessage( CIQFSummaryMessage *pMsg );
  virtual void HandleFMessage( CIQFFundamentalMessage *pMsg );
  virtual void HandleNMessage( CIQFNewsMessage *pMsg );
  virtual void HandleTMessage( CIQFTimeMessage *pMsg );
  virtual void HandleSMessage( CIQFSystemMessage *pMsg );

private:

};
