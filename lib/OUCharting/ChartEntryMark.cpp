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

#include "ChartEntryMark.h"

namespace ou { // One Unified

ChartEntryMark::ChartEntryMark(void) 
: ChartEntryBase()
{
}

ChartEntryMark::~ChartEntryMark(void) {
}

void ChartEntryMark::AddMark(double price, ou::Colour::enumColour colour, const std::string &name) {
  if ( m_bUseThreadSafety ) {
    Mark_t mark( price, colour, name );
    while ( !m_lfMark.push( mark ) ) {};
  }
  else {
    m_vPrice.push_back( price );
    m_vColour.push_back( colour );
    m_vName.push_back( name );
  }
}

bool ChartEntryMark::AddEntryToChart( XYChart *pXY, structChartAttributes *pAttributes ) {
  bool bAdded( false );
  Mark_t mark;
  while ( m_lfMark.pop( mark ) ) {
    m_vPrice.push_back( mark.m_dblPrice );
    m_vColour.push_back( mark.m_colour );
    m_vName.push_back( mark.m_sName );
  }

  if ( 0 < m_vPrice.size() ) {
    // may need to make an adjustment for using only marks within a certain price range
    for ( size_t ix = 0; ix < m_vPrice.size(); ++ix ) {
      int i = m_vColour[ ix ];
      Mark *pmk = pXY->yAxis()->addMark( m_vPrice[ ix ], m_vColour[ ix ], m_vName[ ix ].c_str() );
      pmk->setAlignment( Left );
      pmk->setDrawOnTop( false );
    }
    bAdded = true;
  }
  return bAdded;
}

void ChartEntryMark::Clear( void ) {
  m_vColour.clear();
  m_vName.clear();
  ChartEntryBase::Clear();
}

} // namespace ou
