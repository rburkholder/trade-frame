#pragma once

#include <string>
#include "Delegate.h"

#include "..\LibTimeSeries\DatedDatum.h"

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


