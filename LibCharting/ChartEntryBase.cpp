#include "StdAfx.h"
#include "ChartEntryBase.h"

#include "ChartDirector\chartdir.h"

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

void CChartEntryBase::Reserve(unsigned int nSize ) {
  m_vPrice.reserve( nSize );
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

void CChartEntryBaseWithTime::Reserve( unsigned int nSize ) {
  CChartEntryBase::Reserve( nSize );
  m_vDateTime.reserve( nSize );
  m_vChartTime.reserve( nSize );
}

void CChartEntryBaseWithTime::Add(boost::posix_time::ptime dt, double price) {
  CChartEntryBase::Add( price );
  m_vDateTime.push_back( dt );
  m_vChartTime.push_back( 
    Chart::chartTime( 
      dt.date().year(), dt.date().month(), dt.date().day(),
      dt.time_of_day().hours(), dt.time_of_day().minutes(), dt.time_of_day().seconds() ) );
}

