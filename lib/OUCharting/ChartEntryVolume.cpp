/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include "ChartEntryVolume.h"

namespace ou { // One Unified

//
// CChartEntryVolume
//

ChartEntryVolume::ChartEntryVolume(void)
: ChartEntryBaseWithTime()
{
}

ChartEntryVolume::ChartEntryVolume(size_type nSize) 
: ChartEntryBaseWithTime(nSize)
{
}

ChartEntryVolume::~ChartEntryVolume(void) {
}

void ChartEntryVolume::Reserve(size_type nSize ) {
  ChartEntryBaseWithTime::Reserve( nSize );
}

void ChartEntryVolume::Append( boost::posix_time::ptime dt, int volume) {
  ChartEntryBaseWithTime::Append( dt, (double) volume );
}

bool ChartEntryVolume::AddEntryToChart( XYChart *pXY, structChartAttributes *pAttributes ) {
  bool bAdded( false );
  ChartEntryBaseWithTime::ClearQueue();
  if ( 0 != this->m_vDateTime.size() ) {
    DoubleArray daXData = ChartEntryBaseWithTime::GetDateTimes();
    if ( 0 != daXData.len ) {
      BarLayer *bl = pXY->addBarLayer( this->GetPrices() );
    
      bl->setXData( daXData );
      pAttributes->dblXMin = daXData[0];
      pAttributes->dblXMax = daXData[ daXData.len - 1 ];

      DataSet *pds = bl->getDataSet(0);
      pds->setDataColor( m_eColour, 0xff000000, 0xff000000 );

      bAdded = true;
    }
  }
  return bAdded;
}

} // ou namespace