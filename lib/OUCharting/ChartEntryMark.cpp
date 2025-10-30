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

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include "ChartEntryMark.h"

namespace ou { // One Unified

ChartEntryMark::ChartEntryMark()
: ChartEntryBase()
{
}

ChartEntryMark::~ChartEntryMark() {
  Clear();
}

// used in background thread
void ChartEntryMark::AddMark(double price, ou::Colour::EColour colour, const std::string &name) {
  //if ( m_bUseThreadSafety ) {
  //  Mark_t mark( price, colour, name );
  //  while ( !m_lfMark.push( mark ) ) {};
  //}
  //else {
  //  m_vPrice.push_back( price );
  //  m_vColour.push_back( colour );
  //  m_vName.push_back( name );
  //}
  AddMark( Mark_t( price, colour, name ) );
}

void ChartEntryMark::AddMark( const Mark_t& mark ) {
  m_queue.Append( mark );
}

// from the queue in main thread
void ChartEntryMark::Pop( const Mark_t& mark ) {
  m_vPrice.push_back( mark.m_dblPrice );
  m_vColour.push_back( mark.m_colour );
  m_vName.push_back( mark.m_sName );
}

bool ChartEntryMark::AddEntryToChart( XYChart* pXY, structChartAttributes& attributes ) {

  bool bAdded( false );

  namespace args = boost::phoenix::placeholders;
  m_queue.Sync( boost::phoenix::bind( &ChartEntryMark::Pop, this, args::arg1 ) );

  if ( 0 < m_vPrice.size() ) {
    // may need to make an adjustment for using only marks within a certain price range
    for ( size_t ix = 0; ix < m_vPrice.size(); ++ix ) {
      Mark* pmk = pXY->yAxis()->addMark( m_vPrice[ ix ], m_vColour[ ix ], m_vName[ ix ].c_str() );
      pmk->setLineWidth( 1 );
      pmk->setAlignment( Chart::Left );
      pmk->setDrawOnTop( false );
    }
    bAdded = true;
  }
  return bAdded;
}

void ChartEntryMark::Clear() {
  //ChartEntryBase::Clear();
  m_vPrice.clear();
  m_vColour.clear();
  m_vName.clear();
}

} // namespace ou
