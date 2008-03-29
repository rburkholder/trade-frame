#include "StdAfx.h"
#include "HDF5TimeSeriesIterator.h"
#include "HDF5TimeSeriesContainer.h"

CHDF5TimeSeriesIterator::CHDF5TimeSeriesIterator( CHDF5TimeSeriesContainer *pContainer ):
  m_pContainer( pContainer ), 
  m_ItemIndex( 0 ),
  m_bPointingSomewhere( false ), 
  m_pDatum( NULL ) {
}

CHDF5TimeSeriesIterator::CHDF5TimeSeriesIterator( CHDF5TimeSeriesContainer *pContainer, hsize_t Index ):
  m_pContainer( pContainer ), 
  m_ItemIndex( Index ),
  m_bPointingSomewhere( true ), 
  m_pDatum( NULL ) /* but no data retrieved yet */ {
}

