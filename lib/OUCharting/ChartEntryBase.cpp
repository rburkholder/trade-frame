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

#include <algorithm>

#include "ChartEntryBase.h"

namespace ou { // One Unified

//
// CChartEntryBase
//

ChartEntryBase::ChartEntryBase(): m_ixStart( 0 ), m_nElements( 0 ) {
}

ChartEntryBase::ChartEntryBase( size_type nSize ) 
: m_eColour( ou::Colour::Black )
{
  m_vPrice.reserve( nSize );
}

ChartEntryBase::~ChartEntryBase() {
  m_vPrice.clear();
}

void ChartEntryBase::Reserve( size_type nSize ) {
  m_vPrice.reserve( nSize );
}

void ChartEntryBase::Append(double price) {
  m_vPrice.push_back( price );
}

void ChartEntryBase::Clear( void ) {
  m_vPrice.clear();
  m_ixStart = 0;
  m_nElements = 0;
}

//
// CChartEntryBaseWithTime
//

ChartEntryBaseWithTime::ChartEntryBaseWithTime() : 
  ChartEntryBase(),
    m_dtViewPortBegin( boost::posix_time::not_a_date_time ), m_dtViewPortEnd( boost::posix_time::not_a_date_time )
{
}

ChartEntryBaseWithTime::ChartEntryBaseWithTime( size_type nSize )
: ChartEntryBase( nSize ), 
    m_dtViewPortBegin( boost::posix_time::not_a_date_time ), m_dtViewPortEnd( boost::posix_time::not_a_date_time )
{
  m_vDateTime.reserve( nSize );
  m_vChartTime.reserve( nSize );
  ChartEntryBase::Reserve( nSize );
}

ChartEntryBaseWithTime::~ChartEntryBaseWithTime() {
  m_vDateTime.clear();
  m_vChartTime.clear();
}

void ChartEntryBaseWithTime::Reserve( size_type nSize ) {
  ChartEntryBase::Reserve( nSize );
  m_vDateTime.reserve( nSize );
  m_vChartTime.reserve( nSize );
}

void ChartEntryBaseWithTime::SetViewPort( boost::posix_time::ptime dtBegin, boost::posix_time::ptime dtEnd ) {
  // record the viewport
  m_dtViewPortBegin = dtBegin;
  m_dtViewPortEnd = dtEnd;
  // initialize viewport values
  m_ixStart = 0;
  m_nElements = 0;
  // calculate new viewport values
  if ( 0 != m_vDateTime.size() ) {
    vDateTime_t::iterator iterBegin( m_vDateTime.begin() );
    vDateTime_t::iterator iterEnd( m_vDateTime.end() );

    if ( boost::posix_time::not_a_date_time != dtBegin ) {
      iterBegin = std::lower_bound( m_vDateTime.begin(), m_vDateTime.end(), dtBegin );
    }
    if ( m_vDateTime.end() != iterBegin ) {
      if ( boost::posix_time::not_a_date_time != dtEnd ) {
        iterEnd = std::upper_bound( iterBegin, m_vDateTime.end(), dtEnd );
      }
    }
    m_ixStart = iterBegin - m_vDateTime.begin();
    m_nElements = iterEnd - iterBegin;
  }
}

void ChartEntryBaseWithTime::Append( boost::posix_time::ptime dt) {
  // some Chart Entries don't use the built in vector
  m_vDateTime.push_back( dt );
  m_vChartTime.push_back( 
    Chart::chartTime( 
      dt.date().year(), dt.date().month(), dt.date().day(),
      dt.time_of_day().hours(), dt.time_of_day().minutes(), dt.time_of_day().seconds() ) );

  if ( ( boost::posix_time::not_a_date_time != m_dtViewPortEnd ) && ( dt >= m_dtViewPortEnd ) ) {
    // don't append any more values to visible area
  }
  else {
    ++m_nElements;
  }
}

void ChartEntryBaseWithTime::Append( boost::posix_time::ptime dt, double price) {
  while ( m_lfTimeDouble.push( TimeDouble_t( dt, price ) ) ) {};  // add error condition here
}

void ChartEntryBaseWithTime::ClearQueue( void ) {  
  TimeDouble_t td;
  while ( m_lfTimeDouble.pop( td ) ) {
    ChartEntryBase::Append( td.m_price );
    Append( td.m_dt );
  };
}

void ChartEntryBaseWithTime::Clear( void ) {
  m_vDateTime.clear();
  m_vChartTime.clear();
  ChartEntryBase::Clear();
}

} // namespace ou
