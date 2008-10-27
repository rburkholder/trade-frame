#pragma once
#include "baskettradesymbolbase.h"

#include "BarFactory.h"

#include "ChartEntryBars.h"
#include "ChartEntryIndicator.h"
#include "ChartEntryMark.h"
#include "ChartEntrySegments.h"
#include "ChartEntryShape.h"

class CBasketTradeSymbolSkeleton :  public CBasketTradeSymbolBase
{
public:
  CBasketTradeSymbolSkeleton( const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy );
  virtual ~CBasketTradeSymbolSkeleton(void);

  int GetQuantityForEntry() { return m_nQuantityForEntry; };
  void WriteTradesAndQuotes( const std::string &sPathPrefix );

  virtual void StartTrading( void );
  virtual void StopTrading( void );

protected:

  CBarFactory m_1MinBarFactory;
  CBars m_bars;
  CQuotes m_quotes;
  CTrades m_trades;

  CChartEntryBars m_ceBars;
  CChartEntryVolume m_ceBarVolume;
  CChartEntryIndicator m_ceTrades;
  CChartEntryVolume m_ceTradeVolume;
  CChartEntryIndicator m_ceQuoteBids;
  CChartEntryIndicator m_ceQuoteAsks;
  CChartEntryMark m_ceLevels; // open, pivots
  CChartEntryShape m_ceOrdersBuy;
  CChartEntryShape m_ceOrdersSell;

  static const size_t m_nBarWidth = 180;  //seconds

  bool m_bFoundOpeningTrade;

  virtual void HandleOrderFilled( COrder *pOrder );

  void Initialize( void );
  virtual void ModelReady( CBars *pBars );

  void HandleBarFactoryBar( const CBar &bar );
  void HandleQuote( const CQuote &quote );
  void HandleTrade( const CTrade &trade );
  void HandleOpen( const CTrade &trade );
private:
};
