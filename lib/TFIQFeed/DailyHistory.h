/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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

/*
 * File:    DailyHistory.h
 * Author:  raymond@burkholder.net
 * Project: lib/TFIQFeed
 * Created: August 7, 2021, 19:06
 */

#pragma once

#include <string>
#include <functional>

#include <TFIQFeed/HistoryQuery.h>

#include <TFTimeSeries/DatedDatum.h>

namespace ou {
namespace tf {
namespace iqfeed {

class DailyHistory: HistoryQuery<DailyHistory> {
  friend HistoryQuery<DailyHistory>;
public:

  using fConnected_t = std::function<void(void)>;
  using fBar_t = std::function<void(const ou::tf::Bar&)>;
  using fDone_t = std::function<void()>;

  DailyHistory( fConnected_t&&, fBar_t&&, fDone_t&& );
  virtual ~DailyHistory() {}
  void Connect();
  void Request( const std::string& sSymbol, unsigned int nDays );
  void Disconnect();
protected:
  void OnHistoryConnected();
  void OnHistoryEndOfDayData( EndOfDay* pDP );
  void OnHistoryRequestDone();
  void OnHistorySendDone();
  void OnHistoryError( size_t e );
private:
  fConnected_t m_fConnected;
  fBar_t m_fBar;
  fDone_t m_fDone;
};

} // namespace iqfeed
} // namespace tf
} // namespace ou
