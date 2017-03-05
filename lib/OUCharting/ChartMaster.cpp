
//#include "StdAfx.h"

#include <vector>

//#include <OUCommon/Colour.h>

#include "ChartMaster.h"

namespace ou { // One Unified

  ChartMaster::ChartMaster(void) 
: m_pCdv( NULL),
  m_nChartWidth( 600 ), m_nChartHeight( 900 ),
  m_dblViewPortXBegin( 0 ), m_dblViewPortXEnd( 0 ),
  m_bCreated( false )
{
  Initialize();
}

ChartMaster::ChartMaster( unsigned int width, unsigned int height ) 
: m_pCdv( NULL),
  m_nChartWidth( width ), m_nChartHeight( height ),
  m_dblViewPortXBegin( 0 ), m_dblViewPortXEnd( 0 ),
  m_bCreated( false )
{
  Initialize();
}

ChartMaster::~ChartMaster(void) {
}

void ChartMaster::Initialize( void ) {
  char code[2048];
  //bool b = Chart::setLicenseCode( "DEVP-2G22-4QPN-HDS6-925A-95C1", &code );
  //bool b = Chart::setLicenseCode( "UDEV-23FP-5DWS-X22U-BBBD-FBD8", &code );
  bool b = Chart::setLicenseCode( "DEVP-2G22-4QPN-HDS6-925A-95C1 UDEV-23FP-5DWS-X22U-BBBD-FBD8", code );
  //Chart::getLicenseAttr()
  assert( b );
}

void ChartMaster::SetChartDimensions(unsigned int width, unsigned int height) {
  m_nChartWidth = width;
  m_nChartHeight = height;
  if ( NULL != m_pCdv ) m_pCdv->SetChanged();
}

void ChartMaster::SetViewPort( boost::posix_time::ptime dtBegin, boost::posix_time::ptime dtEnd ) {
  m_dblViewPortXBegin = 
    ( boost::posix_time::not_a_date_time != dtBegin ) 
    ? 
    Chart::chartTime( 
      dtBegin.date().year(), dtBegin.date().month(), dtBegin.date().day(),
      dtBegin.time_of_day().hours(), dtBegin.time_of_day().minutes(), dtBegin.time_of_day().seconds() )
    : 
    0;
  m_dblViewPortXEnd = 
    ( boost::posix_time::not_a_date_time != dtEnd ) 
    ?
    Chart::chartTime( 
      dtEnd.date().year(), dtEnd.date().month(), dtEnd.date().day(),
      dtEnd.time_of_day().hours(), dtEnd.time_of_day().minutes(), dtEnd.time_of_day().seconds() )
    : 
    0;
}

void ChartMaster::SetBarWidth( boost::posix_time::time_duration tdBarWidth ) {
  m_tdBarWidth = tdBarWidth;
  //m_pCdv-
}

void ChartMaster::DrawChart( bool bViewPortChanged ) {

  struct structSubChart {
    XYChart* xy; // xy chart at this position
    structSubChart( void ) : xy( 0 ) {};
  };

  if ( NULL != m_pCdv ) { // DataView has something to draw
    //if ( m_pCdv->GetChanged() ) {
    if ( true ) {
      //MultiChart multi( m_nChartWidth, m_nChartHeight, Chart::goldColor );
      MultiChart multi( m_nChartWidth, m_nChartHeight );
      
      std::string sTitle( m_pCdv->GetStrategy() + " - " + m_pCdv->GetName() );
      multi.addTitle( sTitle.c_str() );

      // chart 0 (main chart) is x, chrt 1 (volume chart) is 1/4x, ChartN (indicator charts) are 1/3x
      // calc assumes chart 0 and chart 1 are always present
      size_t n = m_pCdv->GetChartCount();
      int heightChart0 = ( 12 * ( m_nChartHeight - 25 ) ) / ( 15 + ( 4 * ( n - 2 ) ) );
      int heightChart1 = heightChart0 / 4;
      int heightChartN = heightChart0 / 3;

      std::vector<structSubChart> vCharts;
      vCharts.resize( n );  // this is the number of sub-charts we are working with (move to class def so not redone all the time?)

      size_t ix = 0;
      int y = 15;  // was 25
      int x = 50;
      int xAxisHeight = 50;
      XYChart *pXY;  // used for each sub-chart
      XYChart *pXY0;  // main chart
      while ( ix < n ) {
        switch ( ix ) {
          case 0:  // main chart
            pXY0 = pXY = new XYChart( m_nChartWidth, heightChart0 );
            pXY->setPlotArea( x, xAxisHeight, m_nChartWidth - 2 * x, heightChart0 - xAxisHeight )->setGridColor(0xc0c0c0, 0xc0c0c0);
            pXY->xAxis()->setColors(Chart::LineColor, Chart::LineColor); 
            pXY->setClipping();
            pXY->setXAxisOnTop( true );
            pXY->xAxis()->setWidth( 2 );
            pXY->yAxis()->setWidth( 2 );
            pXY->yAxis()->setMargin( 2, 5 );
            pXY->addLegend( x, xAxisHeight, true, 0, 9.0 );
            multi.addChart( 0, y, pXY );
            multi.setMainChart( pXY );
            y += heightChart0; 
            break;
          case 1: // volume chart
            pXY = new XYChart( m_nChartWidth, heightChart1 );
            pXY->setPlotArea( x, 0, m_nChartWidth - 2 * x, heightChart1 )->setGridColor(0xc0c0c0, 0xc0c0c0);
            pXY->setClipping();
            //pXY->xAxis()->setColors(Chart::LineColor, Chart::Transparent);  // turn off axis
            pXY->xAxis()->setColors(Chart::LineColor, Chart::LineColor); 
            pXY->xAxis()->copyAxis( pXY0->xAxis() ); // use settings from main subchart
            pXY->xAxis()->setWidth( 2 );
            pXY->yAxis()->setWidth( 2 );
            pXY->yAxis()->setMargin( 5, 5 );
            pXY->addLegend( x, 0, true, 0, 9.0 );
            multi.addChart( 0, y, pXY );
            y += heightChart1;
            break;
          default:  // secondary indicator charts
            pXY = new XYChart( m_nChartWidth, heightChartN );
            pXY->setPlotArea( x, 0, m_nChartWidth - 2 * x, heightChartN )->setGridColor(0xc0c0c0, 0xc0c0c0);
            pXY->setClipping();
            //pXY->xAxis()->setColors(Chart::LineColor, Chart::Transparent);  // turn off axis
            pXY->xAxis()->setColors(Chart::LineColor, Chart::LineColor); 
            pXY->xAxis()->copyAxis( pXY0->xAxis() ); // use settings from main subchart
            pXY->xAxis()->setWidth( 2 );
            pXY->yAxis()->setWidth( 2 );
            pXY->yAxis()->setMargin( 5, 5 );
            pXY->addLegend( x, 0, true, 0, 9.0 );
            multi.addChart( 0, y, pXY );
            y += heightChartN;
            break;
        }
        vCharts[ ix ].xy = pXY;
        ++ix;
      }

      // determine XAxis min/max while adding chart data
      double dblXBegin = m_dblViewPortXBegin;
      double dblXEnd = m_dblViewPortXEnd;
      for ( ChartDataView::iterator iter = m_pCdv->begin(); m_pCdv->end() != iter; ++iter ) {
        size_t ixChart = iter->GetActualChartId();
        ChartEntryBase::structChartAttributes Attributes;
        if ( iter->GetChartEntry()->AddEntryToChart( vCharts[ ixChart ].xy, &Attributes ) ) {
          // following assumes values are always > 0
          if( 0 == m_dblViewPortXBegin ) {
            dblXBegin = ( 0 == dblXBegin ) 
              ? Attributes.dblXMin 
              : std::min<double>( dblXBegin, Attributes.dblXMin );
          }
          if( 0 == m_dblViewPortXEnd ) {
            dblXEnd   = ( 0 == dblXEnd   ) 
              ? Attributes.dblXMax 
              : std::max<double>( dblXEnd,   Attributes.dblXMax );
          }
//          if ( 610.0 < ( dblXEnd - dblXBegin ) ) {
//            double dif = dblXEnd - dblXBegin;
//            static double change( 0 );
//            if ( dif > change ) {
//              change = dif;
              //std::cout << "diff " << dif << std::endl;
//            }
//          }
        }
      }

      // time axis scales
      if ( dblXBegin != dblXEnd ) {
        pXY0->xAxis()->setDateScale( dblXBegin, dblXEnd, 0, 0 );
      }
      else {
        //std::cout << "Time Scales match" << std::endl;
      }

      MemBlock m = multi.makeChart( BMP );
      if ( 0 != m_OnDrawChart ) m_OnDrawChart( m );

      for ( std::vector<structSubChart>::iterator iter = vCharts.begin(); iter < vCharts.end(); ++iter ) {
        delete (*iter).xy;
      }
    }
  }
}

} // namespace ou
