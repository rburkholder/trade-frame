#pragma once

#include "GTStockX.h"
#include "TradeFrame.h"
#include "VuPendingOrders.h"
#include "TimeSeries.h"
#include "IQFeed.h"
#include "IQFeedRetrieveHistory.h"
#include "PivotGroup.h"
#include "VuChart.h"
#include "ChartDirector\FinanceChart.h"
#include "VuMarketDepth.h"
#include "GeneratePeriodicRefresh.h"

#include <string>
using namespace std;

class CTradingLogic {
public:
  CTradingLogic(CString sSymbol);
  ~CTradingLogic(void);

  void Acct1LmtBuy( long Shares, double dblPrice );
  void Acct1LmtSell( long Shares, double dblPrice );
  void Acct2LmtBuy( long Shares, double dblPrice );
  void Acct2LmtSell( long Shares, double dblPrice );
  void Acct1CancelBid();
  void Acct2CancelBid();
  void Acct1CancelAsk();
  void Acct2CancelAsk();

  const char *GetSymbol( void ) { return (LPCTSTR) m_sSymbol; };

protected:
  CString m_sSymbol;

  //CPivotGroup *m_pPivotGroup;

  IQFeedHistoryHD *phd;
  CBars *pBars;
  void OnDailyBarHistoryDone( IQFeedHistory *pHistory );

  IQFeedHistoryHT *pht;
  CQuotes *pQuotes;
  CTrades *pTrades;
  void OnTickHistoryDone( IQFeedHistory *pHistory );

  //CQuote *pLatestQuote;
  //CTrade *pLatestTrade;
  
  CGTStockX *stkSession1;
  CGTStockX *stkSession2;

  CTradeFrame *pTradeFrame;
  //CVuKeyValues *pVuKeyValues;
  CVuPendingOrders *pVuPendingOrders;
  //CVuChart *pChartDaily;
  CVuChart *pChartIntraDay;
  CVuMarketDepth *pVuMarketDepth;

  static const int nLevels = 10;
  long rShares[ nLevels ];
  double rPrices[ nLevels ];

  void OnQuoteLevel1( const CQuote &quote );
  void OnPrint( const CTrade &trade );
  void OnPrintCommon( const CTrade &trade );
  //void OnLevel2( char, long, double, LPSTR );
  void OnLevel2( const CMarketDepth &md );
  void OnCancelAcct1Order( DWORD_PTR );
  void OnCancelAcct2Order( DWORD_PTR );

  enum EState { Init, History, RealTime } m_state;

  bool m_bFirstTradeFound;
  double m_dblOpen;

  enum EMarketDepthToUpdate { MDUpdateNone=0, MDUpdateBid=1, MDUpdateAsk=2, MDUpdateBoth=3 } stateMDUpdate;
  void UpdateMarketDepth( void );

  CGeneratePeriodicRefresh m_refresh;
private:
  void OnIQFeedTimeMessage( CIQFTimeMessage *pMsg );
  void OnPeriodicRefresh( CGeneratePeriodicRefresh *pMsg );
};
