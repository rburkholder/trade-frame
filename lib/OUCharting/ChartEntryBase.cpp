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

#include <algorithm>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include "ChartEntryBase.h"

namespace ou { // One Unified

//
// ChartEntryBase
//

ChartEntryBase::ChartEntryBase()
: m_ixStart( 0 ), m_nElements( 0 ), m_eColour( ou::Colour::Black )/*, m_bUseThreadSafety( false )*/ {
}

ChartEntryBase::~ChartEntryBase() {
}

//
// ChartEntryTime
//

ChartEntryTime::ChartEntryTime() : 
  ChartEntryBase(),
    m_dtViewPortBegin( boost::posix_time::not_a_date_time ), m_dtViewPortEnd( boost::posix_time::not_a_date_time )
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

    if ( ( boost::posix_time::not_a_date_time != m_dtViewPortEnd ) && ( dt > m_dtViewPortEnd ) ) {
      // don't append any more values to visible area
    }
    else {
      ++m_nElements;
    }
  }
  catch(...) {
    std::cout << "there is probably a memory issue" << std::endl;
  }
}

// called from WinChartView::ThreadDrawChart1 -> ChartDataView::SetViewPort
void ChartEntryTime::SetViewPort( boost::posix_time::ptime dtBegin, boost::posix_time::ptime dtEnd ) {
  // record the viewport
  m_dtViewPortBegin = dtBegin;
  m_dtViewPortEnd = dtEnd;
  // initialize viewport values
  m_ixStart = 0;
  m_nElements = 0;
  
  // calculate new viewport values
  // todo: what happens when nothing is within the range, should have zero elements listed
  
  // should this be here or not?
  //ClearQueue();  // should this be here?
  
  if ( 0 != m_vDateTime.size() ) {
    vDateTime_t::const_iterator iterBegin( m_vDateTime.begin() );
    vDateTime_t::const_iterator iterEnd( m_vDateTime.end() );

    if ( boost::posix_time::not_a_date_time != dtBegin ) {
      iterBegin = std::lower_bound( m_vDateTime.begin(), m_vDateTime.end(), dtBegin );
    }
    if ( m_vDateTime.end() != iterBegin ) {
      if ( boost::posix_time::not_a_date_time != dtEnd ) {
        iterEnd = std::upper_bound( iterBegin, m_vDateTime.cend(), dtEnd );
      }
      m_ixStart = iterBegin - m_vDateTime.begin();
      m_nElements = iterEnd - iterBegin;
    }
  }
}


// there are out-of-order issues or loss-of-data issues if m_bUseThreadSafety is changed while something is in the Queue
void ChartEntryTime::ClearQueue( void ) {  
  namespace args = boost::phoenix::placeholders;
  m_queue.Sync( boost::phoenix::bind( &ChartEntryTime::AppendFg, this, args::arg1 ) );
}

void ChartEntryTime::Clear( void ) {
  m_vDateTime.clear();
  m_vChartTime.clear();
}

} // namespace ou
