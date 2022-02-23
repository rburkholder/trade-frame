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

#include <iostream>
#include <algorithm>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include "ChartEntryBase.h"

namespace ou { // One Unified

//
// ChartEntryBase
//

ChartEntryBase::ChartEntryBase()
: m_ixStart( 0 ), m_nElements( 0 ), m_eColour( ou::Colour::Black ) {
}

ChartEntryBase::ChartEntryBase( const ChartEntryBase& rhs )
: m_ixStart( rhs.m_ixStart ), m_nElements( rhs.m_nElements ), m_eColour( rhs.m_eColour )
, m_sName( rhs.m_sName )
{
}

ChartEntryBase::ChartEntryBase( ChartEntryBase&& rhs )
: m_ixStart( rhs.m_ixStart ), m_nElements( rhs.m_nElements ), m_eColour( rhs.m_eColour )
, m_sName( std::move( rhs.m_sName ) )
{
}

ChartEntryBase::~ChartEntryBase() {
}

//
// ChartEntryTime
//

ChartEntryTime::ChartEntryTime() :
  ChartEntryBase()
{
}

//ChartEntryTime::ChartEntryTime( size_type nSize )
//: ChartEntryBase( nSize ),
//    m_dtViewPortBegin( boost::posix_time::not_a_date_time ), m_dtViewPortEnd( boost::posix_time::not_a_date_time )
//{
//  m_bufferedDateTime.Reserve( nSize );
//  m_vChartTime.reserve( nSize );
//  ChartEntryBase::Reserve( nSize );
//}

//ChartEntryBaseWithTime::ChartEntryBaseWithTime( const ChartEntryBaseWithTime& rhs ) :
//  ChartEntryBase( rhs ),
//    m_dtViewPortBegin( rhs.m_dtViewPortBegin ), m_dtViewPortEnd( rhs.m_dtViewPortEnd ),
//    m_vDateTime( rhs.m_vDateTime ), m_vChartTime( rhs.m_vChartTime )
//{
//  m_plfTimeDouble = new lfTimeDouble_t;
//}

ChartEntryTime::~ChartEntryTime() {
  Clear();
}

ChartEntryTime::ChartEntryTime( ChartEntryTime&& rhs )
: ChartEntryBase( std::move( rhs ) )
, m_rangeViewPort( std::move( rhs.m_rangeViewPort ) )
, m_vDateTime( std::move( rhs.m_vDateTime ) )
, m_vChartTime( std::move( rhs.m_vChartTime ) )
, m_queue( std::move( rhs.m_queue ) )
{
}

void ChartEntryTime::Reserve( size_type nSize ) {
  m_vDateTime.reserve( nSize );
  m_vChartTime.reserve( nSize );
}

void ChartEntryTime::Append(boost::posix_time::ptime dt) {
  m_queue.Append( dt );
}

// runs in thread of main
void ChartEntryTime::AppendFg(boost::posix_time::ptime dt) {
  m_vDateTime.push_back( dt );

  // this is maybe done on the fly and not correct here.
  try {
    m_vChartTime.push_back(
      Chart::chartTime(
        dt.date().year(), dt.date().month(), dt.date().day(),
        dt.time_of_day().hours(), dt.time_of_day().minutes(), dt.time_of_day().seconds() ) );

    if ( ( boost::posix_time::not_a_date_time != m_rangeViewPort.dtEnd ) && ( dt > m_rangeViewPort.dtEnd ) ) {
      // don't append any more values to visible area
    }
    else {
     IncCntElements();
    }
  }
  catch(...) {
    std::cout << "there is probably a memory issue" << std::endl;
  }
}

// called from WinChartView::ThreadDrawChart1 -> ChartDataView::SetViewPort
void ChartEntryTime::SetViewPort( boost::posix_time::ptime dtBegin, boost::posix_time::ptime dtEnd ) {
  SetViewPort( range_t( dtBegin, dtEnd ) );
}

void ChartEntryTime::SetViewPort( const range_t& range ) {

  // record the viewport
  m_rangeViewPort = range;

  // initialize viewport values
  SetIxStart( 0 );
  SetCntElements( 0 );

  // calculate new viewport values
  // todo: what happens when nothing is within the range, should have zero elements listed

  // should this be here or not?
  //ClearQueue();  // should this be here?

  // TODO: why would there be not_a_date_time in the vector?
  //   need to test for insertions like this?
  if ( 0 != m_vDateTime.size() ) {
    vDateTime_t::const_iterator iterBegin( m_vDateTime.begin() );
    vDateTime_t::const_iterator iterEnd( m_vDateTime.end() );

    if ( boost::posix_time::not_a_date_time != m_rangeViewPort.dtBegin ) {
      iterBegin = std::lower_bound( m_vDateTime.begin(), m_vDateTime.end(), m_rangeViewPort.dtBegin );
    }
    if ( m_vDateTime.end() != iterBegin ) {
      if ( boost::posix_time::not_a_date_time != m_rangeViewPort.dtEnd ) {
        iterEnd = std::upper_bound( iterBegin, m_vDateTime.cend(), m_rangeViewPort.dtEnd );
      }
      SetIxStart( iterBegin - m_vDateTime.begin() );
      SetCntElements( iterEnd - iterBegin );
    }
/*    else { // need to fix this for simulation viewport
      if ( 0 != m_vDateTime.size() ) {
        iterBegin = m_vDateTime.begin();
        iterEnd = m_vDateTime.end();
        SetIxStart( iterBegin - m_vDateTime.begin() );
        SetCntElements( iterEnd - iterBegin );
      }
    }
*/  }
}

ChartEntryTime::range_t ChartEntryTime::GetExtents() const {
  if ( 0 == m_vDateTime.size() ) {
    return range_t( boost::posix_time::not_a_date_time, boost::posix_time::not_a_date_time );
  }
  else {
    return range_t( m_vDateTime.front(), m_vDateTime.back() );
  }
}

boost::posix_time::ptime ChartEntryTime::GetExtentBegin() const {
  return 0 == m_vDateTime.size() ? boost::posix_time::not_a_date_time : m_vDateTime.front();
}

boost::posix_time::ptime ChartEntryTime::GetExtentEnd() const {
  return 0 == m_vDateTime.size() ? boost::posix_time::not_a_date_time : m_vDateTime.back();
}

// there are out-of-order issues or loss-of-data issues if m_bUseThreadSafety is changed while something is in the Queue
void ChartEntryTime::ClearQueue( void ) {
  namespace args = boost::phoenix::placeholders;
  m_queue.Sync( boost::phoenix::bind( &ChartEntryTime::AppendFg, this, args::arg1 ) );
}

void ChartEntryTime::Clear( void ) {
  ChartEntryBase::Clear();
  m_vDateTime.clear();
  m_vChartTime.clear();
  ChartEntryBase::Clear();
}

} // namespace ou
