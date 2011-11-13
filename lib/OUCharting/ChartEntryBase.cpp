/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

//#include "StdAfx.h"

#include "ChartEntryBase.h"

namespace ou { // One Unified

//
// CChartEntryBase
//

ChartEntryBase::ChartEntryBase() {
}

ChartEntryBase::ChartEntryBase( unsigned int nSize ) 
: m_eColour( ou::Colour::Black )
{
  m_vPrice.reserve( nSize );
}

ChartEntryBase::~ChartEntryBase() {
  m_vPrice.clear();
}

void ChartEntryBase::Reserve(unsigned int nSize ) {
  m_vPrice.reserve( nSize );
}

void ChartEntryBase::Add(double price) {
  /*
  if ( m_vPrice.capacity() == m_vPrice.size() ) {
    m_vPrice.reserve( m_vPrice.size() + ( m_vPrice.size() / 5 ) ); // expand by 20%
  }
  */
  m_vPrice.push_back( price );
}

//
// CChartEntryBaseWithTime
//

ChartEntryBaseWithTime::ChartEntryBaseWithTime() : ChartEntryBase() {
}

ChartEntryBaseWithTime::ChartEntryBaseWithTime( unsigned int nSize )
: ChartEntryBase( nSize ) 
{
  m_vDateTime.reserve( nSize );
  m_vChartTime.reserve( nSize );
}

ChartEntryBaseWithTime::~ChartEntryBaseWithTime() {
  m_vDateTime.clear();
  m_vChartTime.clear();
}

void ChartEntryBaseWithTime::Reserve( unsigned int nSize ) {
  ChartEntryBase::Reserve( nSize );
  m_vDateTime.reserve( nSize );
  m_vChartTime.reserve( nSize );
}

void ChartEntryBaseWithTime::Add(const boost::posix_time::ptime &dt) {
  // some Chart Entries don't use the built in vector
  /*
  if ( m_vDateTime.capacity() == m_vDateTime.size() ) {
    m_vDateTime.reserve( m_vDateTime.size() + ( m_vDateTime.size() / 5 ) ); // expand by 20%
    m_vChartTime.reserve( m_vChartTime.size() + ( m_vChartTime.size() / 5 ) ); // expand by 20%
  }
  */
  m_vDateTime.push_back( dt );
  m_vChartTime.push_back( 
    Chart::chartTime( 
      dt.date().year(), dt.date().month(), dt.date().day(),
      dt.time_of_day().hours(), dt.time_of_day().minutes(), dt.time_of_day().seconds() ) );
}

void ChartEntryBaseWithTime::Add( const boost::posix_time::ptime &dt, double price) {
  ChartEntryBase::Add( price );
  Add( dt );
}

} // namespace ou
