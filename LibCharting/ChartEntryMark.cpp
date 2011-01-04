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

#include "StdAfx.h"

#include "ChartEntryMark.h"

CChartEntryMark::CChartEntryMark(void) 
: CChartEntryBase()
{
}

CChartEntryMark::~CChartEntryMark(void) {
}

void CChartEntryMark::AddMark(double price, ou::Colour::enumColour colour, const std::string &name) {
  m_vPrice.push_back( price );
  m_vColour.push_back( colour );
  m_vName.push_back( name );
}

void CChartEntryMark::AddDataToChart( XYChart *pXY, structChartAttributes *pAttributes ) {
  if ( 0 < m_vPrice.size() ) {
    // may need to make an adjustment for using only marks within a certain price range
    for ( size_t ix = 0; ix < m_vPrice.size(); ++ix ) {
      int i = m_vColour[ ix ];
      Mark *pmk = pXY->yAxis()->addMark( m_vPrice[ ix ], m_vColour[ ix ], m_vName[ ix ].c_str() );
      pmk->setAlignment( TopCenter );
      pmk->setDrawOnTop( false );
    }
  }
  
}
