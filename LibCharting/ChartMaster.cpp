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
  if ( m_bUpdateChart ) {

    struct structSubChart {
      XYChart *xy; // xy chart at this position
      structSubChart( void ) : xy( NULL ) {};
    };

    if ( NULL != m_pCdv ) { // DataView has something to draw
      //MultiChart multi( m_nChartWidth, m_nChartHeight, Chart::goldColor );
      MultiChart multi( m_nChartWidth, m_nChartHeight );

      // chart 0 is x, volume chart is 1/4x, indicator charts are 1/3x
      // calc assumes chart 0 and chart 1 are always present
      int heightChart0 = m_nChartHeight / ( ( 15 + ( 4 * m_pCdv->GetChartCount() ) ) / 12 );
      int heightChart1 = heightChart0 / 4;
      int heightChartN = heightChart0 / 3;

      std::vector<structSubChart> vCharts;
      vCharts.resize( m_pCdv->GetChartCount() );  // this is the number of sub-charts we are working with
      size_t ix = 0;
      int y = 0;
      XYChart *pXY;
      while ( ix < m_pCdv->GetChartCount() ) {
        switch ( ix ) {
          case 0:
            pXY = new XYChart( m_nChartWidth, heightChart0 );
            multi.addChart( 0, y, pXY );
            multi.setMainChart( pXY );
            y += heightChart0; 
            break;
          case 1:
            pXY = new XYChart( m_nChartWidth, heightChart1 );
            multi.addChart( 0, y, pXY );
            y += heightChart1;
            break;
          default:
            pXY = new XYChart( m_nChartWidth, heightChartN );
            multi.addChart( 0, y, pXY );
            y += heightChartN;
            break;
        }
        vCharts[ ix ].xy = pXY;
        //pXY->setPlotArea( 50, 20, m_nChartWidth - 75, m_nChartHeight - 75 );
        ++ix;
      }

      for ( CChartDataView::iterator iter = m_pCdv->begin(); m_pCdv->end() != iter; ++iter ) {
        size_t ixChart = (*iter).GetActualChartId();
        (*iter).GetChartEntry()->AddDataToChart( vCharts[ ix ].xy );
      }
      multi.layout();
      setChart( &multi );
    }

    m_bUpdateChart = false;
  }
}

