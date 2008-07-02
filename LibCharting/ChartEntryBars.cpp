#include "StdAfx.h"
#include "ChartEntryBars.h"

CChartEntryBars::CChartEntryBars(void) 
: CChartEntryBaseWithTime()
{
}

CChartEntryBars::CChartEntryBars(unsigned int nSize) 
: CChartEntryBaseWithTime(nSize)
{
}

CChartEntryBars::~CChartEntryBars(void) {
}

void CChartEntryBars::Reserve( unsigned int nSize ) {
  CChartEntryBaseWithTime::Reserve( nSize );
  m_vOpen.reserve( nSize );
  m_vHigh.reserve( nSize );
  m_vLow.reserve( nSize );
  m_vClose.reserve( nSize );
  m_vVolume.reserve( nSize );
}

void CChartEntryBars::AddBar(const CBar &bar) {
  CChartEntryBaseWithTime::Add( bar.m_dt, 0 );
  m_vOpen.push_back( bar.m_dblOpen );
  m_vHigh.push_back( bar.m_dblHigh );
  m_vLow.push_back( bar.m_dblLow );
  m_vClose.push_back( bar.m_dblClose );
  m_vVolume.push_back( bar.m_nVolume );
}
