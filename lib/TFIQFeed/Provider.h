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

#include <TFSimulation/SimulationInterface.hpp>

#include "IQFeed.h"
#include "Symbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

class Provider :
  public ou::tf::sim::SimulationInterface<Provider,IQFeedSymbol>
, public IQFeed<Provider>
{
  friend ou::tf::sim::SimulationInterface<Provider,IQFeedSymbol>;
  friend IQFeed<Provider>;
public:

  using inherited_t = ou::tf::sim::SimulationInterface<Provider,IQFeedSymbol>;

  using idSymbol_t = inherited_t::idSymbol_t ;
  using pSymbol_t = inherited_t::pSymbol_t;
  using pInstrument_t = inherited_t::pInstrument_t;

  using pProvider_t = std::shared_ptr<Provider>;
  using IQFeed_t = IQFeed<Provider>;

  Provider();
  virtual ~Provider();

  static pProvider_t Factory() {
    return std::make_shared<Provider>();
  }

  static pProvider_t Cast( inherited_t::pProvider_t pProvider ) {
    return std::dynamic_pointer_cast<Provider>( pProvider );
  }

  void EnableExecution( bool bEnable );
  bool ExecutionEnabled() const { return m_bExecutionEnabled; }

  // do these need to be virtual?  use crtp?
  virtual void Connect();
  virtual void Disconnect();

  std::string ListedMarket( key_t nListedMarket ) const { return LookupListedMarket( nListedMarket ); }

protected:

  // overridden from ProviderInterface, called when application adds/removes watches
  virtual void StartQuoteWatch( pSymbol_t pSymbol );
  virtual void  StopQuoteWatch( pSymbol_t pSymbol );

  virtual void StartTradeWatch( pSymbol_t pSymbol );
  virtual void  StopTradeWatch( pSymbol_t pSymbol );

  pSymbol_t virtual NewCSymbol( pInstrument_t pInstrument );  // used by Add/Remove x handlers in base class

  void OnIQFeedDynamicFeedUpdateMessage( linebuffer_t* pBuffer, IQFDynamicFeedUpdateMessage *pMsg );
  void OnIQFeedDynamicFeedSummaryMessage( linebuffer_t* pBuffer, IQFDynamicFeedSummaryMessage *pMsg );
  void OnIQFeedUpdateMessage( linebuffer_t* pBuffer, IQFUpdateMessage *pMsg );
  void OnIQFeedSummaryMessage( linebuffer_t* pBuffer, IQFSummaryMessage *pMsg );
  void OnIQFeedFundamentalMessage( linebuffer_t* pBuffer, IQFFundamentalMessage *pMsg );
  void OnIQFeedNewsMessage( linebuffer_t* pBuffer, IQFNewsMessage *pMsg );
  void OnIQFeedTimeMessage( linebuffer_t* pBuffer, IQFTimeMessage *pMsg );
  void OnIQFeedSystemMessage( linebuffer_t* pBuffer, IQFSystemMessage *pMsg );
  void OnIQFeedSymbolNotFoundMessage( linebuffer_t* pBuffer, IQFErrorMessage* msg);

  void OnIQFeedDisConnected();  // CRTP on IQFeed
  void OnIQFeedConnected(); // CRTP on IQFeed
  void OnIQFeedError( size_t );

private:

  void UpdateQuoteTradeWatch( char command, IQFeedSymbol::WatchState next, IQFeedSymbol *pSymbol );

  void HandleExecution( Order::idOrder_t orderId, const Execution &exec );
  void HandleCommission( Order::idOrder_t orderId, double commission );
  void HandleCancellation( Order::idOrder_t orderId );
};

} // namespace iqfeed
} // namespace tf
} // namespace ou
