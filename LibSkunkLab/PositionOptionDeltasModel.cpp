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

void CPositionOptionDeltasModel::Add( CProviderInterface *pDataProvider, CInstrument::pInstrument_t pInstrument) {
  if ( InstrumentType::Option != pInstrument->GetInstrumentType() ) {
    std::string s = "Instrument " + pInstrument->GetSymbolName() + " is not an Option.";
    throw std::invalid_argument( s );
  }
  CPositionOptionDeltasRow *pRow = new CPositionOptionDeltasRow( m_vDeltaRows.size(), pDataProvider, pInstrument ); 
  m_vDeltaRows.push_back( pRow );
  pRow->SetOnRowUpdated( MakeDelegate( this, &CPositionOptionDeltasModel::QueueUpdatedRowIndex ) );

  //pRow->SetOnRowUpdated( MakeDelegate( pRow, &CPositionOptionDeltasVu::HandleRowUpdate ) );
  if ( NULL != OnInstrumentAdded ) OnInstrumentAdded( m_vDeltaRows.size() );  // append blank visual row.

}

void CPositionOptionDeltasModel::QueueUpdatedRowIndex( vDeltaRows_t::size_type ix ) {
  m_setUpdatedRows.insert( ix );
}

const std::string& CPositionOptionDeltasModel::GetItem( vDeltaRows_t::size_type ixRow, int ixItem ) {
  const static string s( "a" );
  return s;
}