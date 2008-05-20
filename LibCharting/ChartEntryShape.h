#pragma once

#include "ChartEntryBase.h"

class CChartEntryShape :
  public CChartEntryBaseWithTime {
public:
  CChartEntryShape(void);
  virtual ~CChartEntryShape(void);
  enum EShape { EBuy, ESell, EBuyStop, ESellStop };
  void SetShape( EShape shape, EColor color ) { m_eShape = shape; m_eColor = color; };
protected:
  EShape m_eShape;
private:
};
