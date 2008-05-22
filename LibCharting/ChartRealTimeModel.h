#pragma once

#include <vector>

#include "DatedDatum.h"
#include "BarFactory.h"

#include "ChartEntryIndicator.h"

#include "Delegate.h"

// possibly inherit from this to use with trading rules


class CChartRealTimeModel {
public:
  CChartRealTimeModel(void);
  virtual ~CChartRealTimeModel(void);
  void AddQuote( const CQuote &quote );
  void AddTrade( const CTrade &trade );
  Delegate<const CChartRealTimeModel &> OnDataChanged;

protected:
  std::vector<CQuote> m_vQuotes;
  std::vector<CTrade> m_vTrades;
  std::vector<CBar> m_vBars;
  CBar m_barUpdating;  // bar being updated by barfactory
  CBarFactory m_barFactory;
  void HandleNewBarStarted( const CBar &bar );
  void HandleBarUpdated( const CBar &bar );
  void HandleBarCompleted( const CBar &bar );

  CChartEntryIndicator m_ceAsks;
  CChartEntryIndicator m_ceBids;
  CChartEntryIndicator m_ceSpreadMidPoint;
  CChartEntryIndicator m_ceTrades;

private:
};
