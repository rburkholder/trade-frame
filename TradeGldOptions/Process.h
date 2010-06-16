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

#include <vector>

#include <LibTimeSeries/DatedDatum.h>

#include <LibIQFeed/IQFeedHistoryQuery.h>  // seems to be a header ordering dependancy
#include <LibIQFeed/IQFeedProvider.h>

#include <LibInteractiveBrokers/IBTWS.h>

class CProcess: public CIQFeedHistoryQuery<CProcess>
{
  friend CIQFeedHistoryQuery<CProcess>;
public:
  CProcess(void);
  ~CProcess(void);

  void IBConnect( void );
  void IBDisconnect( void );

  void IQFeedConnect( void );
  void IQFeedDisconnect( void );

protected:

  void OnHistoryConnected( void );
  void OnHistorySummaryData( structSummary* pDP );
  void OnHistoryRequestDone( void );
  void OnHistoryDisconnected( void ) {};

private:

  std::string m_sSymbolName;

  CIQFeedProvider m_iqfeed;
  bool m_bIQFeedConnected;

  CIBTWS m_tws;
  bool m_bIBConnected;
  int m_reqId;

  CBar m_Bar;  // keep pointer for when data arrives

  struct structOptionInfo {
    char Side;
    double Strike;
    double delta;
    double gamma;
  };

  struct structLookup {
    double ix1;  // vector sorted by this value
    size_t ix2;  // index into other vectors
  };

  std::vector<double> m_vCrossOverPoints;
  std::vector<structOptionInfo> m_vOptionInfo;  // structure of option info
  std::vector<structLookup> m_vDeltas;  // ix1(deltas) sorted hi->low
  std::vector<structLookup> m_vGammas;  // ix1(gammas) sorted hi->low

  void HandleOnIBConnected( int );
  void HandleOnIBDisconnected( int );

  void HandleOnIQFeedConnected( int );
  void HandleOnIQFeedDisconnected( int );

};
