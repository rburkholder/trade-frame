#include "StdAfx.h"
#include "GTScalp.h"
#include "ChartArmsIntraDay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CChartArmsIntraDay
CChartArmsIntraDay::CChartArmsIntraDay(void) : CChartViewerShim() {

  m_dblIndu = m_dblTrin = -1;
  m_bInduChanged = m_bTrinChanged = false;
  pChart = NULL;

  m_bFirstInduFound = false;

  pzzIndu = new CZigZag( 5.0 );
  pzzIndu->SetOnPeakFound( MakeDelegate( this, &CChartArmsIntraDay::ZZInduChanged ) );
  pzzTrin = new CZigZag( 0.15 );
  pzzTrin->SetOnPeakFound( MakeDelegate( this, &CChartArmsIntraDay::ZZTrinChanged ) );
}

CChartArmsIntraDay::~CChartArmsIntraDay(void) {
  if ( NULL != pChart ) delete pChart;
  delete pzzIndu;
  delete pzzTrin;
}

void CChartArmsIntraDay::ProcessIndu( const Trade &trade ) {
  //stringstream ss;
  //ss << "Indu " << pmc->m_dt;
  //theApp.pConsoleMessages->WriteLine( ss.str().c_str() );
  //Trade *pTrade = (Trade *) pDatum;
  if ( !m_bFirstInduFound ) {
    m_bFirstInduFound = true;
    m_dblFirstIndu = trade.m_dblTrade;
  }
  double t = trade.m_dblTrade - m_dblFirstIndu;
  m_dblIndu = t;
  pzzIndu->Check( trade.m_dt, t );
  DrawChart();
}

void CChartArmsIntraDay::ProcessTrin( const Trade &trade ) {
  //stringstream ss;
  //ss << "Trin " << pmc->m_dt;
  //theApp.pConsoleMessages->WriteLine( ss.str().c_str() );
  //Trade *pTrade = (Trade *) pDatum;
  double t = trade.m_dblTrade;
  m_dblTrin = t;
  pzzTrin->Check( trade.m_dt, t );
  //DrawChart();
}

void CChartArmsIntraDay::ProcessTick( const Trade &trade ) {
  //stringstream ss;
  //ss << "Tick " << pmc->m_dt;
  //theApp.pConsoleMessages->WriteLine( ss.str().c_str() );
  //Trade *pTrade = (Trade *) pDatum;
}

void CChartArmsIntraDay::ZZInduChanged( CZigZag *zz, ptime dt, double pt, CZigZag::EDirection dir) {
  m_dblInduPt1 = pt;
  m_bInduChanged = true;
  //if ( -1 != m_dblTrin ) {
    //m_vdblIndu.push_back( m_dblIndu );
    //m_vdblTrin.push_back( m_dblTrin );
    //DrawChart();
  //}
}

void CChartArmsIntraDay::ZZTrinChanged( CZigZag *zz, ptime dt, double pt, CZigZag::EDirection dir ) {
  m_dblTrinPt1 = pt;
  m_bTrinChanged = true;
  //if ( -1 != m_dblIndu ) {
    //m_vdblTrin.push_back( m_dblTrin );
    //m_vdblIndu.push_back( m_dblIndu );
    DrawChart();
  //}
}

void CChartArmsIntraDay::DrawChart( void ) {

  if ( m_bInduChanged && m_bTrinChanged ) {
    m_vdblIndu.push_back( m_dblInduPt1 );
    m_vdblTrin.push_back( m_dblTrinPt1 );
    m_bInduChanged = m_bTrinChanged = false;
  }

  m_vdblIndu.push_back( m_dblIndu );
  m_vdblTrin.push_back( m_dblTrin );

  // do an overlay for the leading line, so can get a different color on it
  if ( NULL != pChart ) delete pChart;
  pChart = new XYChart( m_nChartWidth, m_nChartHeight );
  pChart ->addTitle( "Trin vs Delta Indu" );
  pChart ->setPlotArea( 30, 10, 550, 130, 0xffffff, -1, 0xc0c0c0, 0xc0c0c0, -1 );
  pChart ->yAxis()->setLogScale( true );
  pChart ->addText(  30,  15, "NormDecl" );
  pChart ->addText(  30,  60, "WeakDecl" );
  pChart ->addText(  30, 105, "UpSoon" );
  pChart ->addText( 250, 105, "UpSn!!" );
  pChart ->addText( 460, 105, "NormAdv" );
  pChart ->addText( 460,  60, "WeakAdv" );
  pChart ->addText( 470,  15, "DnSoon" );
  pChart ->addText( 250,  15, "DnSn!!" );

  vector<double>::iterator iterIndu, iterTrin;
  size_t i, j;
  j = m_vdblIndu.size();
  double *pdblIndu = new double[ j ];
  double *pdblTrin = new double[ j ];
  iterIndu = m_vdblIndu.begin();
  iterTrin = m_vdblTrin.begin();
  i = 0;
  while ( i < j ) {
    pdblIndu[ i ] = *iterIndu;
    pdblTrin[ i ] = *iterTrin;
    ++iterIndu;
    ++iterTrin;
    ++i;
  }
  DoubleArray daIndu( pdblIndu, j );
  DoubleArray daTrin( pdblTrin, j );
  pLLIndu = pChart->addLineLayer( j, pdblTrin, 0x33ff33, "Indu" );
  pLLIndu->setXData( daIndu );
  pLLIndu->setLineWidth( 3 );

  setChart( pChart );
  delete [] pdblIndu;
  delete [] pdblTrin;

  m_vdblIndu.pop_back();
  m_vdblTrin.pop_back();
}