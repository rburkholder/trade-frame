/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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
 * File:    DailyHistory.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFBitsNPieces
 * Created: May 17, 2022 16:24
 */

#include <functional>

#include <OUCharting/ChartEntryMark.h>

#include <TFTimeSeries/TimeSeries.h>

#include <TFIQFeed/BarHistory.h>

class DailyHistory {
public:

  using fAddMark_t = std::function<void(double,const std::string&)>;
  using fDone_t = std::function<void(const ou::tf::Bars&)>;

  ~DailyHistory();

  void Load( const std::string& sIQFeedSymbolName, fAddMark_t&&, fDone_t&& );
  void Close();

protected:
private:
  using pBarHistory_t = ou::tf::iqfeed::BarHistory::pBarHistory_t;
  pBarHistory_t m_pBarHistory;
  ou::tf::Bars m_barsHistory;
};
