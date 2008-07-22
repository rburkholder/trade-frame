7// CtlMarketDepth.cpp : implementation file
//

#include "stdafx.h"
//#include "GTScalp.h"
#include "CtlMarketDepth.h"

// CCtlMarketDepth

IMPLEMENT_DYNAMIC(CCtlMarketDepth, CCtlListCtrlBase)

CCtlMarketDepth::CCtlMarketDepth(): CCtlListCtrlBase() {
  nInsertedRows = ixLastNonBlankRow = ixCurrentRow = 0;
}

CCtlMarketDepth::~CCtlMarketDepth() {
}

void CCtlMarketDepth::InitControl( void ) {

  CCtlListCtrlBase::InitControl();
  
  m_ColumnHeaders.Append( new CLVColumn( _T( "MMID" ), LVCFMT_LEFT, 60 ) );
  m_ColumnHeaders.Append( new CLVColumn( _T( "Price" ), LVCFMT_RIGHT, 60 ) );
  m_ColumnHeaders.Append( new CLVColumn( _T( "Size" ), LVCFMT_RIGHT, 50 ) );
  m_ColumnHeaders.Append( new CLVColumn( _T( "Cum." ), LVCFMT_RIGHT, 50 ) );
  m_ColumnHeaders.Append( new CLVColumn( _T( "Own" ), LVCFMT_RIGHT, 40 ) );
  m_ColumnHeaders.Append( new CLVColumn( _T( "Time" ), LVCFMT_RIGHT, 60 ) );

  Resize();
}

void CCtlMarketDepth::Resize( void ) {
  while ( GetCountPerPage() > nInsertedRows ) {
    InsertItem( nInsertedRows, "" );
    ++nInsertedRows;
  }
}

void CCtlMarketDepth::AssignRow( 
     int n, Colour::enumColour colour, const char* szMMID, int nSize, double price, int nOwn, const char *pTime ) {
       char conv[ 30 ];
  if ( n >= nInsertedRows ) {
    // overflow
  }
  else {
    if ( 0 == n ) {
      nCumulativeCount = 0;
    }
    SetTextBkColor(colour);
    nCumulativeCount += nSize;
    SetItemText( n, 0, szMMID );
    sprintf_s( conv, 30, "%0.2f", price );
    SetItemText( n, 1, conv );
    _itoa( nSize, conv, 10 );
    SetItemText( n, 2, conv );
    _itoa( nCumulativeCount, conv, 10 );
    SetItemText( n, 3, conv );
    _itoa( nOwn, conv, 10 );
    SetItemText( n, 4, conv );
    SetItemText( n, 5, pTime );
    ixCurrentRow = n;
  }
}

void CCtlMarketDepth::BlankRemainingRows( void ) {
  int ix = ixCurrentRow;
  while ( ixCurrentRow < ixLastNonBlankRow ) {
    ++ixCurrentRow;
    SetItemText( ixCurrentRow, 0, "" );
    SetItemText( ixCurrentRow, 1, "" );
    SetItemText( ixCurrentRow, 2, "" );
    SetItemText( ixCurrentRow, 3, "" );
    SetItemText( ixCurrentRow, 4, "" );
    SetItemText( ixCurrentRow, 5, "" );
  }
  ixLastNonBlankRow = ix;
}

BEGIN_MESSAGE_MAP(CCtlMarketDepth, CCtlListCtrlBase)
END_MESSAGE_MAP()

// CCtlMarketDepth message handlers


