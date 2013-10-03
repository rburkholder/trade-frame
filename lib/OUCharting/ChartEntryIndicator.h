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

#pragma once

#include "ChartEntryBase.h"

namespace ou { // One Unified

class ChartEntryIndicator :
  public ChartEntryBaseWithTime {
public:
  ChartEntryIndicator(void);
  ChartEntryIndicator( unsigned int nSize );
  virtual ~ChartEntryIndicator(void);
  virtual void Reserve( unsigned int );
  virtual void AddEntryToChart( XYChart *pXY, structChartAttributes *pAttributes ) const;
protected:
private:
};

} // namespace ou
