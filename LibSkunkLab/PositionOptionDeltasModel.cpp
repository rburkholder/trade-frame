#include "StdAfx.h"

#include <string>

#include "TradingEnumerations.h"

#include "PositionOptionDeltasModel.h"

CPositionOptionDeltasModel::CPositionOptionDeltasModel(void) {
}

CPositionOptionDeltasModel::~CPositionOptionDeltasModel(void) {
  for ( std::vector<CPositionOptionDeltasRow*>::iterator iter = m_vDeltaRows.begin(); iter != m_vDeltaRows.end(); ++iter ) {
    CPositionOptionDeltasRow* pRow = *iter;
    *iter = NULL;
    delete pRow;
  }
}

void CPositionOptionDeltasModel::Add( CInstrument::pInstrument_t pInstrument) {
  if ( InstrumentType::Option != pInstrument->GetInstrumentType() ) {
    std::string s = "Instrument " + pInstrument->GetSymbolName() + " is not an Option.";
    throw std::invalid_argument( s );
  }
  CPositionOptionDeltasRow *pRow = new CPositionOptionDeltasRow( pInstrument ); 
  m_vDeltaRows.push_back( pRow );
}
