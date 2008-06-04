#pragma once

#include "..\LibCommon\Delegate.h"
#include "..\LibTrading\Symbol.h"
#include "IQFeedMessages.h"

#include <vector>
#include <string>
using namespace std;

class CIQFeedSymbol : public CSymbol {
  // needs to be created by IQFeed object as IQFeed injects the actual commands
    friend class CIQFeedProvider;
    friend class CIQFeed;
public:
  CIQFeedSymbol(const string &symbol);
  virtual ~CIQFeedSymbol(void);

  //bool Watch();
  //bool UnWatch();

  Delegate<CIQFeedSymbol*> OnFundamentalMessage;
  Delegate<CIQFeedSymbol*> OnUpdateMessage;
  Delegate<CIQFeedSymbol*> OnSummaryMessage;
  Delegate<CIQFeedSymbol*> OnNewsMessage;

  // Public for RowKeyValues.  Pass in a structure sometime.
  // Public for CVuChartArmsIntraDay.  Pass in structure sometime.
  // Fundamentals
  string m_sOptionRoots;
  int m_AverageVolume;
  string m_sCompanyName;
  int m_Precision;
  double m_dblHistoricalVolatility;
  double m_dblStrikePrice;

  // Update/Summary
  ptime m_dtLastTrade;
  double m_dblTrade;
  double m_dblChange; // last - close
  int m_nTotalVolume;
  int m_nTradeSize;
  double m_dblHigh;
  double m_dblLow;
  double m_dblBid;
  double m_dblAsk;
  int m_nBidSize;
  int m_nAskSize;
  int m_nOpenInterest;
  double m_dblOpen;
  double m_dblClose;
  int m_cntTrades;
  bool m_bNewTrade;
  bool m_bNewQuote;
  bool m_bNewOpen;


protected:
  unsigned short m_cnt;  // used for watch/unwatch
  enum enumQStatus { qUnknown, qFound, qNotFound } m_QStatus;

  void SetQuoteTradeWatchInProgress( void ) { m_bQuoteTradeWatchInProgress = true; };
  void ResetQuoteTradeWatchInProgress( void ) { m_bQuoteTradeWatchInProgress = false; };
  bool GetQuoteTradeWatchInProgress( void ) { return m_bQuoteTradeWatchInProgress; };
  bool m_bQuoteTradeWatchInProgress;
  void SetDepthWatchInProgress( void ) { m_bDepthWatchInProgress = true; };
  void ResetDepthWatchInProgress( void ) { m_bDepthWatchInProgress = false; };
  bool GetDepthWatchInProgress( void ) { return m_bDepthWatchInProgress; };
  bool m_bDepthWatchInProgress;

  void HandleFundamentalMessage( CIQFFundamentalMessage *pMsg );
  void HandleUpdateMessage( CIQFUpdateMessage *pMsg );
  void HandleSummaryMessage( CIQFSummaryMessage *pMsg );
  void HandleNewsMessage( CIQFNewsMessage *pMsg );

  void DecodePricingMessage( CIQFPricingMessage *pMsg );

private:
};
