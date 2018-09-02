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

#include <TFTrading/Position.h>
#include <TFTrading/DailyTradeTimeFrames.h>

class ManageStrategy: public ou::tf::DailyTradeTimeFrame<ManageStrategy> {
  friend ou::tf::DailyTradeTimeFrame<ManageStrategy>;
public:
  
  typedef ou::tf::Position::pPosition_t pPosition_t;
  
  typedef std::function<void(const ou::tf::iqfeed::MarketSymbol::TableRowDef&)> fSymbolDefinition_t;
  typedef std::function<void( const std::string&, fSymbolDefinition_t )> fLoadSymbolDefinitions_t;
  
  typedef std::function<pPosition_t( const std::string& )> fConstructPosition_t;
  
  ManageStrategy( const std::string& sUnderlying, const ou::tf::Bar& barPriorDaily, fConstructPosition_t fConstructPosition, fLoadSymbolDefinitions_t fLoadSymbolDefinitions );
  virtual ~ManageStrategy( );
  
  ou::tf::DatedDatum::volume_t CalcShareCount( double dblAmount );
  
protected:
private:
  
  typedef ou::tf::DatedDatum::volume_t volume_t;
  
  enum enumTradingState {
    TSWaitForEntry, TSMonitorLong, TSMonitorShort, TSNoMore
  };
  
  enumTradingState m_stateTrading;
  
  double m_dblOpen;
  
  const std::string m_sUnderlying;
  
  fLoadSymbolDefinitions_t m_fLoadSymbolDefinitions;  // load option symbols for given underlying
  fConstructPosition_t m_fConstructPosition;
  
  volume_t m_nUnderlyingSharesToTrade;
  volume_t m_nOptionContractsToTrade;
  
  const ou::tf::Bar& m_barPriorDaily;
  
  pPosition_t m_pPositionUnderlying;
  pPosition_t m_PositionPut_Current; // current active put, depending upon roll-downs
  pPosition_t m_pPositionPut_Previous;  // previous put if there was a roll-down
  
  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTrade( const ou::tf::Trade& trade );
  void HandleAfterRH( const ou::tf::Quote& quote );
  
  // one shot inherited states:
  void HandleBellHeard( void );
  void HandleCancel( void );
  void HandleGoNeutral( void );

  // Datum based inherited states:
  void HandleRHTrading( const ou::tf::Quote& quote );
};

#endif /* MANAGESTRATEGY_H */

