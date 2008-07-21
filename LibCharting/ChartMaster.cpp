#include "StdAfx.h"
#include "ChartMaster.h"

#include "Color.h"

#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CChartMaster::CChartMaster(void) 
: CChartViewer(), m_bUpdateChart( false ), m_pCdv( NULL),
  m_nChartWidth( 600 ), m_nChartHeight( 900 )
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
  //if ( m_bUpdateChart ) {  // no way to set this flag, needs to be set in container or elsewhere

    struct structSubChart {
      XYChart *xy; // xy chart at this position
      structSubChart( void ) : xy( NULL ) {};
    };

    if ( NULL != m_pCdv ) { // DataView has something to draw
      //MultiChart multi( m_nChartWidth, m_nChartHeight, Chart::goldColor );
      MultiChart multi( m_nChartWidth, m_nChartHeight );

      // chart 0 is x, volume chart is 1/4x, indicator charts are 1/3x
      // calc assumes chart 0 and chart 1 are always present
      size_t n = m_pCdv->GetChartCount();
      int heightChart0 = ( 12 * ( m_nChartHeight - 100 ) ) / ( 15 + ( 4 * ( n - 2 ) ) );
      int heightChart1 = heightChart0 / 4;
      int heightChartN = heightChart0 / 3;

      std::vector<structSubChart> vCharts;
      vCharts.resize( n );  // this is the number of sub-charts we are working with (move to class def so not redone all the time?)
      size_t ix = 0;
      int y = 25;
      int x = 50;
      int xAxisHeight = 50;
      XYChart *pXY;
      XYChart *pXY0;
      while ( ix < n ) {
        switch ( ix ) {
          case 0:
            pXY0 = pXY = new XYChart( m_nChartWidth, heightChart0 );
            pXY->setPlotArea( x, xAxisHeight, m_nChartWidth - 2 * x, heightChart0 - xAxisHeight );
            pXY->setXAxisOnTop( true );
            multi.addChart( 0, y, pXY );
            multi.setMainChart( pXY );
            y += heightChart0; 
            break;
          case 1:
            pXY = new XYChart( m_nChartWidth, heightChart1 );
            pXY->setPlotArea( x, 0, m_nChartWidth - 2 * x, heightChart1 - 50 );
            pXY->xAxis()->setColors(Chart::LineColor, Chart::Transparent);
            pXY->xAxis()->copyAxis( pXY0->xAxis() );
            multi.addChart( 0, y, pXY );
            y += heightChart1;
            break;
          default:
            pXY = new XYChart( m_nChartWidth, heightChartN );
            pXY->setPlotArea( x, 0, m_nChartWidth - 2 * x, heightChartN );
            pXY->xAxis()->setColors(Chart::LineColor, Chart::Transparent);
            pXY->xAxis()->copyAxis( pXY0->xAxis() );
            multi.addChart( 0, y, pXY );
            y += heightChartN;
            break;
        }
        vCharts[ ix ].xy = pXY;
        ++ix;
      }

      for ( CChartDataView::iterator iter = m_pCdv->begin(); m_pCdv->end() != iter; ++iter ) {
        size_t ixChart = (*iter).GetActualChartId();
        (*iter).GetChartEntry()->AddDataToChart( vCharts[ ixChart ].xy );
      }
      multi.layout();
      setChart( &multi );

      for ( std::vector<structSubChart>::iterator iter = vCharts.begin(); iter < vCharts.end(); ++iter ) {
        delete (*iter).xy;
      }
    }

    m_bUpdateChart = false;
  //}
}

