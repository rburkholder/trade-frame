#pragma once

// for example, the zig zag indicator

#include "ChartEntryBase.h"

class CChartEntrySegments :
  public CChartEntryBaseWithTime {
public:
  CChartEntrySegments(void);
  virtual ~CChartEntrySegments(void);
  void SetAttributes( EColor color, std::string name = "" );
protected:
  
private:
};
