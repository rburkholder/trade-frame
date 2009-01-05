#pragma once

#include "boost/shared_ptr.hpp"

#include "Instrument.h"

class CPortfolioRecord {
public:
  CPortfolioRecord(void);
  ~CPortfolioRecord(void);

  typedef boost::shared_ptr<CPortfolioRecord> pPortfolioRecord_t;

protected:
  CInstrument *m_pInstrument;
  int m_nPosition;
  double m_dblPrice;
  double m_dblAverageCost;
private:
};
