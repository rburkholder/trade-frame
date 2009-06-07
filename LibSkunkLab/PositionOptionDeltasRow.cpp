#include "StdAfx.h"

#include <stdexcept>

#include "PositionOptionDeltasRow.h"

CPositionOptionDeltasRow::CPositionOptionDeltasRow(
  CInstrument::pInstrument_t pInstrument
  ) 
  : m_pInstrument( pInstrument ), m_sSymbol( pInstrument->GetSymbolName() )
{
  std::stringstream ss;
  ss << pInstrument->GetUnderlyingName() 
    << " " << OptionSide::ShortName[ pInstrument->GetOptionSide() ]
    << " " << pInstrument->GetExpiryYear()
    << "/" << pInstrument->GetExpiryMonth()
    << " " << pInstrument->GetStrike();
}

CPositionOptionDeltasRow::~CPositionOptionDeltasRow(void) {
}
