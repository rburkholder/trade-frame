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

class CSymbolBase {
public:

  typedef CInstrument::pInstrument_t pInstrument_t;
  typedef CInstrument::pInstrument_cref pInstrument_cref;
  typedef CInstrument::idInstrument_t symbol_id_t;
  //   may need to use provider specific name in alternate instrument name in CInstrument

  // class should only be constructed with valid instrument, which already has general name as well as provider specific names
  CSymbolBase( pInstrument_t pInstrument );  // instrument supplied name
  CSymbolBase( pInstrument_t pInstrument, const std::string& sName );  // provider specific name
  virtual ~CSymbolBase(void);

  const symbol_id_t& GetId( void ) { return m_id; };  // used for mapping and indexing
  pInstrument_t GetInstrument( void ) { return m_pInstrument; };

  typedef const CQuote& quote_t;
  typedef ou::Delegate<quote_t>::OnMessageHandler quotehandler_t;

  typedef const CTrade& trade_t;
  typedef ou::Delegate<trade_t>::OnMessageHandler tradehandler_t;

  typedef const CMarketDepth& depth_t;
  typedef ou::Delegate<depth_t>::OnMessageHandler depthhandler_t;

  typedef const CGreek& greek_t;
  typedef ou::Delegate<greek_t>::OnMessageHandler greekhandler_t;

  // these handlers are typically updated through the provider, rather than through client code
  bool AddQuoteHandler( quotehandler_t );
  bool RemoveQuoteHandler( quotehandler_t );
  size_t GetQuoteHandlerCount( void ) { return m_OnQuote.Size(); };

  void AddOnOpenHandler( tradehandler_t );
  void RemoveOnOpenHandler( tradehandler_t );
  size_t GetOpenHandlerCount( void ) { return m_OnOpen.Size(); };
  
  bool AddTradeHandler( tradehandler_t ); 
  bool RemoveTradeHandler( tradehandler_t );
  size_t GetTradeHandlerCount( void ) { return m_OnTrade.Size(); };

  bool AddDepthHandler( depthhandler_t );
  bool RemoveDepthHandler( depthhandler_t );
  size_t GetDepthHandlerCount( void ) { return m_OnDepth.Size(); };

  bool AddGreekHandler( greekhandler_t );
  bool RemoveGreekHandler( greekhandler_t );
  size_t GetGreekHandlerCount( void ) { return m_OnGreek.Size(); };

  // these are typically used by the provider only
  bool  OpenWatchNeeded( void ) { return !m_OnOpen.IsEmpty(); };
  bool QuoteWatchNeeded( void ) { return !m_OnQuote.IsEmpty(); };
  bool TradeWatchNeeded( void ) { return !m_OnTrade.IsEmpty(); };
  bool DepthWatchNeeded( void ) { return !m_OnDepth.IsEmpty(); };
  bool GreekWatchNeeded( void ) { return !m_OnGreek.IsEmpty(); };

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
class CSymbol: public CSymbolBase {
public:
  typedef boost::shared_ptr<S> pSymbol_t;
  CSymbol( pInstrument_t pInstrument ) : CSymbolBase( pInstrument ) {};
  CSymbol( pInstrument_t pInstrument, const std::string& sName ) : CSymbolBase( pInstrument, sName ) {};
  ~CSymbol( void ) {};
protected:
private:

};

} // namespace tf
} // namespace ou
