#pragma once

#include "Instrument.h"
#include "Delegate.h"

class CPortfolio {
public:
  CPortfolio(void);
  ~CPortfolio(void);

  struct structUpdatePortfolioRecord {
    CInstrument *pInstrument;
    int nPosition;
    double dblPrice;
    double dblAverageCost;
    structUpdatePortfolioRecord( CInstrument *pInstrument_, int nPosition_, double dblPrice_, double dblAverageCost_ )
      : pInstrument( pInstrument_ ), nPosition( nPosition_ ), dblPrice( dblPrice_ ), dblAverageCost( dblAverageCost_ ) {};
  };

  typedef const structUpdatePortfolioRecord &UpdatePortfolioRecord_t;
  typedef Delegate<UpdatePortfolioRecord_t>::OnMessageHandler UpdatePortfolioRecordHandler_t;

protected:
private:
};
