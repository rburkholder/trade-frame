#pragma once

#include "stdafx.h"
#include "..\LibCommon\Colour.h"

#include <string>
using namespace std;

// ==== CDeltaMarkerBase

class CDeltaMarkerBase {
public:
  CDeltaMarkerBase( CListCtrl *pListCtrl, int row, int col );
  virtual ~CDeltaMarkerBase(void);

  //void SetBackground( EColor color ) { colBackground = color; );
  //void SetForeground( EColor color ) { colForeground = color; );
  //EColor GetBackground( void ) { return colBackground; };
  //EColor GetForeground( void ) { return colForeground; };

  Colour::enumColour colBackground;
  Colour::enumColour colForeground;

  LPCTSTR GetString( void );

protected:

  CString sValue;
  CString sFormat;
  Colour::enumColour colRising;
  Colour::enumColour colFalling;
  int nPrecision;
  bool bFirstFound;

  CListCtrl *m_pListCtrl;
  int m_nRow;  // row in ListCtrl
  int m_nCol;  // col in ListCtrl

private:

};

// ==== DeltaMarker
// *** all template function definitions need to be in the header file

template<class T> class DeltaMarker: public CDeltaMarkerBase {
public:
  DeltaMarker<T>( CListCtrl *pListCtrl, int row, int col );
  virtual ~DeltaMarker<T>(void);

  void SetPrecision( int Precision );
  bool SetValue( T ); // returns true if value has changed

protected:

  T curValue;

private:

};

template<class T> DeltaMarker<T>::DeltaMarker( CListCtrl *pListCtrl, int row, int col ) 
: CDeltaMarkerBase( pListCtrl, row, col ) {
  SetPrecision( 2 );
}

template<class T> DeltaMarker<T>::~DeltaMarker(void) {
}

template<class T> void DeltaMarker<T>::SetPrecision( int precision ) {
  if ( typeid( double ) == typeid( T ) ) {
    nPrecision = precision;
    sFormat.Format( _T( "%s%df" ), _T( "%0." ), precision );
  }
  else {
    if ( typeid( int ) == typeid( T ) ) {
      sFormat = _T( "%d" );
    }
    else {
      if ( typeid( CString ) == typeid( T ) ) {
        sFormat = _T( "%s" );
      }
    }
  }
}

template<class T> bool DeltaMarker<T>::SetValue( T value ) {

  bool bChanged = false;
  if ( bFirstFound ) {
    if ( value > curValue ) {
      bChanged = true;
      colForeground = colRising;
      m_pListCtrl->SetTextColor( colForeground );
    }
    if ( value < curValue ) {
      bChanged = true;
      colForeground = colFalling;
      m_pListCtrl->SetTextColor( colForeground );
    }
  }
  else {
    bFirstFound = true;
    bChanged = true;
  }
  if ( bChanged ) {
    curValue = value;
    sValue.Format( sFormat, value );
    // TODO: may want to do this on a timeout trigger if things get too busy
    m_pListCtrl->SetItemText( m_nRow, m_nCol, (LPCTSTR) sValue );
  }

  return bChanged;
  
}

// ==== CDMDouble

class CDMDouble: public DeltaMarker<double> {
public:
  CDMDouble( CListCtrl *pListCtrl, int row, int col );
  virtual ~CDMDouble(void);
  //bool SetValue( double value ) { return DeltaMarker<double>::SetValue( value ); };
};

// ==== CDMInt

class CDMInt: public DeltaMarker<int> {
public:
  CDMInt( CListCtrl *pListCtrl, int row, int col );
  virtual ~CDMInt(void);
  //bool SetValue( int value ) { return DeltaMarker::SetValue( value ); };
};

// ==== CDMString

class CDMString: public DeltaMarker<CString> {
public:
  CDMString( CListCtrl *pListCtrl, int row, int col );
  virtual ~CDMString(void);
};

