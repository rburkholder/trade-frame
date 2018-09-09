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
 * File:   ManageStrategy.cpp
 * Author: raymond@burkholder.net
 * 
 * Created on August 26, 2018, 6:46 PM
 */

// 2018/09/04: need to start looking for options which cross strikes on a regular basis both ways
//    count up strikes vs down strikes with each daily bar
//  need a way to enter for combination of shorts and longs to can play market in both directions
//    may be use the 10% logic, or 3 day trend bar logic
//  add live charting for portfolio, position, and instruments
//  run implied atm volatility for underlying

#include <algorithm>

#include "ManageStrategy.h"

ManageStrategy::ManageStrategy( 
  const std::string& sUnderlying, const ou::tf::Bar& barPriorDaily, 
  pPortfolio_t pPortfolioStrategy,
  fGatherOptionDefinitions_t fGatherOptionDefinitions,
  fConstructPositionUnderlying_t fConstructPositionUnderlying,
  fConstructPositionOption_t fConstructPositionOption
  )
: ou::tf::DailyTradeTimeFrame<ManageStrategy>(),
  m_sUnderlying( sUnderlying ),
  m_nUnderlyingSharesToTrade {},
  m_barPriorDaily( barPriorDaily ), 
  m_pPortfolioStrategy( pPortfolioStrategy ),
  m_fConstructPositionUnderlying( fConstructPositionUnderlying ), 
  m_fConstructPositionOption( fConstructPositionOption ), 
  m_stateTrading( TSInitializing )
{ 
  assert( nullptr != m_fConstructPositionUnderlying );
  assert( nullptr != fGatherOptionDefinitions );
  assert( nullptr != m_fConstructPositionOption );
  
  m_pPositionUnderlying = m_fConstructPositionUnderlying( sUnderlying );
  assert( nullptr != m_pPositionUnderlying->GetWatch().get() );
  
  m_pPortfolioStrategy->AddPosition( sUnderlying, m_pPositionUnderlying );
  
  fGatherOptionDefinitions( sUnderlying, [this](const ou::tf::iqfeed::MarketSymbol::TableRowDef& row){  // these are iqfeed based symbol names
    assert( ou::tf::iqfeed::MarketSymbol::IEOption == row.sc );
    boost::gregorian::date date( row.nYear, row.nMonth, row.nDay );  // is nDay non-zero?
    
    mapChains_t::iterator iterChains = m_mapChains.find( date );
    if ( m_mapChains.end() == iterChains ) {
      iterChains = m_mapChains.insert( m_mapChains.begin(), mapChains_t::value_type( date, mapChain_t() ) );
    }
    mapChain_t& mapChain( iterChains->second );
    
    mapChain_t::iterator iterChain = mapChain.find( row.dblStrike );
    if ( mapChain.end() == iterChain ) {
      iterChain = mapChain.insert( mapChain.begin(), mapChain_t::value_type( row.dblStrike, OptionsAtStrike() ) );
    }
    OptionsAtStrike& oas( iterChain->second );
    
    switch ( row.eOptionSide ) {
      case ou::tf::OptionSide::Call:
        oas.sCall = row.sSymbol;
        break;
      case ou::tf::OptionSide::Put:
        oas.sPut = row.sSymbol;
        break;
    }
  });
  
  assert( 0 != m_mapChains.size() );
  
  m_bfTrades.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarUnderlying ) );
  
}

ManageStrategy::~ManageStrategy( ) { 
}

ou::tf::DatedDatum::volume_t ManageStrategy::CalcShareCount( double dblFunds ) {
  m_nOptionContractsToTrade = ( (volume_t)( dblFunds / m_barPriorDaily.Close() ) )/ 100;
  m_nUnderlyingSharesToTrade = m_nOptionContractsToTrade * 100;  // round down to nearest 100
  return m_nUnderlyingSharesToTrade;
}

void ManageStrategy::Start( void ) {
  
  assert( TSInitializing == m_stateTrading );
  
  pWatch_t pWatch;
  pWatch = m_pPositionUnderlying->GetWatch();
  pWatch->OnQuote.Add( MakeDelegate( this, &ManageStrategy::HandleQuoteUnderlying ) );
  pWatch->OnTrade.Add( MakeDelegate( this, &ManageStrategy::HandleTradeUnderlying ) );
  
  if ( 0 == m_dblFundsToTrade ) {
    std::cout << m_sUnderlying << " not started, no funds" << std::endl;
    m_stateTrading = TSNoMore;
  }
  else {
    m_nSharesToTrade = CalcShareCount( m_dblFundsToTrade );
    if ( 200 > m_nSharesToTrade ) {
      std::cout << m_sUnderlying << " not started, need room for 200 shares" << std::endl;
      m_stateTrading = TSNoMore;
    }
    else {
      // can start with what was supplied
      std::cout << m_sUnderlying << " starting with $" << m_dblFundsToTrade << " for " << m_nSharesToTrade << " shares" << std::endl;
      m_stateTrading = TSWaitForFirstTrade;
    }
  }
}

void ManageStrategy::Stop( void ) {
  pWatch_t pWatch;
  pWatch = m_pPositionUnderlying->GetWatch();
  pWatch->OnQuote.Remove( MakeDelegate( this, &ManageStrategy::HandleQuoteUnderlying ) );
  pWatch->OnTrade.Remove( MakeDelegate( this, &ManageStrategy::HandleTradeUnderlying ) );
}

void ManageStrategy::HandleBellHeard( void ) {
  
}

void ManageStrategy::HandleQuoteUnderlying( const ou::tf::Quote& quote ) {
  if ( quote.IsValid() ) {
    m_quotes.Append( quote );
    TimeTick( quote );
  }
}

void ManageStrategy::HandleTradeUnderlying( const ou::tf::Trade& trade ) {
  switch ( m_stateTrading ) {
    case TSWaitForFirstTrade:
      m_dblOpen = trade.Price();
      std::cout << m_sUnderlying << " " << trade.DateTime() << ": First Price: " << trade.Price() << std::endl;
      m_stateTrading = TSWaitForEntry;
      break;
    case TSWaitForEntry:
    default:
      break;
  }
  m_trades.Append( trade );
  m_bfTrades.Add( trade );
}

void ManageStrategy::HandleRHTrading( const ou::tf::Quote& quote ) {
  switch ( m_stateTrading ) {
    case TSWaitForEntry: 
      {
        boost::gregorian::date date( quote.DateTime().date() );
        mapChains_t::iterator iterChains = std::find_if( m_mapChains.begin(), m_mapChains.end(), 
          [date](const mapChains_t::value_type& vt)->bool{
            return date < vt.first;  // TODO: at least one or two days difference?
        } );
        if ( m_mapChains.end() == iterChains ) {
          std::cout << m_sUnderlying << " found no chain for " << date << std::endl;
          m_stateTrading = TSNoMore;
        }
        else {
          double mid = quote.Midpoint();
          mapChain_t& mapChain( iterChains->second );
          mapChain_t::reverse_iterator iterChain = std::find_if( mapChain.rbegin(), mapChain.rend(), 
            [mid, date](const mapChain_t::value_type& vt)->bool{
              return vt.first < mid;
          });
          if ( mapChain.rend() == iterChain ) {
            std::cout << m_sUnderlying << "found no strike for mid-point " << mid << " on " << date << std::endl;
            m_stateTrading = TSNoMore;
          }  
          else {
            OptionsAtStrike& oas( iterChain->second );
            std::cout << m_sUnderlying << ", quote midpoint " << mid << " starts with " << iterChain->first << " put of " << date << std::endl;
            m_PositionPut_Current = m_fConstructPositionOption( m_pPositionUnderlying->GetInstrument(), oas.sPut );
            m_pPortfolioStrategy->AddPosition( m_PositionPut_Current->GetInstrument()->GetInstrumentName(), m_PositionPut_Current );
            m_pPositionUnderlying->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, m_nSharesToTrade - 100 );
            m_PositionPut_Current->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 2 );
            m_stateTrading = TSMonitorLong;
          }
        }
      }
      break;
    case TSMonitorLong:
      // TODO: monitor for delta changes, or being able to roll down to next strike
      // TODO: for over night, roll on one or two days.
      //    on up trend: drop puts once in profit zone, and set trailing stop
      //    on down trend:  keep rolling down each strike or selling on delta change of 1, and revert to up-trend logic
      break;
  }
}

void ManageStrategy::HandleRHTrading( const ou::tf::Bar& bar ) {
}

void ManageStrategy::HandleCancel( void ) {
  std::cout << m_sUnderlying << " cancel" << std::endl;
  m_pPositionUnderlying->CancelOrders();
  m_PositionPut_Current->CancelOrders();
}

void ManageStrategy::HandleGoNeutral( void ) {
  std::cout << m_sUnderlying << " close" << std::endl;
  m_pPositionUnderlying->ClosePosition();
  m_PositionPut_Current->ClosePosition();
}

void ManageStrategy::HandleAfterRH( const ou::tf::Quote& quote ) {
  std::cout << m_sUnderlying << " close results underlying " << *m_pPositionUnderlying << std::endl;
  std::cout << m_sUnderlying << " close results option put " << *m_PositionPut_Current << std::endl;
  // need to set a state to do this once
}

void ManageStrategy::HandleAfterRH( const ou::tf::Bar& bar ) {
  //std::cout << m_sUnderlying << " close results " << *m_pPositionUnderlying << std::endl;
  // need to set a state to do this once
}

void ManageStrategy::HandleBarUnderlying( const ou::tf::Bar& bar ) {
  m_bars.Append( bar );
  //m_nRHBars++;
  // *** step in to state to test last three bars to see if trade should be entered
  TimeTick( bar );
}

void ManageStrategy::SaveSeries( const std::string& sPrefix ) {
  m_pPositionUnderlying->GetWatch()->SaveSeries( sPrefix );
  m_PositionPut_Current->GetWatch()->SaveSeries( sPrefix );
}