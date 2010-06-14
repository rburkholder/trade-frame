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
#include <string>

#include <LibTimeSeries/TimeSeries.h>
#include <LibInteractiveBrokers/IBTWS.h>
//#include <LibIQFeed/IQFeed.h>

class CProcess
//  : 
//  public CIQFeed<CProcess>
{
//  friend CIQFeed<CProcess>;
public:
  CProcess( void );
  ~CProcess( void );
  void IBConnect( void );
  void PlaceBuyOrder( void );
  void PlaceSellOrder( void );
  void IBDisconnect( void );

  void StartWatch( void );
  void StopWatch( void );
protected:

  void OnIQFeedConnected( void );
  void OnIQFeedDisConnected( void );
//  void OnIQFeedUpdateMessage( linebuffer_t* pBuffer, CIQFUpdateMessage* msg);
private:

  std::string m_sSymbolName;
  CIBTWS m_tws;
  bool m_bIBConnected;
  CIBSymbol* m_pSymbol;
  CQuotes m_vQuotes;
  CTrades m_vTrades;

  void HandleOnQuote( CIBSymbol::quote_t );
  void HandleOnTrade( CIBSymbol::trade_t );
};
