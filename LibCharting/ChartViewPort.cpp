#include "StdAfx.h"
#include "ChartViewPort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CChartViewPort::CChartViewPort( size_t ix, CWnd* pParent )
: m_ixViewPort( ix ), CGUIFrameBase()
{
  CGUIFrameBase::SetPosition( 0, 0, 900, 400 );
  CGUIFrameBase::Create( );

  //CRect clientRect;
  //CFrameWnd::GetClientRect(&clientRect);
  //CRect chartRect( clientRect.left + 5, clientRect.top + 5, clientRect.right - 5, clientRect.bottom - 5 );
}

CChartViewPort::~CChartViewPort(void) {
}

BEGIN_MESSAGE_MAP(CChartViewPort, CGUIFrameBase)
	ON_WM_DESTROY()
  ON_WM_SIZE( )
  //ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CChartViewPort::OnDestroy()  {
	CGUIFrameBase::OnDestroy();
}

afx_msg void CChartViewPort::OnSize(UINT nType, int cx, int cy) {
  CGUIFrameBase::OnSize(nType,cx,cy);
  CRect clientRect;
  CFrameWnd::GetClientRect(&clientRect);
  CRect chartRect( clientRect.left + 5, clientRect.top + 5, clientRect.right - 5, clientRect.bottom - 5 );
  //m_prtView->SetChartDimensions( chartRect.Width(), chartRect.Height() );
  //std::cout << "size " << chartRect.Width() << ", " << chartRect.Height() << endl;
}


