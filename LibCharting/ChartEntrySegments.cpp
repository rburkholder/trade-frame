#include "StdAfx.h"
#include "ChartEntrySegments.h"

CChartEntrySegments::CChartEntrySegments(void) 
: CChartEntryBaseWithTime()
{
}

CChartEntrySegments::~CChartEntrySegments(void) {
}

void CChartEntrySegments::AddDataToChart(XYChart *pXY, structChartAttributes *pAttributes) {
  if ( 0 < m_vPrice.size() ) {
    LineLayer *layer = pXY->addLineLayer( GetPrice(), m_eColour, m_sName.c_str() );
    DoubleArray daXData = CChartEntryBaseWithTime::GetDateTime();
    layer->setXData( daXData );
    pAttributes->dblXMin = daXData[0];
    pAttributes->dblXMax = daXData[ daXData.len - 1 ];
  }
}