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

/* 
 * File:   ManageStrategy.h
 * Author: raymond@burkholder.net
 *
 * Created on August 26, 2018, 6:46 PM
 */

#ifndef MANAGESTRATEGY_H
#define MANAGESTRATEGY_H

#include <string>
#include <functional>

#include <TFIQFeed/MarketSymbol.h>

#include <TFTimeSeries/TimeSeries.h>
#include <TFTimeSeries/BarFactory.h>

#include <TFOptions/IvAtm.h>
#include <TFOptions/Option.h>

#include <TFTrading/Position.h>
#include <TFTrading/Portfolio.h>
#include <TFTrading/DailyTradeTimeFrames.h>

class ManageStrategy: public ou::tf::DailyTradeTimeFrame<ManageStrategy> {
  friend ou::tf::DailyTradeTimeFrame<ManageStrategy>;
public:
  
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::option::Option::pWatch_t pWatch_t;
  typedef ou::tf::option::Option::pOption_t pOption_t;
  
  typedef ou::tf::Position::pPosition_t pPosition_t;
  typedef ou::tf::Portfolio::pPortfolio_t pPortfolio_t;
  
  typedef std::function<void(const ou::tf::iqfeed::MarketSymbol::TableRowDef&)> fOptionDefinition_t;
  typedef std::function<void(const std::string&, fOptionDefinition_t)> fGatherOptionDefinitions_t;
  
  typedef std::function<void(pWatch_t)> fConstructedWatch_t;
  typedef std::function<void(pOption_t)> fConstructedOption_t;
  
  typedef std::function<void(const std::string&, fConstructedWatch_t)> fConstructWatch_t;
  typedef std::function<void(const std::string&, const pInstrument_t, fConstructedOption_t)> fConstructOption_t;  // source from IQFeed Symbol Name
  
  typedef std::function<pPosition_t( const ou::tf::Portfolio::idPortfolio_t, pWatch_t )> fConstructPosition_t;
  
  ManageStrategy( 
    const std::string& sUnderlying, const ou::tf::Bar& barPriorDaily, 
    pPortfolio_t,
    fGatherOptionDefinitions_t,
    fConstructWatch_t,
    fConstructOption_t,
    fConstructPosition_t
    );
  virtual ~ManageStrategy( );
  
  ou::tf::DatedDatum::volume_t CalcShareCount( double dblAmount );
  void SetFundsToTrade( double dblFundsToTrade ) { m_dblFundsToTrade = dblFundsToTrade; };
  bool& ToBeTraded( void ) { return m_bToBeTraded; };  // remote set/get
  void Start( void );
  void Stop( void );
  void SaveSeries( const std::string& sPrefix );
  
protected:
private:
  
  typedef ou::tf::DatedDatum::volume_t volume_t;
  
  std::string m_sUnderlying;
  
  bool m_bToBeTraded; // may not be used, other than as a flag for remote state manipulation
  double m_dblFundsToTrade;
  volume_t m_nSharesToTrade;

  double m_dblOpen;
  
  ou::tf::Bar m_barInfo;
  
  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;
  ou::tf::Bars m_bars;
  ou::tf::BarFactory m_bfTrades;

  enum enumTradingState {
    TSInitializing, TSWaitForFirstTrade, TSWaitForEntry, TSWaitForContract, TSMonitorLong, TSMonitorShort, TSNoMore
  };

  enumTradingState m_stateTrading;
  
  typedef std::map<boost::gregorian::date, ou::tf::option::IvAtm> mapChains_t;
  
  mapChains_t m_mapChains;
  
  fConstructWatch_t m_fConstructWatch;
  fConstructOption_t m_fConstructOption;
  fConstructPosition_t m_fConstructPosition;
  
  volume_t m_nUnderlyingSharesToTrade;
  volume_t m_nOptionContractsToTrade;
  
  const ou::tf::Bar& m_barPriorDaily;
  
  pPosition_t m_pPositionUnderlying;
  pPosition_t m_PositionPut_Current; // current active put, depending upon roll-downs
  //pPosition_t m_pPositionPut_Previous;  // previous put if there was a roll-down
  
  pPortfolio_t m_pPortfolioStrategy;
  
  void HandleQuoteUnderlying( const ou::tf::Quote& quote );
  void HandleTradeUnderlying( const ou::tf::Trade& trade );
  void HandleBarUnderlying( const ou::tf::Bar& bar );
  
  void HandleAfterRH( const ou::tf::Quote& quote );
  void HandleAfterRH( const ou::tf::Bar& bar );
  
  // one shot inherited states:
  void HandleBellHeard( void );
  void HandleCancel( void );
  void HandleGoNeutral( void );

  // Datum based inherited states:
  void HandleRHTrading( const ou::tf::Quote& quote );
  void HandleRHTrading( const ou::tf::Bar& bar );
};

#endif /* MANAGESTRATEGY_H */

