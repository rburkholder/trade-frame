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

#include <boost/asio/io_context.hpp>
#include <boost/asio/io_context_strand.hpp>

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
  using idSymbol_t = Instrument::idInstrument_t;
  //   may need to use provider specific name in alternate instrument name in Instrument

  // class should only be constructed with valid instrument, which already has general name as well as provider specific names
  SymbolBase( pInstrument_t pInstrument );  // instrument supplied name
  SymbolBase( pInstrument_t pInstrument, const std::string& sName );  // provider specific name
  virtual ~SymbolBase();

  const idSymbol_t& GetId() const { return m_id; };  // used for mapping and indexing
  pInstrument_t GetInstrument() { return m_pInstrument; };

  using quote_t = const Quote&;
  using quotehandler_t = ou::Delegate<quote_t>::OnDispatchHandler;

  using trade_t = const Trade&;
  using tradehandler_t = ou::Delegate<trade_t>::OnDispatchHandler;

  using depthbymm_t = const DepthByMM&;
  using depthbymmhandler_t = ou::Delegate<depthbymm_t>::OnDispatchHandler;

  using depthbyorder_t = const DepthByOrder&;
  using depthbyorderhandler_t = ou::Delegate<depthbyorder_t>::OnDispatchHandler;

  using greek_t = const Greek&;
  using greekhandler_t = ou::Delegate<greek_t>::OnDispatchHandler;

  // these handlers are typically updated through the provider, rather than through client code
  void AddOnOpenHandler( tradehandler_t );
  void RemoveOnOpenHandler( tradehandler_t );
  size_t GetOpenHandlerCount() const { return m_OnOpen.Size(); };

  bool AddQuoteHandler( quotehandler_t );
  bool RemoveQuoteHandler( quotehandler_t );
  size_t GetQuoteHandlerCount() const { return m_OnQuote.Size(); };

  bool AddTradeHandler( tradehandler_t );
  bool RemoveTradeHandler( tradehandler_t );
  size_t GetTradeHandlerCount() const { return m_OnTrade.Size(); };

  bool AddDepthByMMHandler( depthbymmhandler_t );
  bool RemoveDepthByMMHandler( depthbymmhandler_t );
  size_t GetDepthByMMHandlerCount() const { return m_OnDepthByMM.Size(); };

  bool AddDepthByOrderHandler( depthbyorderhandler_t );
  bool RemoveDepthByOrderHandler( depthbyorderhandler_t );
  size_t GetDepthByOrderHandlerCount() const { return m_OnDepthByOrder.Size(); };

  bool AddGreekHandler( greekhandler_t );
  bool RemoveGreekHandler( greekhandler_t );
  size_t GetGreekHandlerCount() const { return m_OnGreek.Size(); };

  // these are typically used by the provider only
  bool OpenWatchNeeded() const { return !m_OnOpen.IsEmpty(); };
  bool QuoteWatchNeeded() const { return !m_OnQuote.IsEmpty(); };
  bool TradeWatchNeeded() const { return !m_OnTrade.IsEmpty(); };
  bool DepthByMMWatchNeeded() const { return !m_OnDepthByMM.IsEmpty(); };
  bool DepthByOrderWatchNeeded() const { return !m_OnDepthByOrder.IsEmpty(); };
  bool GreekWatchNeeded() const { return !m_OnGreek.IsEmpty(); };

  void SetContext( boost::asio::io_context& );

protected:

  idSymbol_t m_id;  // may be overwritten with provider specific override
  pInstrument_t m_pInstrument;  // composition rather than inheritance as same instrument refers to different market data and order providers

  ou::Delegate<trade_t> m_OnOpen;  // first value upon market opening
  ou::Delegate<quote_t> m_OnQuote;
  ou::Delegate<trade_t> m_OnTrade;
  ou::Delegate<depthbymm_t> m_OnDepthByMM;
  ou::Delegate<depthbyorder_t> m_OnDepthByOrder;
  ou::Delegate<greek_t> m_OnGreek;

  bool m_bStrand;
  std::unique_ptr<boost::asio::io_context::strand> m_pStrand;

private:

};

template <typename S>  // S for Provider specific CRTP Symbol type
class Symbol: public SymbolBase {
public:
  using pSymbol_t = boost::shared_ptr<S>;
  Symbol( pInstrument_t pInstrument ) : SymbolBase( pInstrument ) {};
  Symbol( pInstrument_t pInstrument, const std::string& sName ) : SymbolBase( pInstrument, sName ) {};
  virtual ~Symbol() {};
protected:
private:

};

} // namespace tf
} // namespace ou
