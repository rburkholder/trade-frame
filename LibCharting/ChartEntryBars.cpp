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

void CChartEntryVolume::Add(const boost::posix_time::ptime &dt, double volume) {
  CChartEntryBaseWithTime::Add( dt, volume );
}

//
// CChartEntryBars
//

CChartEntryBars::CChartEntryBars(void) 
: CChartEntryVolume()
{
}

CChartEntryBars::CChartEntryBars(unsigned int nSize) 
: CChartEntryVolume(nSize)
{
}

CChartEntryBars::~CChartEntryBars(void) {
}

void CChartEntryBars::Reserve( unsigned int nSize ) {
  CChartEntryVolume::Reserve( nSize );
  m_vOpen.reserve( nSize );
  m_vHigh.reserve( nSize );
  m_vLow.reserve( nSize );
  m_vClose.reserve( nSize );
  //m_vVolume.reserve( nSize );
}

void CChartEntryBars::AddBar(const CBar &bar) {
  CChartEntryVolume::Add( bar.m_dt, bar.m_nVolume );
  m_vOpen.push_back( bar.m_dblOpen );
  m_vHigh.push_back( bar.m_dblHigh );
  m_vLow.push_back( bar.m_dblLow );
  m_vClose.push_back( bar.m_dblClose );
  //m_vVolume.push_back( bar.m_nVolume );
}
