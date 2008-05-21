#pragma once

#include "ChartEntryBase.h"

class CChartEntryIndicator :
  public CChartEntryBaseWithTime {
public:
  CChartEntryIndicator(void);
  CChartEntryIndicator( unsigned int nSize );
  virtual ~CChartEntryIndicator(void);
  virtual void Reserve( unsigned int );
protected:
private:
};
