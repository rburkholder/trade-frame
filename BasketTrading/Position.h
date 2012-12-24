/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#pragma once

// AKA Strategy

#include <TFTrading/DailyTradeTimeFrames.h>
#include <TFTimeSeries/TimeSeries.h>
#include <TFTrading/Position.h>
#include <TFTimeSeries/BarFactory.h>

class ManagePosition: public ou::tf::DailyTradeTimeFrame<ManagePosition> {
  friend ou::tf::DailyTradeTimeFrame<ManagePosition>;
public:

  typedef ou::tf::CPosition::pPosition_t pPosition_t;

  ManagePosition( const std::string& sName, const ou::tf::Bar& bar );
  ~ManagePosition(void);

  void SetFundsToTrade( double dblFundsToTrade ) { m_dblFundsToTrade = dblFundsToTrade; };
  void SetPosition( pPosition_t pPosition ) { m_pPosition = pPosition; };
  void Start( void );
  void Stop( void );
  void SaveSeries( const std::string& sPrefix );
  ou::tf::DatedDatum::volume_t CalcShareCount( double dblAmount );
  double GetPrevousClose( void ) const { return m_barInfo.Close(); };
  bool& ToBeTraded( void ) { return m_bToBeTraded; };
protected:
private:

  typedef ou::tf::DatedDatum::volume_t volume_t;

  ou::tf::Bar m_barInfo;
  bool m_bToBeTraded;
  std::string m_sName;

  double m_dblFundsToTrade;
  volume_t m_nSharesToTrade;

  bool m_bCountBars;
  size_t m_nRHBars;

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;
  ou::tf::Bars m_bars;
  ou::tf::BarFactory m_bfTrades;

  enum enumTradingState {
    TSWaitForEntry, TSMonitorLong, TSMonitorShort, TSNoMore
  };
  size_t m_nAttempts;  // how many times entered - try a position up to three times?
  enumTradingState m_stateTrading;
  double m_dblOpen;
  double m_dblStop;

  pPosition_t m_pPosition;

  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTrade( const ou::tf::Trade& trade );
  void HandleOpen( const ou::tf::Trade& trade );
  void HandleBar( const ou::tf::Bar& bar );

  // one shot inherited states:
  void HandleBellHeard( void );
  void HandleCancel( void );
  void HandleGoNeutral( void );

  // Datum based inherited states:
  void HandleRHTrading( const ou::tf::Bar& bar );
  void HandleRHTrading( const ou::tf::Quote& quote );

};

