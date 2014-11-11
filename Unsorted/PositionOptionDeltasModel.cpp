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

  if ( NULL != OnInstrumentCountChanged ) OnInstrumentCountChanged( m_vDeltaRows.size() );  // append row of visual items
}

void CPositionOptionDeltasModel::QueueUpdatedRowIndex( vDeltaRows_t::size_type ix ) {
  // used for updating list of updated rows from background update thread
  m_setUpdatedRows.insert( ix );
}

const std::string& CPositionOptionDeltasModel::GetItem( vDeltaRows_t::size_type ixRow, int ixItem ) {

  // assert row is within reason
  assert( ixRow >= 0 );
  assert( ixRow < m_vDeltaRows.size() );  // handles case when nothing is vector

  // assert item is within reason
  assert( ixItem >= 0 );
  assert( ixItem < COLHDR_DELTAS_ARRAY_ROW_COUNT );

  // get the item string
  return m_vDeltaRows[ ixRow ]->m_vVisibleItems[ ixItem ]->String();
}

void CPositionOptionDeltasModel::CheckForUpdatedRows() {
  // use m_setUpdatedRows to update GUI
  for ( std::set<vDeltaRows_t::size_type>::iterator begin = m_setUpdatedRows.begin();
    begin != m_setUpdatedRows.end();
    ++begin ) {
      if ( NULL != OnRowUpdated ) OnRowUpdated( *begin );
  }
  m_setUpdatedRows.clear();
}
