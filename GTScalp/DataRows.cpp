#include "StdAfx.h"
#include "DataRows.h"

CDataRows::CDataRows() {
  m_Iterator = m_mpDataRow.end();
  nEntry = 0;
  InitializeCriticalSection( &csEntry );
}

CDataRows::~CDataRows(void) {
  m_Iterator = m_mpDataRow.end();
  for each ( pair<unsigned int, CDataRow*> p in m_mpDataRow ) {
    delete p.second;
  }
}

CDataRow *CDataRows::Insert( unsigned int ix ) {
  CDataRow *row;
  row = new CDataRow( ix, (double) ix / m_Multiplier );
  m_mpDataRow.insert( m_Pair( ix, row ) );
  m_Iterator = m_mpDataRow.end();  //unknown position upon new insertion
  if ( NULL != OnRowInsert ) OnRowInsert( row );
  return row;
}

CDataRow *CDataRows::operator []( unsigned int ix ) {
  // optimized for random then sequential access forward
  // provide for mechanism where client can obtain an interator for use here,
  //   but would need to have ability to be invalidated when another 
  //   client does an insert, so use pointer to structure this method can use,
  //   ie, client gets a handle to an interator

  ++nEntry;
//  ASSERT( nEntry < 2 );
  EnterCriticalSection( &csEntry );
  CDataRow *row = NULL;

  bool bDoRandomSearch = 
     ( m_mpDataRow.end() == m_Iterator ) // if we are at end of list
     ;
  if ( !bDoRandomSearch ) {
    // have done a random search already, or in existing sequential search
    bDoRandomSearch = true;  // random search anyway if all conditions not met
    ++m_Iterator;
    if ( m_mpDataRow.end() != m_Iterator ) {
      if ( m_Iterator->first == ix ) { // check needed in case of holes in row list
        row = m_Iterator->second;
        bDoRandomSearch = false; // we did find something so no further search
      }
    }
  }

  if ( bDoRandomSearch ) {
    // random search, which creates rows if they don't already exist
    m_Iterator = m_mpDataRow.find( ix );
    if ( m_mpDataRow.end() == m_Iterator ) {
      row = Insert( ix );
    }
    else {
      row = m_Iterator ->second;
    }
  }

  LeaveCriticalSection( &csEntry );
  --nEntry;
  ASSERT( NULL != row );
  return row;
}

