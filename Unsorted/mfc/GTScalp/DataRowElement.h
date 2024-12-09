#pragma once
#pragma warning( disable : 4996 )
//#define _CRT_SECURE_NO_WARNINGS
// To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.

#include "VisibleElement.h"
#include <list>

using namespace std;

template<class T> class CDataRowElement {
public:
  CDataRowElement<T>(bool *pChanged, const char *pFormat );  
  virtual ~CDataRowElement<T>(void);
  void SetVisibleElement( CVisibleElement *ve ) { m_pve = ve; };
  virtual void Set( T val );
  T Get( void );
  virtual void UpdateVisibleElement( void );
protected:
  const static int nBufSize = 40;
  const static int nFormatSize = 10;
  bool *m_pChanged;  // set when something changes, force row update
  T m_val;  // the value stored in this element
  char m_str[ nBufSize ]; // place for a formatted string
  char m_pFormat[ nFormatSize ];  // place for format
  CVisibleElement *m_pve;  
private:
};

template<class T> CDataRowElement<T>::CDataRowElement( bool *pChanged, const char *pFormat ) {
  ASSERT( NULL != pChanged );
  m_pChanged = pChanged;
  ASSERT( NULL != pFormat );
  strcpy_s( m_pFormat, nFormatSize, pFormat );
  m_val = 0;
  m_str[ 0 ] = 0;
  m_pve = NULL;
}

template<class T> CDataRowElement<T>::~CDataRowElement() {
  m_pve = NULL;
  m_pChanged = NULL;
}

template<class T> void CDataRowElement<T>::Set( T val ) {
  m_val = val;
  *m_pChanged = true;
}

template<class T> T CDataRowElement<T>::Get() {
  return m_val;
}

template<class T> void CDataRowElement<T>::UpdateVisibleElement() {
  if ( m_val != 0 ) {
    _snprintf( m_str, nBufSize, m_pFormat, m_val );
  }
  else {
    m_str[ 0 ] = 0;
  }
  if ( NULL != m_pve ) m_pve->SetText( m_str );
}

// Class CDataRowElementDoublePrice

class CDataRowElementPrice: public CDataRowElement<int> {
public:
  CDataRowElementPrice( bool *pChanged, const char *pFormat, char *psUnadornedPrice  );
  virtual ~CDataRowElementPrice( void ) {};
  void UpdateVisibleElement();
protected:
  char m_sUnadornedPrice[ nBufSize ];
private:
};

// Class CDataRowElementStaticIndicator

class CDataRowElementStaticIndicator: public CDataRowElement<char *> {
public:
  CDataRowElementStaticIndicator( bool *pChanged, const char *pFormat );
  virtual ~CDataRowElementStaticIndicator( void ) { delete m_val; };
  void UpdateVisibleElement( void );
  void Set( const char *val ); // indicator string to add
protected:
  static const int nCommentSize = 128;
private:
};

// Class CDataRowElementDynamicIndicator

class CDataRowElementDynamicIndicator: public CDataRowElement<char *> {
public:
  CDataRowElementDynamicIndicator( bool *pChanged, const char *pFormat );
  virtual ~CDataRowElementDynamicIndicator( void ) { delete m_val; };
  void UpdateVisibleElement();
  void Set( char *str ) { };
  void Add( const char *str );
  void Remove( const char *str );
protected:
  static const int nCommentSize = 128;
  list<char *> m_listIndicators;
  bool m_bListChanged;
private:
};



