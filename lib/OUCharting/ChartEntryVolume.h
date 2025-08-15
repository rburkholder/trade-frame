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

#pragma once

#include <TFTimeSeries/DatedDatum.h>

#include "ChartEntryPrice.h"

namespace ou { // One Unified

// volume is located here due to its association with ChartEntryBars,
//  which does not have volume as the volume portion is in another location in the chart set
class ChartEntryVolume: public ChartEntryPrice {
public:

  typedef ChartEntryPrice::size_type size_type;

  ChartEntryVolume(void);
  //ChartEntryVolume(size_type nSize);
  virtual ~ChartEntryVolume();
  virtual void Reserve( size_type );
  void Append( ptime dt, int volume );
  void Append( const ou::tf::Bar& bar );
  virtual bool AddEntryToChart( XYChart *pXY, structChartAttributes *pAttributes );

protected:
private:
};

} // namespace ou