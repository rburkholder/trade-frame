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

#include <TFIQFeed/IQFeedHistoryQuery.h>

class HistoryDailyTick:  public ou::tf::CIQFeedHistoryQuery<HistoryDailyTick>
{
  friend ou::tf::CIQFeedHistoryQuery<HistoryDailyTick>;
public:
  HistoryDailyTick(void);
  ~HistoryDailyTick(void);
protected:
private:
  void OnHistoryTickDataPoint( structTickDataPoint* pDP );
  void OnHistoryConnected( void );
  void OnHistoryDisconnected( void );
  void OnHistoryRequestDone( void );
  void OnHistoryRequestDone( void );
};

