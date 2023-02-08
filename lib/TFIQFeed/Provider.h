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

#include <TFTrading/ProviderInterface.h>

#include "IQFeed.h"
#include "Symbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

class Provider :
  public ProviderInterface<Provider,IQFeedSymbol>
, public IQFeed<Provider>
{
  friend IQFeed<Provider>;
public:

  using pProvider_t = std::shared_ptr<Provider>;
  using inherited_t = ProviderInterface<Provider,IQFeedSymbol>;
  using idSymbol_t = inherited_t::idSymbol_t ;
  using pSymbol_t = inherited_t::pSymbol_t;
  using pInstrument_t = inherited_t::pInstrument_t;
  using IQFeed_t = IQFeed<Provider>;

  Provider();
  virtual ~Provider();

  static pProvider_t Factory() {
    return std::make_shared<Provider>();
  }

  static pProvider_t Cast( inherited_t::pProvider_t pProvider ) {
    return std::dynamic_pointer_cast<Provider>( pProvider );
  }

  void EnableExecution( bool bEnable ) { m_bExecutionEnabled = bEnable; }
  bool ExecutionEnabled() const { return m_bExecutionEnabled; }

  // do these need to be virtual?  use crtp?
  virtual void Connect();
  virtual void Disconnect();

  std::string ListedMarket( key_t nListedMarket ) const { return LookupListedMarket( nListedMarket ); }

  void SetCommission( const std::string& sSymbol, double commission );

protected:

  void AddQuoteHandler( pInstrument_cref pInstrument, Provider::quotehandler_t handler );
  void RemoveQuoteHandler( pInstrument_cref pInstrument, Provider::quotehandler_t handler );
  void AddTradeHandler( pInstrument_cref pInstrument, Provider::tradehandler_t handler );
  void RemoveTradeHandler( pInstrument_cref pInstrument, Provider::tradehandler_t handler );

  //void AddDepthByMMHandler( pInstrument_cref pInstrument, Provider::depthbymmhandler_t handler );
  //void RemoveDepthByMMHandler( pInstrument_cref pInstrument, Provider::depthbymmhandler_t handler );
  //void AddDepthByOrderHandler( pInstrument_cref pInstrument, Provider::depthbyorderhandler_t handler );
  //void RemoveDepthByOrderHandler( pInstrument_cref pInstrument, Provider::depthbyorderhandler_t handler );

  // overridden from ProviderInterface, called when application adds/removes watches
  virtual void StartQuoteWatch( pSymbol_t pSymbol );
  virtual void  StopQuoteWatch( pSymbol_t pSymbol );

  virtual void StartTradeWatch( pSymbol_t pSymbol );
  virtual void  StopTradeWatch( pSymbol_t pSymbol );

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

  bool m_bExecutionEnabled;

  void UpdateQuoteTradeWatch( char command, IQFeedSymbol::WatchState next, IQFeedSymbol *pSymbol );

  void HandleExecution( Order::idOrder_t orderId, const Execution &exec );
  void HandleCommission( Order::idOrder_t orderId, double commission );
  void HandleCancellation( Order::idOrder_t orderId );
};

} // namespace iqfeed
} // namespace tf
} // namespace ou
