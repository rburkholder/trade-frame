#pragma once
#include "IQFeedMessages.h"

#include "FastDelegate.h"
using namespace fastdelegate;

#include "Delegate.h"

#include <vector>
#include <string>
using namespace std;

class CIQFSymbol {
  // needs to be created by IQFeed object as IQFeed injects the actual commands
public:
  CIQFSymbol(const string &symbol);
  virtual ~CIQFSymbol(void);

  string m_sSymbol;

  bool Watch();
  bool UnWatch();

  void HandleFundamentalMessage( CIQFFundamentalMessage *pMsg );
  void HandleUpdateMessage( CIQFUpdateMessage *pMsg );
  void HandleSummaryMessage( CIQFSummaryMessage *pMsg );
  void HandleNewsMessage( CIQFNewsMessage *pMsg );

  //typedef FastDelegate1<CIQFSymbol*> OnFundamentalMessageHandler;
  //Delegate<OnFundamentalMessageHandler,CIQFSymbol*> OnFundamentalMessage;
  Delegate<CIQFSymbol*> OnFundamentalMessage, OnUpdateMessage, OnSummaryMessage, OnNewsMessage;

  //typedef FastDelegate1<CIQFSymbol*> OnUpdateMessageHandler;
  //Delegate<OnUpdateMessageHandler,CIQFSymbol*> OnUpdateMessage;
  //Delegate<CIQFSymbol*> OnUpdateMessage;

  //typedef FastDelegate1<CIQFSymbol*> OnSummaryMessageHandler;
  //Delegate<OnSummaryMessageHandler,CIQFSymbol*> OnSummaryMessage;
  //Delegate<CIQFSymbol*> OnSummaryMessage;

  // Fundamentals
  string sOptionRoots;
  int AverageVolume;
  string sCompanyName;
  int Precision;
  double dblHistoricalVolatility;
  double dblStrikePrice;

  // Update/Summary
  ptime dtLastTrade;
  double dblTrade;
  double dblChange; // last - close
  int TotalVolume;
  int TradeSize;
  double dblHigh;
  double dblLow;
  double dblBid;
  double dblAsk;
  int BidSize;
  int AskSize;
  int OpenInterest;
  double dblOpen;
  double dblClose;
  int cntTrades;

protected:
  unsigned short m_cnt;  // used for watch/unwatch
  enum enumQStatus { qUnknown, qFound, qNotFound };
  enumQStatus QStatus;

private:
  void HandlePricingMessage( CIQFPricingMessage *pMsg );
};
