#pragma once

#include "ChartEntryBase.h"

#include "DatedDatum.h"

class CChartEntryBars :
  public CChartEntryBaseWithTime {
public:
  CChartEntryBars(void);
  CChartEntryBars(unsigned int nSize);
  virtual ~CChartEntryBars(void);
  void AddBar( const CBar &bar );
protected:
  std::vector<double> m_vOpen;
  std::vector<double> m_vHigh;
  std::vector<double> m_vLow;
  std::vector<double> m_vClose;
  std::vector<int> m_vVolume;
private:
};
