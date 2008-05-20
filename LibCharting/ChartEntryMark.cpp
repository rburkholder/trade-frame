#include "StdAfx.h"
#include "ChartEntryMark.h"

CChartEntryMark::CChartEntryMark(void) 
: CChartEntryBase()
{
}

CChartEntryMark::~CChartEntryMark(void) {
}

void CChartEntryMark::AddMark(double price, EColor color, std::string name) {
  m_vPrice.push_back( price );
  m_vColor.push_back( color );
  m_vName.push_back( name );
}
