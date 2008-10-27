#pragma once

#include "BasketTradeSymbolBase.h"

#include <map>

#include "BarFactory.h"
//#include "Delegate.h"

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

  int GetQuantityForEntry() { return m_nQuantityForEntry; };

  virtual void StartTrading( void );
  virtual void StopTrading( void );

protected:
  void Initialize( void );
  virtual void ModelReady( CBars *pBars );

  void HandleQuote( const CQuote &quote );
  void HandleTrade( const CTrade &trade );
  void HandleOpen( const CTrade &trade );

  double m_dblDayOpenPrice;
  double m_dblPriceForEntry;
  double m_dblAveragePriceOfEntry;
  double m_dblMarketValueAtEntry;
  double m_dblCurrentMarketPrice;
  double m_dblCurrentMarketValue;
  int m_nWorkingQuantity;
  double m_dblAllocatedWorkingFunds;
  double m_dblExitPrice;
  double m_dblAveBarHeight;
  double m_dblTrailingStopDistance;

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

  enum enumStateForRangeCalc {
    WaitForRangeStart,
    CalculatingRange, 
    DoneCalculatingRange
  } m_OpeningRangeState, m_RTHRangeState;

  bool m_bDoneTheLong, m_bDoneTheShort;
  bool m_bFoundOpeningTrade;

  size_t m_nBarsInSequence;
  size_t m_nOpenCrossings;
  static const size_t m_nMaxCrossings = 2;  
  static const size_t m_nBarWidth = 30;  //seconds

  void HandleBarFactoryBar( const CBar &bar );
  virtual void HandleOrderFilled( COrder *pOrder );

  CBarFactory m_1MinBarFactory;
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
