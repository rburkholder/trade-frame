#include "StdAfx.h"
#include "ChartEntrySegments.h"

CChartEntrySegments::CChartEntrySegments(void) 
: CChartEntryBaseWithTime()
{
}

CChartEntrySegments::~CChartEntrySegments(void) {
}

void CChartEntrySegments::SetAttributes(Colour::enumColour colour, const std::string &name ) {
  m_sName = name;
  m_eColour = colour;
}
