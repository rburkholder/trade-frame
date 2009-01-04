#pragma once

#include "Instrument.h"

class CPortfolioRecord {
public:
  CPortfolioRecord(void);
  ~CPortfolioRecord(void);
protected:
  CInstrument *m_pInstrument;
  int m_nPosition;
  double m_dblPrice;
  double m_dblAverageCost;
private:
};
