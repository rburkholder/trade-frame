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

CChartEntryIndicator::CChartEntryIndicator(void) 
: CChartEntryBaseWithTime() 
{
}

CChartEntryIndicator::CChartEntryIndicator( unsigned int nSize ) 
: CChartEntryBaseWithTime( nSize )
{
}

CChartEntryIndicator::~CChartEntryIndicator(void) {
}

void CChartEntryIndicator::Reserve( unsigned int nSize ) {
  CChartEntryBaseWithTime::Reserve( nSize );
}

void CChartEntryIndicator::AddDataToChart(XYChart *pXY, structChartAttributes *pAttributes) {
  if ( 0 != this->m_vDateTime.size() ) {
    LineLayer *ll = pXY->addLineLayer( this->GetPrices() );
    DoubleArray daXData = CChartEntryBaseWithTime::GetDateTimes();
    ll->setXData( daXData );
    pAttributes->dblXMin = daXData[0];
    pAttributes->dblXMax = daXData[ daXData.len - 1 ];
    DataSet *pds = ll->getDataSet(0);
    pds->setDataColor( m_eColour );
  }
}

} // namespace ou
