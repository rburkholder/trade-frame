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

class Position: public ou::tf::DailyTradeTimeFrame<Position> {
public:

  typedef ou::tf::CPosition::pPosition_t pPosition_t;

  Position( const std::string& sName, const ou::tf::Bar& bar );
  ~Position(void);

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

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;
  ou::tf::Bars m_bars;

  pPosition_t m_pPosition;

  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTrade( const ou::tf::Trade& trade );
  void HandleOpen( const ou::tf::Trade& trade );

};

