#include "StdAfx.h"
#include "ChartRealTimeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CChartRealTimeView::CChartRealTimeView() : CChartViewer()  {
  m_nChartWidth = 600;
  m_nChartHeight = 180;
  m_refresh.OnRefresh.Add( MakeDelegate( this, &CChartRealTimeView::HandlePeriodicRefresh ) );
}

CChartRealTimeView::~CChartRealTimeView(void) {
  m_refresh.OnRefresh.Remove( MakeDelegate( this, &CChartRealTimeView::HandlePeriodicRefresh ) );
}

BEGIN_MESSAGE_MAP(CChartRealTimeView, CChartViewer)
END_MESSAGE_MAP()

void CChartRealTimeView::SetChartDimensions(unsigned int x, unsigned int y) {
  // need to force a window resize here
  m_nChartWidth = x;
  m_nChartHeight = y;
}

void CChartRealTimeView::HandleModelChanged(const CChartRealTimeModel &model) {
}

void CChartRealTimeView::HandlePeriodicRefresh( CGeneratePeriodicRefresh *pMsg ){
  // redraw the chart here
}

