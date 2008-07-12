#pragma once

// for example, the zig zag indicator
// also use for high and low indicators, keep redoing last segment so stretches to 
//   right hand edge (optionally, perhaps in a super class)

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
