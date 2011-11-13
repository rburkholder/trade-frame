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

#include <TFTimeSeries/DatedDatum.h>

#include "ChartEntryBase.h"

namespace ou { // One Unified

// volume is located here due to its association with ChartEntryBars, 
//  which does not have volume as the volume portion is in another location in the chart set
class ChartEntryVolume: public ChartEntryBaseWithTime {
public:
  ChartEntryVolume(void);
  ChartEntryVolume(unsigned int nSize);
  virtual ~ChartEntryVolume(void);
  virtual void Reserve( unsigned int );
  void Add( const ptime &dt, int volume );
  virtual void AddDataToChart( XYChart *pXY, structChartAttributes *pAttributes ) const;
protected:
private:
};

} // namespace ou