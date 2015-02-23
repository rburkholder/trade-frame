/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

//#include "StdAfx.h"

#include "ChartRealTimeTreeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CChartRealTimeTreeView, CGUIFrameBase)

CChartRealTimeTreeView::CChartRealTimeTreeView(void) : CGUIFrameBase() {
  m_sDialogTitle = "Symbols";
  CGUIFrameBase::SetPosition( 0, 0, 200, 600 );
  CGUIFrameBase::Create( );

  CRect clientRect;
  CFrameWnd::GetClientRect(&clientRect);
  CRect treeRect( clientRect.left + 5, clientRect.top + 5, clientRect.right - 5, clientRect.bottom - 5 );
  m_Tree.Create( WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | WS_TABSTOP | WS_CHILD | TVS_SHOWSELALWAYS, treeRect, this, 2001 );
}

CChartRealTimeTreeView::~CChartRealTimeTreeView(void) {
}

BEGIN_MESSAGE_MAP(CChartRealTimeTreeView, CGUIFrameBase)
  ON_WM_CREATE()
	ON_WM_DESTROY()
  ON_WM_SIZE( )
  ON_NOTIFY(TVN_SELCHANGED, 2001, &CChartRealTimeTreeView::OnTvnSelChangedTree)
END_MESSAGE_MAP()

int CChartRealTimeTreeView::OnCreate( LPCREATESTRUCT lpCreateStruct ) {
  return CGUIFrameBase::OnCreate( lpCreateStruct );
}

void CChartRealTimeTreeView::OnDestroy()  {
	CGUIFrameBase::OnDestroy();
}

afx_msg void CChartRealTimeTreeView::OnSize(UINT nType, int cx, int cy) {
  CGUIFrameBase::OnSize(nType,cx,cy);
}

void CChartRealTimeTreeView::OnTvnSelChangedTree(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
  m_Tree.ProcessSelectionChanged( pNMTreeView );
  *pResult = 0;
}

