#pragma once

#include "BasketTradeSymbolBase.h"

#include <map>

#include "BarFactory.h"
#include "Order.h"
#include "OrderManager.h"
#include "Delegate.h"

#include "ChartEntryBars.h"
#include "ChartEntryIndicator.h"
#include "ChartEntryMark.h"
#include "ChartEntrySegments.h"
#include "ChartEntryShape.h"

class CBasketTradeSymbolInfo: public CBasketTradeSymbolBase {
public:
  explicit CBasketTradeSymbolInfo( 
    const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy );
  explicit CBasketTradeSymbolInfo( std::stringstream *pStream );
  ~CBasketTradeSymbolInfo( void );

  Delegate<CBasketTradeSymbolInfo *> OnBasketTradeSymbolInfoChanged;

  double GetProposedEntryCost() { return m_dblProposedEntryCost; };
  int GetQuantityForEntry() { return m_nQuantityForEntry; };
  const std::string &GetSymbolName( void ) { return m_status.sSymbolName; };
  void WriteTradesAndQuotes( const std::string &sPathPrefix );

  void StartTrading( void );
  void StopTrading( void );

  structFieldsForDialog *GetDialogFields( void ) { return &m_status; };  // needs to come after structure definition

  void CalculateTrade( structCommonModelInformation *pParameters  );

protected:
  void HandleQuote( const CQuote &quote );
  void HandleTrade( const CTrade &trade );
  void HandleOpen( const CTrade &trade );

  void Initialize( void );

  double m_dblDayOpenPrice;
  double m_dblPriceForEntry;
  double m_dblAveragePriceOfEntry;
  double m_dblMarketValueAtEntry;
  double m_dblCurrentMarketPrice;
  double m_dblCurrentMarketValue;
  int m_nQuantityForEntry;
  int m_nWorkingQuantity;
  double m_dblAllocatedWorkingFunds;
  double m_dblExitPrice;
  double m_dblProposedEntryCost;
  enum enumPositionState { 
    Init, WaitingForOpen, 
    WaitingForThe3Bars, 
    WaitingForOrderFulfillmentLong, WaitingForOrderFulfillmentShort,
    WaitingForLongExit, WaitingForShortExit,
    WaitingForExit, Exited } m_PositionState;
  enum enumTradingState {
    WaitForFirstTrade,
    WaitForOpeningTrade, 
    WaitForOpeningBell, // 9:30 exchange time
    SetOpeningRange,  // spend 5 minutes here
    ActiveTrading,  // through the day
    NoMoreTrades, // 15:40 exchange time
    CancelTrades, // 15:45 exchange time
    CloseTrades,  // 15:50 exchange time
    DoneTrading   // 15:50 exchange time
  } m_TradingState;
  double m_dblAveBarHeight;
  double m_dblTrailingStopDistance;

  bool m_bDoneTheLong, m_bDoneTheShort;
  bool m_bFoundOpeningTrade;

  size_t m_nBarsInSequence;
  size_t m_nOpenCrossings;
  static const size_t m_nMaxCrossings = 2;  
  static const size_t m_nBarWidth = 30;  //seconds

  enum enumStateForRangeCalc {
    WaitForRangeStart,
    CalculatingRange, 
    DoneCalculatingRange
  } m_OpeningRangeState, m_RTHRangeState;

  COrderManager m_OrderManager;

  void HandleBarFactoryBar( const CBar &bar );
  void HandleOrderFilled( COrder *pOrder );

  std::map<unsigned long, COrder*> m_mapActiveOrders;
  std::map<unsigned long, COrder*> m_mapCompletedOrders;

  CBarFactory m_1MinBarFactory;
  CQuotes m_quotes;
  CTrades m_trades;
  CBars m_bars;

  CChartEntryBars m_ceBars;
  CChartEntryVolume m_ceBarVolume;
  CChartEntryIndicator m_ceTrades;
  CChartEntryVolume m_ceTradeVolume;
  CChartEntryIndicator m_ceQuoteBids;
  CChartEntryIndicator m_ceQuoteAsks;
  CChartEntryMark m_ceLevels; // open, pivots
  CChartEntryShape m_ceOrdersBuy;
  CChartEntryShape m_ceOrdersSell;
  CChartEntrySegments m_ceZigZag;
  CChartEntryIndicator m_ceBollinger20TickAverage;
  CChartEntryIndicator m_cdBollinger20TickUpper;
  CChartEntryIndicator m_cdBollinger20TickLower;
  CChartEntryIndicator m_ceHi;
  CChartEntryIndicator m_ceLo;

  //CChartEntryIndicator  // some sort of indicator for order flow:  trade direction vs quotes, etc


private:
  CBasketTradeSymbolInfo( const CBasketTradeSymbolInfo & );  // disallow copy construction
};
