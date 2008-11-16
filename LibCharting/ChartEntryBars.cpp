#include "StdAfx.h"
#include "ChartEntryBars.h"

//
// CChartEntryVolume
//

CChartEntryVolume::CChartEntryVolume(void)
: CChartEntryBaseWithTime()
{
}

CChartEntryVolume::CChartEntryVolume(unsigned int nSize) 
: CChartEntryBaseWithTime(nSize)
{
}

CChartEntryVolume::~CChartEntryVolume(void) {
}

void CChartEntryVolume::Reserve(unsigned int nSize ) {
  CChartEntryBaseWithTime::Reserve( nSize );
}

void CChartEntryVolume::Add(const boost::posix_time::ptime &dt, int volume) {
  CChartEntryBaseWithTime::Add( dt, (double) volume );
}

void CChartEntryVolume::AddDataToChart( XYChart *pXY, structChartAttributes *pAttributes ) {
  if ( 0 != this->m_vDateTime.size() ) {
    BarLayer *bl = pXY->addBarLayer( this->GetPrice() );

    DoubleArray daXData = CChartEntryBaseWithTime::GetDateTime();
    bl->setXData( daXData );
    pAttributes->dblXMin = daXData[0];
    pAttributes->dblXMax = daXData[ daXData.len - 1 ];

    DataSet *pds = bl->getDataSet(0);
    pds->setDataColor( m_eColour );
  }
}

//
// CChartEntryBars
//

CChartEntryBars::CChartEntryBars(void) 
: CChartEntryBaseWithTime()
{
}

CChartEntryBars::CChartEntryBars(unsigned int nSize) 
: CChartEntryBaseWithTime(nSize)
{
}

CChartEntryBars::~CChartEntryBars(void) {
  m_vOpen.clear();
  m_vHigh.clear();
  m_vLow.clear();
  m_vClose.clear();
}

void CChartEntryBars::Reserve( unsigned int nSize ) {
  CChartEntryBaseWithTime::Reserve( nSize );
  m_vOpen.reserve( nSize );
  m_vHigh.reserve( nSize );
  m_vLow.reserve( nSize );
  m_vClose.reserve( nSize );
}

void CChartEntryBars::AddBar(const CBar &bar) {
  if ( m_vOpen.capacity() == m_vOpen.size() ) {
    int sz = m_vOpen.size() + ( m_vOpen.size() / 5 ); // expand by 20%
    //CChartEntryBaseWithTime::Reserve( sz );
    m_vOpen.reserve( sz ); 
    m_vHigh.reserve( sz ); 
    m_vLow.reserve( sz ); 
    m_vClose.reserve( sz ); 
  }
  CChartEntryBaseWithTime::Add( bar.m_dt );
  m_vOpen.push_back( bar.m_dblOpen );
  m_vHigh.push_back( bar.m_dblHigh );
  m_vLow.push_back( bar.m_dblLow );
  m_vClose.push_back( bar.m_dblClose );
}

void CChartEntryBars::AddDataToChart(XYChart *pXY, structChartAttributes *pAttributes) {
  if ( 0 != this->m_vDateTime.size() ) {
    CandleStickLayer *candle = pXY->addCandleStickLayer( 
      this->GetHigh(),
      this->GetLow(),
      this->GetOpen(),
      this->GetClose(),
      0x00ff00, 0xff0000
      );
    //candle->setDataGap( 0 );
    DoubleArray daXData = CChartEntryBaseWithTime::GetDateTime();
    candle->setXData( daXData );
    pAttributes->dblXMin = daXData[0];
    pAttributes->dblXMax = daXData[ daXData.len - 1 ];
  }
}
