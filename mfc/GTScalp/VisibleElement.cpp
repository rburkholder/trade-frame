// RowStatic.cpp : implementation file
//

#include "stdafx.h"
#include "VisibleElement.h"
#include "Colour.h"

#include <iostream>
using namespace std;

// CVisibleElement

IMPLEMENT_DYNAMIC(CVisibleElement, CWnd)

CVisibleElement::CVisibleElement() : CWnd() {

  m_BackColorDefault = m_BackColor = ::GetSysColor( COLOR_WINDOW );
  m_ForeColorDefault = m_ForeColor = ::GetSysColor( COLOR_WINDOWTEXT );
  m_bShowFocusBox = false;
  m_bCanShowFocus = true;
}

CVisibleElement::CVisibleElement( COLORREF BackColor, COLORREF ForeColor ) {

  m_BackColor = BackColor;
  m_ForeColor = ForeColor;
  m_bShowFocusBox = false;
}

CVisibleElement::~CVisibleElement() {
  pLastFocus = NULL;
}

CVisibleElement *CVisibleElement::pLastFocus = NULL;  // used for changing focus

BOOL CVisibleElement::Create(LPCTSTR lpszText, DWORD dwStyle,
                             const RECT& rect, CWnd* pParentWnd, UINT nID) {

  // style is provided CVisibleRow

  BOOL b = CWnd::Create(_T("STATIC"), NULL, dwStyle, rect, pParentWnd, nID );

  m_sText = lpszText;
  m_font.CreateFontA( rect.bottom - rect.top - 1, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, 
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
    DEFAULT_PITCH | FF_SWISS, _T("Arial") );

  m_brush.CreateSolidBrush( m_BackColor );
  m_penNull.CreatePen(PS_NULL, 0, Colour::Black);
  m_pen.CreatePen(PS_SOLID, 1, Colour::Black );

  return b;
}

void CVisibleElement::SetBackColorDefault( COLORREF color ) {
  m_BackColorDefault = color;
  m_BackColor = color;
}

void CVisibleElement::SetForeColorDefault( COLORREF color ) {
  m_ForeColorDefault = color;
  m_ForeColor = color;
}

void CVisibleElement::SetBackColor( COLORREF color ) {
  m_BackColor = color;
  //CWnd::Invalidate();
}

void CVisibleElement::SetForeColor( COLORREF color ) {
  m_ForeColor = color;
  //CWnd::Invalidate();
}

void CVisibleElement::SetBackColor( void ) {
  m_BackColor = m_BackColorDefault;
  //CWnd::Invalidate();
}

void CVisibleElement::SetForeColor( void ) {
  m_ForeColor = m_ForeColorDefault;
  //CWnd::Invalidate();
}

void CVisibleElement::SetText( LPCTSTR sText ) {
  m_sText = sText;
  CWnd::Invalidate();
}

LPCTSTR CVisibleElement::GetText() {
  return m_sText;
}

//void CVisibleElement::RedrawElement( void ) {
//  CWnd::Invalidate();
//}

void CVisibleElement::SetCanFocus( bool bCanFocus ) {
  m_bCanShowFocus = bCanFocus;
}

void CVisibleElement::ResetFocusBox() {
  m_bShowFocusBox = false;
  CWnd::Invalidate();
}

void CVisibleElement::SetFocusBox() {
  m_bShowFocusBox = true;
  CWnd::Invalidate();
}

BEGIN_MESSAGE_MAP(CVisibleElement, CWnd)
  ON_WM_PAINT( )
  ON_WM_CTLCOLOR( )
  ON_WM_ERASEBKGND( )
  ON_WM_LBUTTONUP( )
  ON_WM_RBUTTONUP( )
  ON_WM_DESTROY( )
//  ON_WM_KEYDOWN()
//  ON_WM_KEYUP()
  ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// http://www.codeproject.com/cpp/FastDelegate.asp

// CVisibleElement message handlers

afx_msg void CVisibleElement::OnMouseMove( UINT nFlags, CPoint point ) {

  if ( m_bCanShowFocus ) {
    if ( this != pLastFocus ) {
      if ( NULL != pLastFocus ) {
        (pLastFocus->ResetFocusBox)();
      }
      pLastFocus = this;
      SetFocusBox();
    }
  }

  CWnd::OnMouseMove( nFlags, point );
}

afx_msg void CVisibleElement::OnDestroy( ) {

  m_font.DeleteObject();
  m_brush.DeleteObject();
  m_penNull.DeleteObject();
  m_pen.DeleteObject();

  CWnd::OnDestroy();
}

afx_msg void CVisibleElement::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags) {
  CString s;
  s.Format( "CVisibleElement::OnKeyDown: char %d, repcnt %d, flags %d", 
    nChar, nRepCnt, nFlags);
  //theApp.pConsoleMessages->WriteLine(s);
  cout << s << endl;
  CWnd::OnKeyDown( nChar, nRepCnt, nFlags );
}

afx_msg void CVisibleElement::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags ) {
  CString s;
  s.Format( "CVisibleElement::OnKeyUp: char %d, repcnt %d, flags %d", 
    nChar, nRepCnt, nFlags);
  //theApp.pConsoleMessages->WriteLine(s);
  cout << s << endl;
  CWnd::OnKeyUp( nChar, nRepCnt, nFlags );
}

afx_msg void CVisibleElement::OnLButtonUp(UINT nFlags, CPoint point) {
  if ( m_bCanShowFocus ) {
    CString s;
    s.Format( "CVisibleElement::OnLButtonUp: flags %d, x %d, y %d", 
      nFlags, point.x, point.y);
    //theApp.pConsoleMessages->WriteLine(s);
    cout << s << endl;
    if ( NULL != OnLeftClick ) {
      OnLeftClick();
    }
  }
  CWnd::OnLButtonUp(nFlags, point);
}

afx_msg void CVisibleElement::OnRButtonUp(UINT nFlags, CPoint point) {
  if ( m_bCanShowFocus ) {
    CString s;
    s.Format( "CVisibleElement::OnRButtonUp: flags %d, x %d, y %d", 
      nFlags, point.x, point.y);
    //theApp.pConsoleMessages->WriteLine(s);
    cout << s << endl;
    if ( NULL != OnRightClick ) {
      OnRightClick();
    }
  }
  CWnd::OnRButtonUp(nFlags, point);
}

afx_msg HBRUSH CVisibleElement::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
  HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
  return hbr;
}

afx_msg void CVisibleElement::OnPaint( ) {

  CPaintDC dc(this);

  CRect rc;
  GetClientRect(&rc);

  dc.SelectObject(m_font);
  
  dc.SetBkMode( TRANSPARENT );  // allows existing background to show through text
  dc.DrawText(m_sText, &rc, DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_NOCLIP);
}

afx_msg BOOL CVisibleElement::OnEraseBkgnd(CDC* pDC) {

  CRect rc;
  GetClientRect( &rc );
  //rc.InflateRect(1,1);  // **
  pDC->SelectObject( m_brush );
  //pDC->SelectObject( m_penNull );
  pDC->SelectObject( m_pen );
  //pDC->SelectObject( m_bShowFocusBox ? m_penFocus : m_penNull );
  //pDC->DrawFocusRect(&rc);
  pDC->FillSolidRect(&rc, m_BackColor );
  if ( m_bCanShowFocus ) {
    //pDC->DrawEdge(&rc, m_bShowFocusBox ? EDGE_RAISED : EDGE_ETCHED, BF_RECT|BF_RECT );
    if ( m_bShowFocusBox ) {
//      pDC->DrawEdge(&rc, EDGE_BUMP, BF_RECT|BF_FLAT|BF_MONO );
      pDC->Rectangle( &rc );
    }
    
  }

  return 1;
}


