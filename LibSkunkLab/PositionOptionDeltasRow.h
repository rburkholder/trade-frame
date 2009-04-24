#pragma once

#include <string>

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include "PositionOptionDeltasWnd.h"

class CPositionOptionDeltasRow {
public:
  CPositionOptionDeltasRow( 
    const std::string &sSymbolUnderlying, const std::string &sSymbol,
    double dblStrike, const ptime &dtExpiry
    );
  ~CPositionOptionDeltasRow(void);
protected:
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( COLHDR_DELTAS_ARRAY ), COLHDR_DELTAS_EMIT_DefineVars, ~ )
  /*
  std::string m_sSymbolUnderlying;
  std::string m_sSymbol;
  double m_dblStrike;
  ptime m_dtExpiry;
  double m_dblBid;
  int m_nBidSize;
  double m_dblSpread;
  double m_dblAsk;
  int m_nAskSize;
  int m_nPosition;
  double m_dblAverageCost;
  double m_dblDelta;
  double m_dblGamma;
  double m_dblUnrealizedPL;
  double m_dblRealizedPL;
  */
private:
};
