#pragma once

#include "ChartEntryBase.h"

// need to add in std::string for commenting the shape

class CChartEntryShape :
  public CChartEntryBaseWithTime {
public:
  CChartEntryShape(void);
  virtual ~CChartEntryShape(void);
  enum EShape { EBuy, ESell, EBuyStop, ESellStop, EFillLong, EFillShort };
  void SetShape( EShape shape, EColor color ) { m_eShape = shape; m_eColor = color; };
protected:
  EShape m_eShape;
private:
};
