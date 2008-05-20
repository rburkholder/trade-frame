#include "StdAfx.h"
#include "ChartEntrySegments.h"

CChartEntrySegments::CChartEntrySegments(void) 
: CChartEntryBaseWithTime()
{
}

CChartEntrySegments::~CChartEntrySegments(void) {
}

void CChartEntrySegments::SetAttributes(EColor color, std::string name ) {
  m_sName = name;
  m_eColor = color;
}
