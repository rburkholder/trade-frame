#pragma once
#include "baskettradesymbolbase.h"

#include "BarFactory.h"
#include "TimeSeriesSlidingWindowStats.h"

#include "ChartEntryBars.h"
#include "ChartEntryIndicator.h"
#include "ChartEntryMark.h"
#include "ChartEntrySegments.h"
#include "ChartEntryShape.h"

class CBasketTradeSymbolV2 :  public CBasketTradeSymbolBase
{
public:
  CBasketTradeSymbolV2( const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy );
  virtual ~CBasketTradeSymbolV2(void);

  int GetQuantityForEntry() { return m_nQuantityForEntry; };
  void WriteTradesAndQuotes( const std::string &sPathPrefix );

  virtual void StartTrading( void );
  virtual void StopTrading( void );

protected:

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

  CChartEntryIndicator m_ceAvg;
  CChartEntryIndicator m_ceBBUpper;
  CChartEntryIndicator m_ceBBLower;

  static const size_t m_nBarWidth = 180;  //seconds

  bool m_bFoundOpeningTrade;
  bool m_bFirstOrder;

  virtual void HandleOrderFilled( COrder *pOrder );

  void Initialize( void );
  virtual void ModelReady( CBars *pBars );

  void HandleBarFactoryBar( const CBar &bar );
  void HandleQuote( const CQuote &quote );
  void HandleTrade( const CTrade &trade );
  void HandleOpen( const CTrade &trade );

  double m_dblBaseLinePrice;
  double m_dblTradeMovingSum;
  double m_dblTradeMovingAvg;
  int m_cntMovingAverageTrades;  // used only for ramp up of averaging process
  static const int m_nMovingAverageValues = 5;
  int m_ixRemovalTrade;

  enum enumTradeSide {
    UnknownTradeSide, 
    GoingLong, GoingShort, TransitionLong, TransitionShort, 
    SearchForLong, SearchForShort,
    ConfirmLong, ConfirmShort, ConfirmLongCancel, ConfirmShortCancel,
    NoMoreTrades,
    CancelTrades,
    ClosePositions,
    NoMoreTrading
  } m_TradeSideState;

  static const double m_dblHysterisis;

  CTimeSeriesSlidingWindowStatsQuote *m_pQuoteSW0256;  //  4.2 minutes
  CTimeSeriesSlidingWindowStatsQuote *m_pQuoteSW0768;  // 12.8 minutes
  CTimeSeriesSlidingWindowStatsQuote *m_pQuoteSW2048;  // 34.1 minutes
  CTimeSeriesSlidingWindowStatsQuote *m_pQuoteSW20;  // 20 quotes
  CTimeSeriesSlidingWindowStatsTrade *m_pTradesSW20;  // 20 trades

private:
};
