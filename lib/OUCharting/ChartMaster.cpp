/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <vector>

//#include <OUCommon/Colour.h>

#include "ChartMaster.h"

namespace ou { // One Unified

  ChartMaster::ChartMaster(void) 
: m_pCdv( NULL),
  m_nChartWidth( 600 ), m_nChartHeight( 900 ),
  m_dblMinDuration( 60 * 1 ), m_dblCurDuration( 60 * 1 ), // 1 minute width, 1 minute width
  m_dblXMin( 0 ), m_dblXMax( 0 ),
  m_bCreated( false )
//  m_dtViewPortBegin( boost::posix_time::not_a_date_time ), m_dtViewPortEnd( boost::posix_time::not_a_date_time )
{
  Initialize();
}

ChartMaster::ChartMaster( unsigned int width, unsigned int height ) 
: m_pCdv( NULL),
  m_nChartWidth( width ), m_nChartHeight( height ),
  m_dblMinDuration( 60 * 1 ), m_dblCurDuration( 60 * 1 ), // 1 minute width, 1 minute width
  m_dblXMin( 0 ), m_dblXMax( 0 ),
  m_bCreated( false )
//  m_dtViewPortBegin( boost::posix_time::not_a_date_time ), m_dtViewPortEnd( boost::posix_time::not_a_date_time )
{
  Initialize();
}

ChartMaster::~ChartMaster(void) {
}

void ChartMaster::Initialize( void ) {
  bool b = Chart::setLicenseCode( "DEVP-2G22-4QPN-HDS6-925A-95C1" );
  assert( b );
}

void ChartMaster::SetChartDimensions(unsigned int width, unsigned int height) {
  m_nChartWidth = width;
  m_nChartHeight = height;
  if ( NULL != m_pCdv ) m_pCdv->SetChanged();
}

//void ChartMaster::SetViewPort( boost::posix_time::ptime dtBegin, boost::posix_time::ptime dtEnd ) {
//  m_dtViewPortBegin = dtBegin;
//  m_dtViewPortEnd = dtEnd;
//}

void ChartMaster::SetBarWidth( boost::posix_time::time_duration tdBarWidth ) {
  m_tdBarWidth = tdBarWidth;
  //m_pCdv-
}

void ChartMaster::DrawChart( bool bViewPortChanged ) {

  struct structSubChart {
    XYChart *xy; // xy chart at this position
    structSubChart( void ) : xy( 0 ) {};
  };

  if ( NULL != m_pCdv ) { // DataView has something to draw
    //if ( m_pCdv->GetChanged() ) {
    if ( true ) {
      //MultiChart multi( m_nChartWidth, m_nChartHeight, Chart::goldColor );
      MultiChart multi( m_nChartWidth, m_nChartHeight );

      std::string sTitle( m_pCdv->GetName() + " - " + m_pCdv->GetStrategy() );
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
            multi.addChart( 0, y, pXY );
            y += heightChartN;
            break;
        }
        vCharts[ ix ].xy = pXY;
        ++ix;
      }

      // determine XAxis min/max while adding chart data
      m_dblXMin = 0;
      m_dblXMax = 0;
      for ( ChartDataView::iterator iter = m_pCdv->begin(); m_pCdv->end() != iter; ++iter ) {
        size_t ixChart = iter->GetActualChartId();
        ChartEntryBase::structChartAttributes Attributes;
        iter->GetChartEntry()->AddEntryToChart( vCharts[ ixChart ].xy, &Attributes );
        m_dblXMin = ( 0 == m_dblXMin ) ? Attributes.dblXMin : std::min<double>( m_dblXMin, Attributes.dblXMin );
        m_dblXMax = ( 0 == m_dblXMax ) ? Attributes.dblXMax : std::max<double>( m_dblXMax, Attributes.dblXMax );
      }

      // time axis scales
      double dblLower;
      double dblUpper;
      if ( m_dblXMin != m_dblXMax ) {
//*        if ( ( m_dblXMax - m_dblXMin ) < m_dblMinDuration ) {  // minimum time window
          dblUpper = m_dblXMax;
          dblLower = dblUpper - m_dblMinDuration;
//*        }
//*        else {
          // fracional viewport calculation, for when mfc was used
//*          dblLower = m_dblXMin + (m_dblXMax - m_dblXMin) *  this->getViewPortLeft();
//*          dblUpper = m_dblXMin + (m_dblXMax - m_dblXMin) * (this->getViewPortLeft() + this->getViewPortWidth());
          //dblUpper = m_dblXMax;
          //dblLower = m_dblXMin;
//*        }
//        pXY0->xAxis()->setDateScale( dblLower, dblUpper, 0, 0 );
      }

//*      setChart( &multi );
      MemBlock m = multi.makeChart( BMP );
      if ( 0 != m_OnDrawChart ) m_OnDrawChart( m );

      for ( std::vector<structSubChart>::iterator iter = vCharts.begin(); iter < vCharts.end(); ++iter ) {
        delete (*iter).xy;
      }
    }
  }
}

} // namespace ou
