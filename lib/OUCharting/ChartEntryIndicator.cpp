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

#include "ChartEntryIndicator.h"

namespace ou { // One Unified

ChartEntryIndicator::ChartEntryIndicator(void) 
: ChartEntryBaseWithTime() 
{
}

ChartEntryIndicator::ChartEntryIndicator( unsigned int nSize ) 
: ChartEntryBaseWithTime( nSize )
{
}

ChartEntryIndicator::~ChartEntryIndicator(void) {
}

void ChartEntryIndicator::Reserve( unsigned int nSize ) {
  ChartEntryBaseWithTime::Reserve( nSize );
}

void ChartEntryIndicator::AddDataToChart(XYChart *pXY, structChartAttributes *pAttributes) const {
  if ( 0 != this->m_vDateTime.size() ) {
    LineLayer *ll = pXY->addLineLayer( this->GetPrices() );
    DoubleArray daXData = ChartEntryBaseWithTime::GetDateTimes();
    ll->setXData( daXData );
    pAttributes->dblXMin = daXData[0];
    pAttributes->dblXMax = daXData[ daXData.len - 1 ];
    DataSet *pds = ll->getDataSet(0);
    pds->setDataColor( m_eColour );
  }
}

} // namespace ou
