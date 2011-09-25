#include "StdAfx.h"
#include "DataRowElement.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Class CDataRowElementDoublePrice

CDataRowElementPrice::CDataRowElementPrice( bool *pChanged, const char *pFormat, char *psUnadornedPrice ) :
  CDataRowElement( pChanged, pFormat ) {
  strcpy_s( m_sUnadornedPrice, nBufSize, psUnadornedPrice );
  strcpy_s( m_str, nBufSize, psUnadornedPrice );
}

void CDataRowElementPrice::UpdateVisibleElement() {
  if ( m_val > 0 ) {
    _snprintf( m_str, nBufSize, "%d@%s", m_val, m_sUnadornedPrice );
  }
  else {
    strcpy_s( m_str, nBufSize, m_sUnadornedPrice );
  }
  if ( NULL != m_pve ) m_pve->SetText( m_str );
}

// Class CDataRowElementStaticIndicator

CDataRowElementStaticIndicator::CDataRowElementStaticIndicator( bool *pChanged, const char *pFormat ) :
  CDataRowElement( pChanged, pFormat ) {
  m_val = new char[ nCommentSize ];
  *m_val = 0;
}

void CDataRowElementStaticIndicator::Set( const char *val ) {
  if ( 0 != *m_val ) {
    strcat_s( m_val, nCommentSize, " " );
  }
  strcat_s( m_val, nCommentSize, val );
  *m_pChanged = true;
}

void CDataRowElementStaticIndicator::UpdateVisibleElement() {
  if ( NULL != m_pve ) m_pve->SetText( m_val );
}

// Class CDataRowElementDynamicIndicator

CDataRowElementDynamicIndicator::CDataRowElementDynamicIndicator( bool *pChanged, const char *pFormat ) :
  CDataRowElement( pChanged, pFormat ) {
  m_val = new char[ nCommentSize ];
  *m_val = 0;
}

void CDataRowElementDynamicIndicator::Add( const char *val ) {
  list<char *>::iterator iter;
  iter = m_listIndicators.begin();
  bool bFound = false;
  while ( m_listIndicators.end() != iter ) {
    if ( 0 == strcmp( val, *iter ) ) {
      bFound = true;
      break;
    }
    ++iter;
  }
  if ( bFound ) {
  }
  else {
    char *t = new char[ nFormatSize ];
    strcpy_s( t, nFormatSize, val );
    m_listIndicators.push_back( t );
    m_bListChanged = true;
    *m_pChanged = true;
  }
}

void CDataRowElementDynamicIndicator::Remove( const char *val ) {
  list<char *>::iterator iter;
  iter = m_listIndicators.begin();
  while ( m_listIndicators.end() != iter ) {
    if ( 0 == strcmp( val, *iter ) ) {
      delete *iter;
      m_listIndicators.erase( iter );
      m_bListChanged = true;
      *m_pChanged = true;
      break;
    }
    ++iter;
  }
}

void CDataRowElementDynamicIndicator::UpdateVisibleElement() {
  if ( m_bListChanged ) {
    *m_val = 0;
    list<char *>::iterator iter;
    iter = m_listIndicators.begin();
    while ( m_listIndicators.end() != iter ) {
      strcat_s( m_val, nCommentSize, *iter );
      strcat_s( m_val, nCommentSize, " " );
      ++iter;
    }
    m_bListChanged = false;
  }
  if ( NULL != m_pve ) m_pve->SetText( m_val );
}

