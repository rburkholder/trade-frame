#include "StdAfx.h"
#include "ChartDatedDatum.h"
//#include "chartdir.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// recieve quotes and trades, maintain window of values, and re-chart the window
// as new values are received.
// generate bars 

ChartDatedDatum::ChartDatedDatum(void) : CChartViewerShim() {
  m_pWindowBars = new SlidingWindowBars();
  m_bUpdateChart = true;
  chart = NULL;
  m_majorTickInc = 0;
  m_minorTickInc = 0;
  m_factory.SetOnBarComplete( MakeDelegate( this, &ChartDatedDatum::HandleOnNewBar ) );
  m_factory.SetOnBarUpdated( MakeDelegate( this, &ChartDatedDatum::HandleOnBarUpdated ) );
}

ChartDatedDatum::~ChartDatedDatum(void) {
}

BEGIN_MESSAGE_MAP(ChartDatedDatum, CChartViewerShim)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void ChartDatedDatum::OnDestroy()  {

  if ( NULL != chart ) delete chart;
  delete m_pWindowBars;

	CChartViewerShim::OnDestroy();
}

void ChartDatedDatum::SetWindowWidthSeconds( long seconds ) {
  m_pWindowBars -> SetSlidingWindowSeconds( seconds );
}

void ChartDatedDatum::HandleOnNewBar( const CBar &bar) {
  m_pWindowBars->Add( bar.m_dt, new CBar( bar ) );   // TODO: need to do garbage collection after
  m_pWindowBars->UpdateWindow();
  UpdateChart();
}

void ChartDatedDatum::HandleOnBarUpdated( const CBar &bar) {
  m_pWindowBars->Add( bar.m_dt, new CBar( bar ) );   // TODO: need to do garbage collection after
  m_pWindowBars->UpdateWindow();
  UpdateChart();
  CBar *pUndoneBar = m_pWindowBars->UndoPush();
  //if ( pUndoneBar != pBar ) {
    delete pUndoneBar;  // need to emit an error message
  //}
  //delete pBar;
}

void ChartDatedDatum::Add(const CBar &bar) {
  m_pWindowBars->Add( bar.m_dt, new CBar( bar ) );   // TODO: need to do garbage collection after
  m_pWindowBars->UpdateWindow();
  UpdateChart();
}

void ChartDatedDatum::Add( const Trade &trade ) {
  m_factory.Add( trade.m_dt, trade.m_dblTrade, trade.m_nTradeSize );
}

void ChartDatedDatum::ClearChart() {
}

void ChartDatedDatum::UpdateChart() {
  if ( m_bUpdateChart ) {
    // draw chart
    CBar *pBar;

    pBar = m_pWindowBars->First();

    int n = (int) m_pWindowBars->Count();

    double *prTimeStamps = new double[ n ];
    double *prHi = new double[ n ];
    double *prLo = new double[ n ];
    double *prOpen = new double[ n ];
    double *prClose = new double[ n ];
    double *prVolume = new double[ n ];

    //double *prPivot1 = new double[ n ];
    //double *prPivot2 = new double[ n ];
    //double *prPivot3 = new double[ n ];
    //double *prPivot4 = new double[ n ];

    bool bHasVolume = false;
    int i = 0;
    while ( NULL != pBar ) {
      prTimeStamps[ i ] = Chart::chartTime( 
        pBar->m_dt.date().year(), 
        pBar->m_dt.date().month(), 
        pBar->m_dt.date().day(),
        pBar->m_dt.time_of_day().hours(),
        pBar->m_dt.time_of_day().minutes(),
        pBar->m_dt.time_of_day().seconds() );
//        0, 0, 0 );
      prHi[ i ] = pBar -> m_dblHigh;
      prLo[ i ] = pBar -> m_dblLow;
      prOpen[ i ] = pBar -> m_dblOpen;
      prClose[ i ] = pBar -> m_dblClose;
      double d = pBar -> m_nVolume;
      if ( 0 < d ) bHasVolume = true;
      prVolume[ i ] = d;
      //prPivot1[ i ] = 425.0;
      //prPivot2[ i ] = 525.0;
      //prPivot3[ i ] = 625.0;
      //prPivot4[ i ] = 675.0;
      pBar = m_pWindowBars->Next();
      i++;
    }

    DoubleArray daTimeStamps( prTimeStamps, n );
    DoubleArray daHi( prHi, n );
    DoubleArray daLo( prLo, n );
    DoubleArray daOpen( prOpen, n );
    DoubleArray daClose( prClose, n );
    DoubleArray daVolume( prVolume, n );
    //DoubleArray daPivot1( prPivot1, n );
    //DoubleArray daPivot2( prPivot2, n );
    //DoubleArray daPivot3( prPivot3, n );
    //DoubleArray daPivot4( prPivot4, n );

    if ( NULL != chart ) {
      delete chart;
      chart = NULL;
    }
/*
    XYChart *xyChart = new XYChart( 600, 350 );
    xyChart->setPlotArea( 50, 25, 500, 250 )->setGridColor( 0xc0c0c0, 0xc0c0c0 );
    xyChart->addTitle( "Special Chart" );
    xyChart->setYAxisOnRight( true );
    xyChart->yAxis()->setAutoScale();

    //xyChart->xAxis()->setDateScale(prTimeStamps[ 0 ], prTimeStamps[ n - 1 ]);
    xyChart->xAxis()->setDateScale( 
      prTimeStamps[ 0 ], prTimeStamps[ n - 1 ], m_majorTickInc, m_minorTickInc );
    xyChart->xAxis()->setTickDensity( 10, -1 );
    xyChart->xAxis()->setLabels( daTimeStamps )->setFontAngle(45);

    xyChart->xAxis()->setFormatCondition( "align", 360 * 86400 );
    xyChart->xAxis()->setLabelFormat( "{value|yyyy}" );

    xyChart->xAxis()->setFormatCondition( "align", 30 * 86400 );
    xyChart->xAxis()->setMultiFormat( 
      Chart::StartOfMonthFilter(), "{value|mmm/dd<*br*>yyyy}",
      Chart::AllPassFilter(), "{value|mmm/dd}", 25 );

    xyChart->xAxis()->setFormatCondition( "align", 86400 );
    xyChart->xAxis()->setMultiFormat( 
      Chart::StartOfYearFilter(), "{value|yyyy/mmm/dd<*br*>yyyy}",
      Chart::StartOfMonthFilter(), "{value|mmm/dd}", 1 );

    xyChart->xAxis()->setFormatCondition( "align", 3600  );
    xyChart->xAxis()->setMultiFormat( 
      Chart::StartOfDayFilter(), "{value|hh:nn:ss<*br*>yyyy/mm/dd}",
      Chart::StartOfHourFilter(), "{value|hh:nn:ss}", 10 );

    xyChart->xAxis()->setFormatCondition( "align", 240  );
    xyChart->xAxis()->setMultiFormat( 
      Chart::StartOfDayFilter(), "{value|hh:nn:ss<*br*>yyyy-mm-dd}",
      Chart::StartOfHourFilter(), "{value|hh:nn:ss}", 10 );

    //xyChart->xAxis()->setLabelStep( 5 );
    //xyChart->xAxis()->setMultiFormat(
    CandleStickLayer *layer 
      = xyChart->addCandleStickLayer( daHi, daLo, daOpen,daClose, 0x00ff00, 0xff0000 );
    layer->setLineWidth( 1 );
    //xyChart->layout();
    //xyChart->makeChart( "data.png" );
    setChart( xyChart );
*/    

///*
    //int n = pBars->Count();
    //int i;

    /*
    pChartDaily->m_chart.ClearChart();
    pChartDaily->m_chart.SetUpdateChart( false );
    pChartDaily->m_chart.setMajorTickInc( 30 * 86400  );
    pChartDaily->m_chart.setMinorTickInc(  86400  );

    for ( i = 0; i < n; i++ ) {
      pBar = pBars -> At( i );
      pChartDaily->m_chart.Add( pBar );
    }
    pChartDaily->m_chart.SetUpdateChart( true );
    pChartDaily->m_chart.UpdateChart();
    */

    chart = new FinanceChart( m_nChartWidth );
    chart->setDateLabelSpacing( 5 );
    chart->setDateLabelFormat( 
      "{value|yyyy}", "{value|yyyy/mmm}", 
      "{value|mmm}", "{value|yyyy/mm/dd hh}", 
      "{value|dd hh}", "{value|dd hh:nn}", "{value|hh:nn:ss}" );
    //chart->setXAxisStyle( "Arial", 8, TextColor, 45 );
    chart -> addTitle( m_sChartTitle.c_str() );
    chart -> setData( daTimeStamps, daHi, daLo, daOpen, daClose, daVolume, 0 );
    XYChart *main = chart -> addMainChart( m_nChartHeight );
    //chart->addHLOC(0x008000, 0xcc0000);
    chart->addCandleStick(0x00ff00, 0xff0000);
    if ( bHasVolume ) chart->addVolBars(70, 0x99ff99, 0xff9999, 0x808080);
    //XYChart *bars = chart->addVolIndicator(70, 0x99ff99, 0xff9999, 0x808080);
    //chart->addLineIndicator2( main, daPivot1, 0x99ff99, "Pivot1" );
    //chart->addLineIndicator2( main, daPivot2, 0xff9999, "Pivot2" );
    //chart->addLineIndicator2( main, daPivot3, 0x808080, "Pivot3" );
    //chart->addLineIndicator2( main, daPivot4, 0x00ff00, "Pivot4" );
    chart->layout();
    setChart( chart );
//*/
    //pChart->m_chart.setChart( chart );
    //chart->makeChart( "mychart.png" );

    //delete chart;

    delete [] prTimeStamps;
    delete [] prHi;
    delete [] prLo;
    delete [] prOpen;
    delete [] prClose;
    delete [] prVolume;
  }
}


