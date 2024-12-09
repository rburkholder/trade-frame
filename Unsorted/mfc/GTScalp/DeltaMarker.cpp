// DeltaMarker.cpp : implementation file
//

#include "stdafx.h"
#include <typeinfo>
#include "DeltaMarker.h"

// ==== CDeltaMarkerBase

CDeltaMarkerBase::CDeltaMarkerBase( CListCtrl *pListCtrl, int row, int col ) {
  colBackground = Colour::Black;
  colForeground = Colour::White;
  colRising = Colour::Green;
  colFalling = Colour::Red;
  nPrecision = 0;
  bFirstFound = false;
  m_pListCtrl = pListCtrl;
  m_nRow = row;
  m_nCol = col;
  //m_pListCtrl->SetBkColor( colBackground );
  //m_pListCtrl->SetTextBkColor( colBackground );
  //m_pListCtrl->SetTextColor( colForeground );
}

CDeltaMarkerBase::~CDeltaMarkerBase(void) {
  m_pListCtrl = NULL;
}

LPCTSTR CDeltaMarkerBase::GetString(void) {
  return (LPCTSTR) sValue;
}

// ==== CDMDouble

CDMDouble::CDMDouble( CListCtrl *pListCtrl, int row, int col )
  :DeltaMarker<double>( pListCtrl, row, col ) { 
  curValue = 0; 
}

CDMDouble::~CDMDouble(void) {
  //DeltaMarker<double>::~DeltaMarker();
}

// ==== CDMInt

CDMInt::CDMInt( CListCtrl *pListCtrl, int row, int col )  
  :DeltaMarker<int>( pListCtrl, row, col ) { 
  curValue = 0; 
}

CDMInt::~CDMInt(void) {
  //DeltaMarker<int>::~DeltaMarker();
}

// ==== CDMString

CDMString::CDMString( CListCtrl *pListCtrl, int row, int col ) 
  :DeltaMarker<CString>( pListCtrl, row, col ) { 
}

CDMString::~CDMString(void) {
  //DeltaMarker<CString>::~DeltaMarker();
}