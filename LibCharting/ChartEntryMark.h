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
  void AddMark( double price, Colour::enumColour colour, const std::string &name );
  virtual void AddDataToChart( XYChart *pXY, structChartAttributes *pAttributes );
protected:
  std::vector<Colour::enumColour> m_vColour;
  std::vector<std::string> m_vName;
private:
};
