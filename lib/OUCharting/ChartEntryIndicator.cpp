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

ChartEntryIndicator::ChartEntryIndicator( size_type nSize ) 
: ChartEntryBaseWithTime( nSize )
{
}

ChartEntryIndicator::~ChartEntryIndicator(void) {
}

void ChartEntryIndicator::Reserve( size_type nSize ) {
  ChartEntryBaseWithTime::Reserve( nSize );
}

bool ChartEntryIndicator::AddEntryToChart(XYChart *pXY, structChartAttributes *pAttributes)  {
  bool bAdded( false );
  ChartEntryBaseWithTime::ClearQueue();
  if ( 0 != this->m_vDateTime.size() ) {
    DoubleArray daXData = ChartEntryBaseWithTime::GetDateTimes();
    if ( 0 != daXData.len ) {
      LineLayer *ll = pXY->addLineLayer( this->GetPrices() );
      ll->setXData( daXData );
      pAttributes->dblXMin = daXData[0];
      pAttributes->dblXMax = daXData[ daXData.len - 1 ];
      DataSet *pds = ll->getDataSet(0);
      pds->setDataColor( m_eColour );
      pds->setDataName( GetName().c_str() );
      bAdded = true;
    }
  }
  return bAdded;
}

} // namespace ou
