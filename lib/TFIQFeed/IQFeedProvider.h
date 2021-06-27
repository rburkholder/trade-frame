/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

class IQFeedProvider :
  public ProviderInterface<IQFeedProvider,IQFeedSymbol>,
  public IQFeed<IQFeedProvider>
{
  friend IQFeed<IQFeedProvider>;
public:

  using pProvider_t = boost::shared_ptr<IQFeedProvider>;
  using inherited_t = ProviderInterface<IQFeedProvider,IQFeedSymbol>;
  using symbol_id_t = inherited_t::symbol_id_t ;
  using pSymbol_t = inherited_t::pSymbol_t;
  using pInstrument_t = inherited_t::pInstrument_t;
  using IQFeed_t = IQFeed<IQFeedProvider>;

  IQFeedProvider();
  virtual ~IQFeedProvider();

  // do these need to be virtual?  use crtp?
  virtual void Connect();
  virtual void Disconnect();

  void SetAlternateInstrumentName( pInstrument_t );

protected:

  // overridden from ProviderInterface, called when application adds/removes watches
  virtual void StartQuoteWatch( pSymbol_t pSymbol );
  virtual void  StopQuoteWatch( pSymbol_t pSymbol );

  virtual void StartTradeWatch( pSymbol_t pSymbol );
  virtual void  StopTradeWatch( pSymbol_t pSymbol );

  virtual void StartDepthWatch( pSymbol_t pSymbol ) {};
  virtual void  StopDepthWatch( pSymbol_t pSymbol ) {};

  pSymbol_t NewCSymbol( pInstrument_t pInstrument );  // used by Add/Remove x handlers in base class

  void OnIQFeedDynamicFeedUpdateMessage( linebuffer_t* pBuffer, IQFDynamicFeedUpdateMessage *pMsg );
  void OnIQFeedDynamicFeedSummaryMessage( linebuffer_t* pBuffer, IQFDynamicFeedSummaryMessage *pMsg );
  void OnIQFeedUpdateMessage( linebuffer_t* pBuffer, IQFUpdateMessage *pMsg );
  void OnIQFeedSummaryMessage( linebuffer_t* pBuffer, IQFSummaryMessage *pMsg );
  void OnIQFeedFundamentalMessage( linebuffer_t* pBuffer, IQFFundamentalMessage *pMsg );
  void OnIQFeedNewsMessage( linebuffer_t* pBuffer, IQFNewsMessage *pMsg );
  void OnIQFeedTimeMessage( linebuffer_t* pBuffer, IQFTimeMessage *pMsg );
  void OnIQFeedSystemMessage( linebuffer_t* pBuffer, IQFSystemMessage *pMsg );

  void OnIQFeedDisConnected();  // CRTP on IQFeed
  void OnIQFeedConnected(); // CRTP on IQFeed
  void OnIQFeedError( size_t );

private:

  void UpdateQuoteTradeWatch( char command, IQFeedSymbol::WatchState next, IQFeedSymbol *pSymbol );

};

} // namespace tf
} // namespace ou
