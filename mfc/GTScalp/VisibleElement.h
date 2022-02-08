#pragma once

// CVisibleElement

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

class CVisibleElement : public CWnd {
	DECLARE_DYNAMIC(CVisibleElement)

public:
	CVisibleElement();
	CVisibleElement(COLORREF BackColor, COLORREF ForeColor);
	virtual ~CVisibleElement();
	virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle,
				const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);
  void SetBackColor( COLORREF color );
  void SetForeColor( COLORREF color );
  void SetBackColorDefault( COLORREF color );
  void SetForeColorDefault( COLORREF color );
  void SetBackColor( void );  // sets color to default
  void SetForeColor( void );  // sets color to default
  void SetCanFocus(bool);
  void SetText( LPCTSTR Text );
  LPCTSTR GetText();
  //void RedrawElement();

  typedef FastDelegate0<> OnLeftClickHandler;
  void SetOnLeftClick( OnLeftClickHandler function ) {
    OnLeftClick = function;
  }
  typedef FastDelegate0<> OnRightClickHandler;
  void SetOnRightClick( OnRightClickHandler function ) {
    OnRightClick = function;
  }

private:
  CString m_sText;
  CFont m_font;
  CBrush m_brush;
  CPen m_penNull;
  CPen m_pen;

  COLORREF m_BackColorDefault;
  COLORREF m_BackColor;
  COLORREF m_ForeColorDefault;
  COLORREF m_ForeColor;

  OnLeftClickHandler OnLeftClick;
  OnRightClickHandler OnRightClick;

  void ResetFocusBox();
  void SetFocusBox();

  bool m_bShowFocusBox;
  bool m_bCanShowFocus;

  static CVisibleElement *pLastFocus;  // used for changing focus


protected:

  afx_msg void OnPaint( );
  afx_msg HBRUSH OnCtlColor( CDC*, CWnd*, UINT );
  afx_msg BOOL OnEraseBkgnd( CDC * );
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnDestroy( );
  afx_msg void OnMouseMove( UINT, CPoint );
  afx_msg void OnKeyDown( UINT, UINT, UINT );
  afx_msg void OnKeyUp( UINT, UINT, UINT );

  DECLARE_MESSAGE_MAP()
};


