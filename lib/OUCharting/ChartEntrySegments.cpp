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

#include "ChartEntrySegments.h"

namespace ou { // One Unified

ChartEntrySegments::ChartEntrySegments(void) 
: ChartEntryBaseWithTime()
{
}

ChartEntrySegments::~ChartEntrySegments(void) {
}

void ChartEntrySegments::AddEntryToChart(XYChart *pXY, structChartAttributes *pAttributes) {
  if ( 0 < m_vPrice.size() ) {
    LineLayer *layer = pXY->addLineLayer( GetPrices(), m_eColour, m_sName.c_str() );
    DoubleArray daXData = ChartEntryBaseWithTime::GetDateTimes();
    layer->setXData( daXData );
    pAttributes->dblXMin = daXData[0];
    pAttributes->dblXMax = daXData[ daXData.len - 1 ];
  }
}

} // namespace ou
