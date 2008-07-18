#include "StdAfx.h"
#include "ChartMaster.h"

#include "Color.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CChartMaster::CChartMaster(void) 
: CChartViewer(), m_bUpdateChart( false ), m_pCdv( NULL),
  m_nChartWidth( 600 ), m_nChartHeight( 180 )
{
  m_refresh.OnRefresh.Add( MakeDelegate( this, &CChartMaster::HandlePeriodicRefresh ) );
}

CChartMaster::~CChartMaster(void) {
  m_refresh.OnRefresh.Remove( MakeDelegate( this, &CChartMaster::HandlePeriodicRefresh ) );
}

BEGIN_MESSAGE_MAP(CChartMaster, CChartViewer)
END_MESSAGE_MAP()

void CChartMaster::SetChartDimensions(unsigned int width, unsigned int height) {
  // need to force a window resize here
  m_nChartWidth = width;
  m_nChartHeight = height;
  m_bUpdateChart = true;
}

void CChartMaster::HandlePeriodicRefresh( CGeneratePeriodicRefresh *pMsg ){
  if ( m_bUpdateChart ) {
    // redraw the chart here
    /*
    FinanceChart chart( m_nChartWidth );
    //assert( NULL != m_pModel );
    XYChart *xy = chart.addMainChart( 300 ); 
    //LineLayer *lltrade = chart.addLineIndicator2( xy, m_pModel->Trades()->GetPrice(), Green, _T( "Trade" ) );
    //XYChart *xyask = chart.addLineIndicator( 100, m_pModel->Asks()->GetPrice(), Red, _T( "Ask" ) );
    LineLayer *lltrade = xy->addLineLayer( m_pModel->Asks()->GetPrice(), Green, _T( "Trade" ) );
    //LineLayer *llask = xy->addLineLayer( m_pModel->Asks()->GetPrice(), Red, _T( "Ask" ) );
    lltrade->setXData( m_pModel->Trades()->GetDateTime() );
    //LineLayer *llbid = chart.addLineIndicator2( xy, m_pModel->Bids()->GetPrice(), Blue, _T( "Bid" ) );
//    llbid->setXData( m_pModel->Bids()->GetDateTime() );
    CChartEntryBars *bars = m_pModel->Bars();
    chart.setData( 
      bars->GetDateTime(), 
      bars->GetHigh(), 
      bars->GetLow(), 
      bars->GetOpen(), 
      bars->GetClose(), 
      bars->GetVolume(), 
      0 );
    //chart.layout();
    chart.addCandleStick(0x00ff00, 0xff0000);
    chart.addVolBars( 70, 0x99ff99, 0xff9999, 0x808080);*/


    //MultiChart multi( m_nChartWidth, m_nChartHeight, Chart::goldColor );
    
    XYChart xy( m_nChartWidth, m_nChartHeight );
    //multi.addChart( 0, 0, &xy );
    xy.setPlotArea( 50, 20, m_nChartWidth - 75, m_nChartHeight - 75 );
/*    
    CChartEntryBars *bars = m_pModel->Bars();
    if ( 0 < bars->Size() ) {
      BarLayer *bl = xy.addBarLayer( bars->GetVolume() );
      bl->setXData( bars->GetDateTime() );
      bl->setUseYAxis2( true );

      LineLayer *lltrade = xy.addLineLayer( m_pModel->Trades()->GetPrice(), Green, _T( "Trade" ) );
      lltrade->setXData( m_pModel->Trades()->GetDateTime() );
      LineLayer *llasks = xy.addLineLayer( m_pModel->Asks()->GetPrice(), Red, _T( "Ask" ) );
      llasks->setXData( m_pModel->Asks()->GetDateTime() );
      LineLayer *llbids = xy.addLineLayer( m_pModel->Bids()->GetPrice(), Blue, _T( "Bid" ) );
      llbids->setXData( m_pModel->Bids()->GetDateTime() );

      CandleStickLayer *candle = xy.addCandleStickLayer( 
        bars->GetHigh(), 
        bars->GetLow(), 
        bars->GetOpen(), 
        bars->GetClose(), 0x00ff00, 0xff0000);
      candle->setXData( bars->GetDateTime() );
    }
*/

    xy.layout();
    //multi.layout();
    setChart( &xy );
    //setChart( &multi );
    m_bUpdateChart = false;
  }
}

