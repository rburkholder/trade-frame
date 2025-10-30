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

// for example, the zig zag indicator
// also use for high and low indicators, keep redoing last segment so stretches to
//   right hand edge (optionally, perhaps in a super class)

#include "ChartEntryPrice.h"

namespace ou { // One Unified

class ChartEntrySegments :
  public ChartEntryPrice {
public:
  ChartEntrySegments(void);
  virtual ~ChartEntrySegments();
  virtual bool AddEntryToChart( XYChart *pXY, structChartAttributes& ) override;
protected:

private:
};

} // namespace ou
