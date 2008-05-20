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

void CChartEntryBars::AddBar(const CBar &bar) {
  m_vDateTime.push_back( bar.m_dt );
  m_vOpen.push_back( bar.m_dblOpen );
  m_vHigh.push_back( bar.m_dblHigh );
  m_vLow.push_back( bar.m_dblLow );
  m_vClose.push_back( bar.m_dblClose );
  m_vVolume.push_back( bar.m_nVolume );
}
