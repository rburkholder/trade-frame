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

#include "LibCommon/Delegate.h"

#include "LibTimeSeries/DatedDatum.h"

class CSymbol {
public:
  CSymbol( const std::string sName );
  virtual ~CSymbol(void);

  const std::string &Name( void ) { return m_sSymbolName; };

  typedef const CQuote &quote_t;
  typedef Delegate<quote_t>::OnMessageHandler quotehandler_t;

  typedef const CTrade &trade_t;
  typedef Delegate<trade_t>::OnMessageHandler tradehandler_t;

  typedef const CMarketDepth &depth_t;
  typedef Delegate<depth_t>::OnMessageHandler depthhandler_t;

  virtual void AddOnOpenHandler( tradehandler_t );
  virtual void RemoveOnOpenHandler( tradehandler_t );
  
  virtual bool AddQuoteHandler( quotehandler_t );
  virtual bool RemoveQuoteHandler( quotehandler_t );

  virtual bool AddTradeHandler( tradehandler_t ); 
  virtual bool RemoveTradeHandler( tradehandler_t );
  size_t GetTradeHandlerCount( void ) { return m_OnTrade.Size(); };

  virtual bool AddDepthHandler( depthhandler_t );
  virtual bool RemoveDepthHandler( depthhandler_t );

  bool QuoteWatchNeeded( void ) { return !m_OnQuote.IsEmpty(); };
  bool TradeWatchNeeded( void ) { return !m_OnTrade.IsEmpty(); };
  bool DepthWatchNeeded( void ) { return !m_OnDepth.IsEmpty(); };
protected:
  std::string m_sSymbolName;

  Delegate<trade_t> m_OnOpen;  // when open value changes
  Delegate<quote_t> m_OnQuote; 
  Delegate<trade_t> m_OnTrade;
  Delegate<depth_t> m_OnDepth;

private:
};


