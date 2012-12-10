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

#include <TFInteractiveBrokers/IBTWS.h>
#include <TFIQFeed/IQFeedProvider.h>
#include <TFSimulation/SimulationProvider.h>

#include <TFTrading/DailyTradeTimeFrames.h>
#include <TFTimeSeries/TimeSeries.h>

class Position: public ou::tf::DailyTradeTimeFrame<Position> {
public:
  Position( const std::string& sName, const ou::tf::Bar& bar );
  ~Position(void);
  void Start( double dblFundsToTrade );
  void Stop( void );
  void SaveSeries( const std::string& sPrefix );
  ou::tf::DatedDatum::volume_t CalcShareCount( double dblAmount );
  double GetPrevousClose( void ) const { return m_barInfo.Close(); };
  bool& ToBeTraded( void ) { return m_bToBeTraded; };
protected:
private:

  typedef ou::tf::DatedDatum::volume_t volume_t;

  typedef ou::tf::IBTWS::pProvider_t pProviderIBTWS_t;
  typedef ou::tf::CIQFeedProvider::pProvider_t pProviderIQFeed_t;
  typedef ou::tf::SimulationProvider::pProvider_t pProviderSim_t;  

  typedef ou::tf::IBTWS::pInstrument_t pInstrument_t;

  ou::tf::Bar m_barInfo;
  bool m_bToBeTraded;
  std::string m_sName;

  double m_dblFundsToTrade;
  volume_t m_nSharesToTrade;

  pProviderIBTWS_t m_pIB;
  pProviderIQFeed_t m_pIQ;
  pProviderSim_t n_pSim;

  pInstrument_t m_pInstrument;

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;
  ou::tf::Bars m_bars;

  void HandleIBContractDetails( const ou::tf::IBTWS::ContractDetails& details, pInstrument_t& pInstrument );
  void HandleIBContractDetailsDone( void );

  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTrade( const ou::tf::Trade& trade );
  void HandleOpen( const ou::tf::Trade& trade );

};

