#include "StdAfx.h"
#include "ChartEntryIndicator.h"

CChartEntryIndicator::CChartEntryIndicator(void) 
: CChartEntryBaseWithTime() 
{
}

CChartEntryIndicator::CChartEntryIndicator( unsigned int nSize ) 
: CChartEntryBaseWithTime( nSize )
{
}

CChartEntryIndicator::~CChartEntryIndicator(void) {
}

void CChartEntryIndicator::Reserve( unsigned int nSize ) {
  CChartEntryBaseWithTime::Reserve( nSize );

}
