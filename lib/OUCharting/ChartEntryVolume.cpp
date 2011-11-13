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

ChartEntryVolume::ChartEntryVolume(unsigned int nSize) 
: ChartEntryBaseWithTime(nSize)
{
}

ChartEntryVolume::~ChartEntryVolume(void) {
}

void ChartEntryVolume::Reserve(unsigned int nSize ) {
  ChartEntryBaseWithTime::Reserve( nSize );
}

void ChartEntryVolume::Add(const boost::posix_time::ptime &dt, int volume) {
  ChartEntryBaseWithTime::Add( dt, (double) volume );
}

void ChartEntryVolume::AddDataToChart( XYChart *pXY, structChartAttributes *pAttributes ) const {
  if ( 0 != this->m_vDateTime.size() ) {
    BarLayer *bl = pXY->addBarLayer( this->GetPrices() );

    DoubleArray daXData = ChartEntryBaseWithTime::GetDateTimes();
    bl->setXData( daXData );
    pAttributes->dblXMin = daXData[0];
    pAttributes->dblXMax = daXData[ daXData.len - 1 ];

    DataSet *pds = bl->getDataSet(0);
    pds->setDataColor( m_eColour );
  }
}

} // ou namespace