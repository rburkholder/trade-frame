#include "StdAfx.h"
#include "ChartEntrySegments.h"

CChartEntrySegments::CChartEntrySegments(void) 
: CChartEntryBaseWithTime()
{
}

CChartEntrySegments::~CChartEntrySegments(void) {
}

void CChartEntrySegments::AddDataToChart(XYChart *pXY) {
  if ( 0 < m_vPrice.size() ) {
    LineLayer *layer = pXY->addLineLayer( GetPrice(), m_eColour, m_sName.c_str() );
    layer->setXData( GetDateTime() );
  }
}