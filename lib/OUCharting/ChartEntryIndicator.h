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

#include "ChartEntryPrice.h"

namespace ou { // One Unified

class ChartEntryIndicator :
  public ChartEntryPrice {
public:
  ChartEntryIndicator();
  ChartEntryIndicator( ChartEntryIndicator&& );
  //ChartEntryIndicator( size_type nSize );
  virtual ~ChartEntryIndicator();
protected:
private:
};

} // namespace ou
