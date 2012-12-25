#pragma once

#include "GTStockX.h"
#include "TradeFrame.h"
#include "VuPendingOrders.h"
#include "TimeSeries.h"
#include "IQFeedProviderSingleton.h"
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
  CTradingLogic(CString sSymbol, CGTSessionX *pGTSession1, CGTSessionX *pGTSession2 );
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
  CIQFeedProviderSingleton m_IQFeedProvider;

  IQFeedHistoryHD* phd;
  Bars* pBars;
  void OnDailyBarHistoryDone( IQFeedHistory *pHistory );

  IQFeedHistoryHT* pht;
  Quotes* pQuotes;
  Trades* pTrades;
  void OnTickHistoryDone( IQFeedHistory *pHistory );

  //Quote *pLatestQuote;
  //Trade *pLatestTrade;
  
  CGTStockX *stkSession1;
  CGTStockX *stkSession2;

  CTradeFrame* pTradeFrame;
  //CVuKeyValues *pVuKeyValues;
  CVuPendingOrders* pVuPendingOrders;
  //CVuChart *pChartDaily;
  CVuChart* pChartIntraDay;
  CVuMarketDepth* pVuMarketDepth;

  static const int nLevels = 10;
  long rShares[ nLevels ];
  double rPrices[ nLevels ];

  void OnQuoteLevel1( const Quote &quote );
  void OnPrint( const Trade &trade );
  void OnPrintCommon( const Trade &trade );
  //void OnLevel2( char, long, double, LPSTR );
  void OnLevel2( const MarketDepth &md );
  void OnCancelAcct1Order( DWORD_PTR );
  void OnCancelAcct2Order( DWORD_PTR );

  enum EState { Init, History, RealTime } m_state;

  bool m_bFirstTradeFound;
  double m_dblOpen;

  enum EMarketDepthToUpdate { MDUpdateNone=0, MDUpdateBid=1, MDUpdateAsk=2, MDUpdateBoth=3 } stateMDUpdate;
  void UpdateMarketDepth( void );

  CGeneratePeriodicRefresh m_refresh;
private:
  void OnIQFeedTimeMessage( IQFTimeMessage* pMsg );
  void OnPeriodicRefresh( CGeneratePeriodicRefresh* pMsg );
};
