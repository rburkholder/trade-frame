#include "StdAfx.h"
#include "ChartRealTimeTreeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CChartRealTimeTreeView, CGUIFrameBase)

CChartRealTimeTreeView::CChartRealTimeTreeView(void) : CGUIFrameBase() {
  m_sDialogTitle = "Symbols";
  CGUIFrameBase::SetPosition( 0, 0, 200, 400 );
  CGUIFrameBase::Create( );

  CRect clientRect;
  CFrameWnd::GetClientRect(&clientRect);
  CRect treeRect( clientRect.left + 5, clientRect.top + 5, clientRect.right - 5, clientRect.bottom - 5 );
  m_Tree.Create( WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | WS_TABSTOP | WS_CHILD , treeRect, this, 1 );
}

CChartRealTimeTreeView::~CChartRealTimeTreeView(void) {
}

BEGIN_MESSAGE_MAP(CChartRealTimeTreeView, CGUIFrameBase)
	ON_WM_DESTROY()
  ON_WM_SIZE( )
END_MESSAGE_MAP()

void CChartRealTimeTreeView::OnDestroy()  {
	CGUIFrameBase::OnDestroy();
}

afx_msg void CChartRealTimeTreeView::OnSize(UINT nType, int cx, int cy) {
  CGUIFrameBase::OnSize(nType,cx,cy);
}

