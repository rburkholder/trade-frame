// VisibleRow.cpp : implementation file
//

#include "stdafx.h"
//#include "GTScalp.h"
#include "VisibleRow.h"

#include <iostream>

// CVisibleRow

//IMPLEMENT_DYNAMIC(CVisibleRow, CWnd)

CVisibleRow::CVisibleRow() {

  crAccount1 = Colour::LightSeaGreen;
  crAccount2 = Colour::LightGreen;
  crPrice = Colour::LightSteelBlue;
  crColumnHeader = Colour::LightGoldenrodYellow;
  crPriceHighlight = Colour::SkyBlue;
  crBidHighlight = Colour::DodgerBlue;
  crAskHighlight = Colour::Magenta;

  m_veAcct1BidPending.SetBackColorDefault( crAccount1 );
  m_veAcct2BidPending.SetBackColorDefault( crAccount2 );
  m_vePrice.SetBackColorDefault( crPrice );
  m_veAcct1AskPending.SetBackColorDefault( crAccount1 );
  m_veAcct2AskPending.SetBackColorDefault( crAccount2 );

  m_rElements.push_back( &m_veAcct1PL );
  m_rElements.push_back( &m_veAcct2PL );
  m_rElements.push_back( &m_veAcctPL );
  m_rElements.push_back( &m_veAcct1BidPending );
  m_rElements.push_back( &m_veAcct2BidPending );
  m_rElements.push_back( &m_veBidQuan );
  m_rElements.push_back( &m_vePrice );
  m_rElements.push_back( &m_veAskQuan );
  m_rElements.push_back( &m_veAcct1AskPending );
  m_rElements.push_back( &m_veAcct2AskPending );
  m_rElements.push_back( &m_veTicks );
  m_rElements.push_back( &m_veVolume );
  m_rElements.push_back( &m_veTickBuyVolume );
  m_rElements.push_back( &m_veTickSellVolume );
  m_rElements.push_back( &m_veStaticIndicators );
  m_rElements.push_back( &m_veDynamicIndicators );

}

int CVisibleRow::ElementWidths[] = { 60, 60, 60, 50, 50, 30, 80, 30, 50, 50, 40, 60, 60, 60, 80, 100, 0 }; 
char* CVisibleRow::ElementColumnNames[] = {
  _T("Acct1 PL"), _T("Acct2 PL"), _T("Acct PL"), 
  _T("Acct1"), _T("Acct2"), _T("Bid"),
  _T("Price"),
  _T("Ask"), _T("Acct1"), _T("Acct2"),
  _T("Ticks"), _T("Volume"), _T("BuyVol"), _T("SellVol"), 
  _T("Static"), _T("Dynamic"),
  _T("")
};

int CVisibleRow::RowWidth() {
  static int width = 0;
  if ( 0 == width ) {
    int *pWidths = ElementWidths;
    while ( 0 != *pWidths ) {
      width += *(pWidths++) + 1;
    }
    CString s;
    s.Format( "RowWidth = %d", width );
    //theApp.pConsoleMessages->WriteLine( s );
    cout << s << endl;
  }
  return width;
}

CVisibleRow::~CVisibleRow() {
  //CWnd::~CWnd();
//  CVisibleElement *pElement;
//  for ( unsigned int ix = 0; ix < size_t( ElementWidths); ix++ ) {
//  }
  m_veAcct1PL.DestroyWindow();
  m_veAcct2PL.DestroyWindow();
  m_veAcctPL.DestroyWindow();
  m_veAcct1BidPending.DestroyWindow();
  m_veAcct2BidPending.DestroyWindow();
  m_veBidQuan.DestroyWindow();
  m_vePrice.DestroyWindow();
  m_veAskQuan.DestroyWindow();
  m_veAcct1AskPending.DestroyWindow();
  m_veAcct2AskPending.DestroyWindow();
  m_veTicks.DestroyWindow();
  m_veVolume.DestroyWindow();
  m_veTickBuyVolume.DestroyWindow();
  m_veTickSellVolume.DestroyWindow();
  m_veStaticIndicators.DestroyWindow();
  m_veDynamicIndicators.DestroyWindow();

}

void CVisibleRow::CreateElement( CVisibleElement *pElement, bool bIsHeader,
                                int yPos, int *pxPos, int Width, int Height,
                                DWORD dwStyle,CWnd *pParentWnd )  {
  pElement->Create(NULL, dwStyle, CRect( *pxPos, yPos, *pxPos + Width - 1, yPos + Height - 1 ), pParentWnd );
  if ( bIsHeader ) {
    pElement->SetBackColor( crColumnHeader );
    pElement->SetCanFocus( false );
  }
  *pxPos += Width + 1;
}

void CVisibleRow::Create( int RowHeight, CPoint &Origin, CWnd *pParentWnd, bool bIsHeader ) {
  int *pWidth = ElementWidths;
  DWORD dwStyle = WS_CHILD|WS_VISIBLE|SS_CENTER|SS_NOTIFY;
  int xPos = Origin.x;
  for ( unsigned int ix = 0; ix < m_rElements.size(); ix++ ) {
    CVisibleElement *pElement = m_rElements[ix];
    CreateElement( pElement, bIsHeader, 
      Origin.y, &xPos, *(pWidth++), RowHeight, 
      dwStyle, pParentWnd );
    if ( bIsHeader ) {
      pElement->SetText( ElementColumnNames[ix] );
    }
  }
}

void CVisibleRow::HighlightBid( bool b ) {
  if ( b ) {
    m_veAcct1BidPending.SetBackColor( crBidHighlight );
    m_veAcct2BidPending.SetBackColor( crBidHighlight );
            m_veBidQuan.SetBackColor( crBidHighlight );
              m_vePrice.SetBackColor( crBidHighlight );
  }
  else {
    m_veAcct1BidPending.SetBackColor(  );
    m_veAcct2BidPending.SetBackColor(  );
            m_veBidQuan.SetBackColor(  );
              m_vePrice.SetBackColor(  );
  }
}

void CVisibleRow::HighlightAsk( bool b ) {
  if ( b ) {
    m_veAcct1AskPending.SetBackColor( crAskHighlight );
    m_veAcct2AskPending.SetBackColor( crAskHighlight );
            m_veAskQuan.SetBackColor( crAskHighlight );
              m_vePrice.SetBackColor( crAskHighlight );
  }
  else {
    m_veAcct1AskPending.SetBackColor(  );
    m_veAcct2AskPending.SetBackColor(  );
            m_veAskQuan.SetBackColor(  );
              m_vePrice.SetBackColor(  );
  }
}

void CVisibleRow::HighlightPrice( bool b ) {
  if ( b ) {
      m_vePrice.SetBackColor( crPriceHighlight );
    m_veBidQuan.SetBackColor( crPriceHighlight );
    m_veAskQuan.SetBackColor( crPriceHighlight );
  }
  else {
      m_vePrice.SetBackColor(  );
    m_veBidQuan.SetBackColor(  );
    m_veAskQuan.SetBackColor(  );
  }
}

//BEGIN_MESSAGE_MAP(CVisibleRow, CWnd)
//END_MESSAGE_MAP()



// CVisibleRow message handlers


