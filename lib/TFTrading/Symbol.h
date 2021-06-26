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

#include <string>

#include <boost/shared_ptr.hpp>

#include <OUCommon/Delegate.h>
#include <TFTimeSeries/DatedDatum.h>

#include "Instrument.h"

// change the Add/Remove...Handlers from virtual to CRTP?
//  probably not,

namespace ou { // One Unified
namespace tf { // TradeFrame

class SymbolBase {
public:

  using pInstrument_t = Instrument::pInstrument_t;
  using pInstrument_cref =  Instrument::pInstrument_cref;
  using symbol_id_t = Instrument::idInstrument_t;
  //   may need to use provider specific name in alternate instrument name in Instrument

  // class should only be constructed with valid instrument, which already has general name as well as provider specific names
  SymbolBase( pInstrument_t pInstrument );  // instrument supplied name
  SymbolBase( pInstrument_t pInstrument, const std::string& sName );  // provider specific name
  virtual ~SymbolBase(void);

  const symbol_id_t& GetId() const { return m_id; };  // used for mapping and indexing
  pInstrument_t GetInstrument() { return m_pInstrument; };

  using quote_t = const Quote&;
  using quotehandler_t = ou::Delegate<quote_t>::OnDispatchHandler;

  using trade_t = const Trade&;
  using tradehandler_t = ou::Delegate<trade_t>::OnDispatchHandler;

  using depth_t = const MarketDepth&;
  using depthhandler_t = ou::Delegate<depth_t>::OnDispatchHandler;

  using greek_t = const Greek&;
  using greekhandler_t = ou::Delegate<greek_t>::OnDispatchHandler;

  // these handlers are typically updated through the provider, rather than through client code
  bool AddQuoteHandler( quotehandler_t );
  bool RemoveQuoteHandler( quotehandler_t );
  size_t GetQuoteHandlerCount() const { return m_OnQuote.Size(); };

  void AddOnOpenHandler( tradehandler_t );
  void RemoveOnOpenHandler( tradehandler_t );
  size_t GetOpenHandlerCount() { return m_OnOpen.Size(); };

  bool AddTradeHandler( tradehandler_t );
  bool RemoveTradeHandler( tradehandler_t );
  size_t GetTradeHandlerCount() { return m_OnTrade.Size(); };

  bool AddDepthHandler( depthhandler_t );
  bool RemoveDepthHandler( depthhandler_t );
  size_t GetDepthHandlerCount() { return m_OnDepth.Size(); };

  bool AddGreekHandler( greekhandler_t );
  bool RemoveGreekHandler( greekhandler_t );
  size_t GetGreekHandlerCount() { return m_OnGreek.Size(); };

  // these are typically used by the provider only
  bool  OpenWatchNeeded() const { return !m_OnOpen.IsEmpty(); };
  bool QuoteWatchNeeded() const { return !m_OnQuote.IsEmpty(); };
  bool TradeWatchNeeded() const { return !m_OnTrade.IsEmpty(); };
  bool DepthWatchNeeded() const { return !m_OnDepth.IsEmpty(); };
  bool GreekWatchNeeded() const { return !m_OnGreek.IsEmpty(); };

protected:

  symbol_id_t m_id;  // may be overwritten with provider specific override
  pInstrument_t m_pInstrument;  // composition rather than inheritance as same instrument refers to different market data and order providers

  ou::Delegate<trade_t> m_OnOpen;  // first value upon market opening
  ou::Delegate<quote_t> m_OnQuote;
  ou::Delegate<trade_t> m_OnTrade;
  ou::Delegate<depth_t> m_OnDepth;
  ou::Delegate<greek_t> m_OnGreek;

private:

};

template <typename S>  // S for Provider specific CRTP Symbol type
class Symbol: public SymbolBase {
public:
  using pSymbol_t = boost::shared_ptr<S>;
  Symbol( pInstrument_t pInstrument ) : SymbolBase( pInstrument ) {};
  Symbol( pInstrument_t pInstrument, const std::string& sName ) : SymbolBase( pInstrument, sName ) {};
  virtual ~Symbol( void ) {};
protected:
private:

};

} // namespace tf
} // namespace ou
