#include "StdAfx.h"
#include "ChartEntryBase.h"


//
// CChartEntryBase
//

CChartEntryBase::CChartEntryBase() {
}

CChartEntryBase::CChartEntryBase( unsigned int nSize ) {
  m_vPrice.reserve( nSize );
}

CChartEntryBase::~CChartEntryBase() {
  m_vPrice.clear();
}

void CChartEntryBase::Add(double price) {
  m_vPrice.push_back( price );
}

//
// CChartEntryBaseWithTime
//

CChartEntryBaseWithTime::CChartEntryBaseWithTime() : CChartEntryBase() {
}

CChartEntryBaseWithTime::CChartEntryBaseWithTime( unsigned int nSize )
: CChartEntryBase( nSize ) 
{
  m_vDateTime.reserve( nSize );
}

CChartEntryBaseWithTime::~CChartEntryBaseWithTime() {
  m_vDateTime.clear();
}

void CChartEntryBaseWithTime::Add(boost::posix_time::ptime dt, double price) {
  CChartEntryBase::Add( price );
  m_vDateTime.push_back( dt );
}

