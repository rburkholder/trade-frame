#pragma once

#include "ChartEntryBase.h"

#include <string>
#include <vector>

// level markers (horizontal lines at a price level)

class CChartEntryMark :
  public CChartEntryBase {
public:
  CChartEntryMark(void);
  virtual ~CChartEntryMark(void);
  void AddMark( double price, EColor color, std::string name = "" );
protected:
  std::vector<EColor> m_vColor;
  std::vector<std::string> m_vName;
private:
};
