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

#include "TFTrading/ProviderInterface.h"

#include "IQFeed.h"
#include "IQFeedSymbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CIQFeedProvider :
  public CProviderInterface<CIQFeedProvider,CIQFeedSymbol>, 
  public CIQFeed<CIQFeedProvider> 
{
  friend CIQFeed<CIQFeedProvider>;
public:

  typedef boost::shared_ptr<CIQFeedProvider> pProvider_t;
  typedef CProviderInterface<CIQFeedProvider,CIQFeedSymbol> inherited_t;
  typedef inherited_t::symbol_id_t symbol_id_t;
  typedef inherited_t::pSymbol_t pSymbol_t;
  typedef inherited_t::pInstrument_t pInstrument_t;
  typedef CIQFeed<CIQFeedProvider>  IQFeed_t;

  CIQFeedProvider( void );
  ~CIQFeedProvider( void );

  // do these need to be virtual?  use crtp?
  virtual void Connect( void );
  virtual void Disconnect( void );

protected:

  void StartQuoteTradeWatch( CIQFeedSymbol *pSymbol );
  void StopQuoteTradeWatch( CIQFeedSymbol *pSymbol );

  // overridden from ProviderInterface, called when application adds/removes watches
  virtual void StartQuoteWatch( pSymbol_t pSymbol ) ;
  virtual void  StopQuoteWatch( pSymbol_t pSymbol );

  virtual void StartTradeWatch( pSymbol_t pSymbol );
  virtual void  StopTradeWatch( pSymbol_t pSymbol );

  virtual void StartDepthWatch( pSymbol_t pSymbol ) {};
  virtual void  StopDepthWatch( pSymbol_t pSymbol ) {};

  pSymbol_t NewCSymbol( pInstrument_t pInstrument );  // used by Add/Remove x handlers in base class

  void OnIQFeedUpdateMessage( linebuffer_t* pBuffer, CIQFUpdateMessage *pMsg );
  void OnIQFeedSummaryMessage( linebuffer_t* pBuffer, CIQFSummaryMessage *pMsg );
  void OnIQFeedFundamentalMessage( linebuffer_t* pBuffer, CIQFFundamentalMessage *pMsg );
  void OnIQFeedNewsMessage( linebuffer_t* pBuffer, CIQFNewsMessage *pMsg );
  void OnIQFeedTimeMessage( linebuffer_t* pBuffer, CIQFTimeMessage *pMsg );
  void OnIQFeedSystemMessage( linebuffer_t* pBuffer, CIQFSystemMessage *pMsg );

  void OnIQFeedDisConnected( void );  // CRTP on IQFeed
  void OnIQFeedConnected( void ); // CRTP on IQFeed

private:

};

} // namespace tf
} // namespace ou
