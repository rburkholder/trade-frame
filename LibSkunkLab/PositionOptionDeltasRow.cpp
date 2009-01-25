#include "StdAfx.h"
#include "PositionOptionDeltasRow.h"

CPositionOptionDeltasRow::CPositionOptionDeltasRow(
  const std::string &sSymbolUnderlying, const std::string &sSymbol,
  double dblStrike, const ptime &dtExpiry) 
: m_sSymbolUnderlying( sSymbolUnderlying ), m_sSymbol( sSymbol ),
  m_dblStrike( dblStrike ), m_dtExpiry( dtExpiry )
{
}

CPositionOptionDeltasRow::~CPositionOptionDeltasRow(void) {
}
