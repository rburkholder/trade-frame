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


#include "ChartMaster.h"
#include "ChartDataView.h"

namespace ou { // One Unified

ChartMaster::ChartMaster( unsigned int width, unsigned int height )
: m_pCdv( nullptr), m_pDA( nullptr )
, m_nChartWidth( width ), m_nChartHeight( height )
, m_intCrossHairX {}, m_intCrossHairY {}, m_bCrossHair( false )
, m_bHasData( false )
, m_dblX {}, m_dblY {}
, m_xLeft {}, m_xX {}, m_xRight {}
, m_formatter( "%.2f"  )
, m_nChart {}
{
  m_szCursorTime[ 0 ] = 0;
  Initialize();
}

ChartMaster::ChartMaster(): ChartMaster( 600, 900 ) {}

ChartMaster::~ChartMaster() {
}

void ChartMaster::Initialize() {
  // NOTE license is now found in /etc/chartdir.lic
}

void ChartMaster::SetChartDataView( ChartDataView* pcdv ) {

  //ResetDynamicLayer();

  m_pCdv = pcdv;

  m_pChart = std::make_unique<MultiChart>( m_nChartWidth, m_nChartHeight );
  ChartStructure();

  //if ( nullptr != m_pCdv ) m_pCdv->SetChanged();
};

void ChartMaster::SetChartDimensions(unsigned int width, unsigned int height) {

  //ResetDynamicLayer();

  m_nChartWidth = width;
  m_nChartHeight = height;

  //MultiChart multi( m_nChartWidth, m_nChartHeight, Chart::goldColor );
  m_pChart = std::make_unique<MultiChart>( m_nChartWidth, m_nChartHeight );
  ChartStructure();

  //if ( nullptr != m_pCdv ) m_pCdv->SetChanged();
}

/*
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
*/

void ChartMaster::SetBarWidth( boost::posix_time::time_duration tdBarWidth ) {
  m_tdBarWidth = tdBarWidth;
}

void ChartMaster::ChartStructure() {

  std::string sTitle( m_pCdv->GetName() + " - " + m_pCdv->GetDescription() );
  m_pChart->addTitle( sTitle.c_str() );

  // chart 0 (main chart) is x, chrt 1 (volume chart) is 1/4x, ChartN (indicator charts) are 1/3x
  // calc assumes chart 0 and chart 1 are always present
  size_t n = m_pCdv->GetChartCount();
  int heightChart0 = ( 12 * ( m_nChartHeight - 25 ) ) / ( 15 + ( 4 * ( n - 2 ) ) );
  int heightChart1 = heightChart0 / 4;
  int heightChartN = heightChart0 / 3;

  m_vSubCharts.resize( n );  // this is the number of sub-charts we are working with (move to class def so not redone all the time?)

  size_t ix = 0;
  int y = 15;  // was 25
  int x = 50;
  int xAxisHeight = 50;
  XYChart* pXY;  // used for each sub-chart

  while ( ix < n ) {
    switch ( ix ) {
      case 0:  // main chart
        m_pXY0 = pXY = new XYChart( m_nChartWidth, heightChart0 );
        pXY->setPlotArea( x, xAxisHeight, m_nChartWidth - 2 * x, heightChart0 - xAxisHeight )->setGridColor(0xc0c0c0, 0xc0c0c0);
        pXY->xAxis()->setColors(Chart::LineColor, Chart::LineColor);
        pXY->setClipping();
        pXY->setXAxisOnTop( true );
        pXY->xAxis()->setWidth( 2 );
        pXY->yAxis()->setWidth( 2 );
        pXY->yAxis()->setMargin( 2, 5 );
        pXY->addLegend( x, xAxisHeight, true, 0, 9.0 );
        m_pChart->addChart( 0, y, pXY );
        m_pChart->setMainChart( pXY );
        y += heightChart0;
        break;
      case 1: // volume chart
        pXY = new XYChart( m_nChartWidth, heightChart1 );
        pXY->setPlotArea( x, 0, m_nChartWidth - 2 * x, heightChart1 )->setGridColor(0xc0c0c0, 0xc0c0c0);
        pXY->setClipping();
        //pXY->xAxis()->setColors(Chart::LineColor, Chart::Transparent);  // turn off axis
        pXY->xAxis()->setColors(Chart::LineColor, Chart::LineColor);
        pXY->xAxis()->copyAxis( m_pXY0->xAxis() ); // use settings from main subchart
        pXY->xAxis()->setWidth( 2 );
        pXY->yAxis()->setWidth( 2 );
        pXY->yAxis()->setMargin( 5, 5 );
        pXY->addLegend( x, 0, true, 0, 9.0 );
        m_pChart->addChart( 0, y, pXY );
        y += heightChart1;
        break;
      default:  // secondary indicator charts
        pXY = new XYChart( m_nChartWidth, heightChartN );
        pXY->setPlotArea( x, 0, m_nChartWidth - 2 * x, heightChartN )->setGridColor(0xc0c0c0, 0xc0c0c0);
        pXY->setClipping();
        //pXY->xAxis()->setColors(Chart::LineColor, Chart::Transparent);  // turn off axis
        pXY->xAxis()->setColors(Chart::LineColor, Chart::LineColor);
        pXY->xAxis()->copyAxis( m_pXY0->xAxis() ); // use settings from main subchart
        pXY->xAxis()->setWidth( 2 );
        pXY->yAxis()->setWidth( 2 );
        pXY->yAxis()->setMargin( 5, 5 );
        pXY->addLegend( x, 0, true, 0, 9.0 );
        m_pChart->addChart( 0, y, pXY );
        y += heightChartN;
        break;
    }
    m_vSubCharts[ ix ] = std::unique_ptr<XYChart>( pXY );
    ++ix;
  }
}

void ChartMaster::ChartData( XYChart* pXY0 ) {

  using vfPostLayout_t = std::vector<ChartEntryBase::structChartAttributes::fPostLayout_t>;
  vfPostLayout_t vfPostLayout;

  // determine XAxis min/max while adding chart data
  double dblXBegin {};
  double dblXEnd {};

  m_pCdv->EachChartEntryCarrier(
    [this,&dblXBegin,&dblXEnd,&vfPostLayout]( ou::ChartEntryCarrier& carrier ){
      size_t ixChart = carrier.GetActualChartId();
      ChartEntryBase::structChartAttributes attributes;
      if ( carrier.GetChartEntry()->AddEntryToChart( m_vSubCharts[ ixChart ].get(), attributes ) ) {
        //std::cout << "  attribute " << ixChart << ',' << attributes.dblXMin << ',' << attributes.dblXMax << std::endl;
        dblXBegin
          = ( 0 == dblXBegin )
          ? attributes.dblXMin
          : std::min<double>( dblXBegin, attributes.dblXMin );
        dblXEnd
          = ( 0 == dblXEnd )
          ? attributes.dblXMax
          : std::max<double>( dblXEnd,   attributes.dblXMax );
        if ( nullptr != attributes.fPostLayout ) {
          vfPostLayout.emplace_back( std::move( attributes.fPostLayout ) );
        }
      }
    } );

  // time axis scales
  if ( dblXBegin != dblXEnd ) {
    pXY0->xAxis()->setDateScale( dblXBegin, dblXEnd, 0, 0 );
    m_bHasData = true;
    //std::cout << "setDateScale: " << dblXBegin << " - " << dblXEnd << std::endl;
    if ( 0 < vfPostLayout.size() ) {
      pXY0->layoutAxes();
      for ( auto& fPostLayout: vfPostLayout ) {
        fPostLayout();
      }
    }
  }
  else {
    m_bHasData = false;
    //std::cout << "Time Scales match" << std::endl;
  }

}

void ChartMaster::DrawChart() {

  if ( m_pChart ) {
    if ( m_pCdv ) { // DataView has something to draw

      ChartData( m_pXY0 );
      RenderChart();   // PROBLEM in lock 3

    }
  }
}

void ChartMaster::RenderChart() {
  if ( m_bHasData ) {  // Did you 'm_pWinChartView->SetSim( true );'?
    bool bCursor( true );
    if ( m_bCrossHair ) {
      bCursor = DrawDynamicLayer();
    }
    MemBlock m = m_pChart->makeChart( Chart::BMP );   // PROBLEM in lock 4
    if ( m_fOnDrawChart ) m_fOnDrawChart( bCursor, m );
  }
}

bool ChartMaster::ResetDynamicLayer() {
  bool bExisted { false };
  if ( nullptr != m_pDA ) {
    bExisted = true;
    m_pChart->removeDynamicLayer();
    m_pDA = nullptr;
  }
  return bExisted;
}

void ChartMaster::SetCrossHairPosition( int x, int y ) {
  m_intCrossHairX = x;
  m_intCrossHairY = y;
}

void ChartMaster::SetCrossHairTime( const std::string& sTime ) {
  auto size = sTime.size();
  memcpy( m_szCursorTime, sTime.c_str(), size );
  m_szCursorTime[ size ] = 0;
}

void ChartMaster::CrossHairDraw( bool bDraw ) {
  m_bCrossHair = bDraw;
}

bool ChartMaster::DrawDynamicLayer() {
  // TODO: maybe trigger more often than data update happens?
  //   trigger a call back for sync?
  //   confirm when hit, only when mouse is over?

  bool bCrossHairs( false );

  if ( m_pChart ) {

    m_pDA = m_pChart->initDynamicLayer(); // new/clear

    // this however, does work, not like the one below
    //std::stringstream ss;
    //ss << "x=" << m_intCrossHairX << ",y=" << m_intCrossHairY;
    //m_pDA->text( ss.str().c_str(), "normal", 10, 10, 10, Colour::Black );

    const int n = m_pChart->getChartCount();
    assert( 0 < n );

    BaseChart* pSubBaseChart;
    int top {}, bottom {};

    XYChart* pChartFocus( nullptr );

    for ( int ix = 0; ix < n; ix++ ) {

      pSubBaseChart = m_pChart->getChart( ix );
      XYChart* pSubChart = dynamic_cast<XYChart*>( pSubBaseChart );
      PlotArea* pArea = pSubChart->getPlotArea();

      int pxChartTop = pSubChart->getAbsOffsetY() + pArea->getTopY();

      if ( 0 == ix ) {
        auto AbsOffsetX = pSubChart->getAbsOffsetX(); // offset to left side of chart
        // set based upon top chart:
        top      = pxChartTop;
        m_xLeft  = AbsOffsetX + pArea->getLeftX();
        m_xRight = AbsOffsetX + pArea->getRightX();
      }

      bottom = pSubChart->getAbsOffsetY() + pArea->getBottomY();

      if ( pxChartTop < m_intCrossHairY ) {
        pChartFocus = pSubChart;
        m_nChart = ix;
      }

    } // end for ix

    if (
      ( top < m_intCrossHairY  ) &&
      ( bottom > m_intCrossHairY ) &&
      ( m_xLeft < m_intCrossHairX ) &&
      ( m_xRight > m_intCrossHairX )
    ) {

      bCrossHairs = true;

      assert( nullptr != pChartFocus );
      m_xX = m_intCrossHairX - pChartFocus->getAbsOffsetX();
      m_dblX = pChartFocus->getXValue( m_xX );
      m_dblY = pChartFocus->getYValue( m_intCrossHairY - pChartFocus->getAbsOffsetY() );
      const std::string sValue = ( m_formatter % m_dblY ).str();

      // chartdir does not like taking a c_str() of anything, so need to perform a copy
      char sz[100];
      memcpy( sz, sValue.c_str(), sValue.size() );
      sz[ sValue.size() ] = 0;

      m_pDA->hline( m_xLeft, m_xRight, m_intCrossHairY, Colour::Gray );
      m_pDA->vline( top, bottom, m_intCrossHairX, Colour::Gray );
      m_pDA->text( sz, "normal", 10, m_intCrossHairX + 1, m_intCrossHairY - 14, Colour::Black );

      if ( 0 != m_szCursorTime[ 0 ] ) {
        m_pDA->text( m_szCursorTime, "normal", 10, m_intCrossHairX + 1, m_intCrossHairY + 2, Colour::Black );
      }

    }

  }
  return !bCrossHairs; // caller shows cursor when no crosshairs
}

} // namespace ou
