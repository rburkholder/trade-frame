#include "StdAfx.h"
#include "ChartRealTimeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CChartRealTimeView::CChartRealTimeView( CChartRealTimeModel *pModel ) 
: CChartViewer(), m_bUpdateChart( false ), m_bModelChanged( false ),
  m_nChartWidth( 600 ), m_nChartHeight( 180 ),
  m_pModel( pModel )
{
  m_pModel->OnBarComplete.Add( MakeDelegate( this, &CChartRealTimeView::HandleBarCompleted ) );
  m_refresh.OnRefresh.Add( MakeDelegate( this, &CChartRealTimeView::HandlePeriodicRefresh ) );
}

CChartRealTimeView::~CChartRealTimeView(void) {
  m_refresh.OnRefresh.Remove( MakeDelegate( this, &CChartRealTimeView::HandlePeriodicRefresh ) );
  m_pModel->OnBarComplete.Remove( MakeDelegate( this, &CChartRealTimeView::HandleBarCompleted ) );
}

BEGIN_MESSAGE_MAP(CChartRealTimeView, CChartViewer)
END_MESSAGE_MAP()

void CChartRealTimeView::SetChartDimensions(unsigned int x, unsigned int y) {
  // need to force a window resize here
  m_nChartWidth = x;
  m_nChartHeight = y;
}

void CChartRealTimeView::HandleBarCompleted(CChartRealTimeModel *model) {
  m_bModelChanged = true;
}

void CChartRealTimeView::HandlePeriodicRefresh( CGeneratePeriodicRefresh *pMsg ){
  if ( m_bModelChanged ) {
    // redraw the chart here
  CRect rect;
  CChartViewer::GetClientRect( &rect );
  m_nChartWidth = rect.Width();
  m_nChartHeight = rect.Height();
    FinanceChart chart( m_nChartWidth );
    //assert( NULL != m_pModel );
    CChartEntryBars *bars = m_pModel->Bars();
    chart.setData( 
      bars->GetDateTime(), 
      bars->GetHigh(), 
      bars->GetLow(), 
      bars->GetOpen(), 
      bars->GetClose(), 
      bars->GetVolume(), 
      0 );
    XYChart *xy = chart.addMainChart( m_nChartHeight );
    chart.addCandleStick(0x00ff00, 0xff0000);
    chart.layout();
    setChart( &chart );
    m_bModelChanged = false;
  }
}

