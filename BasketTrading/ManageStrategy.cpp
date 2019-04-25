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

// 2018/09/11
//   change ManagePortfolio:
//     watch opening price/quotes, include that in determination for which symbols to trade
//     may also watch number of option quotes delivered to ensure liquidity
//     then allocate the ToTrade from opening data rather than closing bars
//   output option greeks, and individual portfolio state
//   need a 'resume from over-night' as big moves can happen then, and can make the delta neutral profitable
//   enumerate watching/non-watching entries in the option engine
//   start the AtmIv at open and use to collect some of the above opening trade statistics
//     which also gets the execution contract id in place prior to the trade
//   delta should be >0.40 to enter
//   use option with >7 days to expiry?
//   add charts to watch AtmIv, options, underlying (steal from ComboTrading)
//   add a roll-down maneouver to keep delta close to 1
//   need option exit, and install a stop when in the profit zone, and put not longer necessary
//   need to get out of an option at $0.20, otherwise may not be a market
//     add a stop?  (do not worry when doing multi-day trades)
//

#include <algorithm>

#include "ManageStrategy.h"

ManageStrategy::ManageStrategy(
  const std::string& sUnderlying, const ou::tf::Bar& barPriorDaily,
  pPortfolio_t pPortfolioStrategy,
  fGatherOptionDefinitions_t fGatherOptionDefinitions,
  fConstructWatch_t fConstructWatch,
  fConstructOption_t ConstructOption,
  fConstructPosition_t fConstructPosition,
  fStartCalc_t fStartCalc,
  fStopCalc_t fStopCalc,
  fFirstTrade_t fFirstTrade,
  fBar_t fBar,
  pcdvStrategyData_t pcdvStrategyData
  )
: ou::tf::DailyTradeTimeFrame<ManageStrategy>(),
  m_dblOpen {},
  m_sUnderlying( sUnderlying ),
  m_barPriorDaily( barPriorDaily ),
  m_pPortfolioStrategy( pPortfolioStrategy ),
  m_fConstructWatch( fConstructWatch ),
  m_fConstructOption( ConstructOption ),
  m_fConstructPosition( fConstructPosition ),
  m_stateTrading( TSInitializing ),
  m_fStartCalc( fStartCalc ),
  m_fStopCalc( fStopCalc ),
  m_fFirstTrade( fFirstTrade ),
  m_fBar( fBar ),
  m_eTradeDirection( ETradeDirection::None ),
  m_bfTrades6Sec( 6 ),
  m_pcdvStrategyData( pcdvStrategyData )
{
  //std::cout << m_sUnderlying << " loading up ... " << std::endl;

  assert( nullptr != fGatherOptionDefinitions );
  assert( nullptr != m_fConstructWatch );
  assert( nullptr != m_fConstructOption );
  assert( nullptr != m_fConstructPosition );
  assert( nullptr != m_fStartCalc );
  assert( nullptr != m_fStopCalc );
  assert( nullptr != m_fFirstTrade );
  assert( nullptr != m_fBar );
  assert( pcdvStrategyData );
  
  m_cePvR1.SetColour( ou::Colour::Red );
  m_cePv.SetColour( ou::Colour::Green );
  m_cePvS1.SetColour( ou::Colour::Blue );

  pcdvStrategyData->Add( 0, &m_cePrice );
  pcdvStrategyData->Add( 1, &m_ceVolume );
  pcdvStrategyData->Add( 2, &m_ceProfitLoss );
  //m_dvChart.Add( 0, &m_cePvR1 );
  //m_dvChart.Add( 0, &m_cePv );
  //m_dvChart.Add( 0, &m_cePvS1 );
  //m_dvChart.Add( 0, &m_ceEma1 );
  //m_dvChart.Add( 0, &m_ceEma2 );
  //m_dvChart.Add( 0, &m_ceEma3 );
  //m_dvChart.Add( 0, &m_ceEma4 );

  m_bfTrades6Sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarTrades6Sec ) );

  try {

    m_fConstructWatch( m_sUnderlying,
      [this,fGatherOptionDefinitions](pWatch_t pWatch){

        //std::cout << m_sUnderlying << " watch arrived ... " << std::endl;

        m_pPositionUnderlying = m_fConstructPosition( m_pPortfolioStrategy->Id(), pWatch );
        assert( m_pPositionUnderlying );
        assert( m_pPositionUnderlying->GetWatch() );

        fGatherOptionDefinitions(
          m_pPositionUnderlying->GetInstrument()->GetInstrumentName(),
          [this](const ou::tf::iqfeed::MarketSymbol::TableRowDef& row){  // these are iqfeed based symbol names

            if ( ou::tf::iqfeed::MarketSymbol::IEOption == row.sc ) {
              boost::gregorian::date date( row.nYear, row.nMonth, row.nDay );

              mapChains_t::iterator iterChains;

              {
                using fConstructedOption_t = ou::tf::option::IvAtm::fConstructedOption_t;
                ou::tf::option::IvAtm ivAtm(
                        m_pPositionUnderlying->GetWatch(),
                      // IvAtm::fConstructOption_t
                        m_fConstructOption,
                      // IvAtm::fStartCalc_t
                        m_fStartCalc,
                      // IvAtm::fStopCalc_t
                        m_fStopCalc
                        );

                iterChains = m_mapChains.find( date );
                if ( m_mapChains.end() == iterChains ) {
                  iterChains = m_mapChains.insert(
                    m_mapChains.begin(),
                    mapChains_t::value_type( date, std::move( ivAtm ) )
                    );
                }
              }

              {
                ou::tf::option::IvAtm& ivAtm( iterChains->second );

                try {
                  switch ( row.eOptionSide ) {
                    case ou::tf::OptionSide::Call:
                      ivAtm.SetIQFeedNameCall( row.dblStrike, row.sSymbol );
                      break;
                    case ou::tf::OptionSide::Put:
                      ivAtm.SetIQFeedNamePut( row.dblStrike, row.sSymbol );
                      break;
                  }
                }
                catch ( std::runtime_error& e ) {
                  std::cout << "fGatherOptionDefinitions error" << std::endl;
                }
              }
            }
        });

        assert( 0 != m_mapChains.size() );

        //std::cout << m_sUnderlying << " watch done." << std::endl;

        m_bfTrades.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarUnderlying ) );
        //pWatch_t pWatch = m_pPositionUnderlying->GetWatch();
        pWatch->OnQuote.Add( MakeDelegate( this, &ManageStrategy::HandleQuoteUnderlying ) );
        pWatch->OnTrade.Add( MakeDelegate( this, &ManageStrategy::HandleTradeUnderlying ) );
    } );

  }
  catch (...) {
    std::cout << "*** " << "something wrong with " << m_sUnderlying << " creation." << std::endl;
  }

  //std::cout << m_sUnderlying << " loading done." << std::endl;

}

ManageStrategy::~ManageStrategy( ) {
  if ( m_pPositionUnderlying ) {
    pWatch_t pWatch = m_pPositionUnderlying->GetWatch();
    if ( pWatch ) {
      pWatch->OnQuote.Remove( MakeDelegate( this, &ManageStrategy::HandleQuoteUnderlying ) );
      pWatch->OnTrade.Remove( MakeDelegate( this, &ManageStrategy::HandleTradeUnderlying ) );
    }
  }
}

ou::tf::DatedDatum::volume_t ManageStrategy::CalcShareCount( double dblFunds ) const {
  volume_t nOptionContractsToTrade {};
  if ( 0.0 != m_dblOpen ) {
    nOptionContractsToTrade = ( (volume_t)std::floor( dblFunds / m_dblOpen ) )/ 100;
    //std::cout << m_sUnderlying << " funds on open: " << dblFunds << ", " << m_dblOpen << ", " << nOptionContractsToTrade << std::endl;
  }
  else {
    nOptionContractsToTrade = ( (volume_t)std::floor( dblFunds / m_barPriorDaily.Close() ) )/ 100;
    //std::cout << m_sUnderlying << " funds on bar close: " << dblFunds << ", " << m_barPriorDaily.Close() << ", " << nOptionContractsToTrade << std::endl;
  }
  
  volume_t nUnderlyingSharesToTrade = nOptionContractsToTrade * 100;  // round down to nearest 100
  //std::cout << m_sUnderlying << " funds: " << nOptionContractsToTrade << ", " << nUnderlyingSharesToTrade << std::endl;
  return nUnderlyingSharesToTrade;
}

void ManageStrategy::Start( ETradeDirection direction ) {

  m_eTradeDirection = direction;

  //std::cout << m_sUnderlying << " starting up ... " << std::endl;

  //assert( TSWaitForCalc == m_stateTrading );

  if ( nullptr == m_pPositionUnderlying.get() ) {
    // should be an unreachable test as this is tested in construction
    std::cout << m_sUnderlying << " doesn't have a position ***" << std::endl;
  }
  else {

    if ( 0 == m_dblFundsToTrade ) {
      std::cout << m_sUnderlying << " not started, no funds" << std::endl;
      m_stateTrading = TSNoMore;
    }
    else {
      m_nSharesToTrade = CalcShareCount( m_dblFundsToTrade );
//      if ( 200 > m_nSharesToTrade ) {
//        std::cout << m_sUnderlying << " not started, need room for 200 shares" << std::endl;
//        m_stateTrading = TSNoMore;
//      }
//      else {
        // can start with what was supplied
        std::cout << m_sUnderlying << " starting with $" << m_dblFundsToTrade << " for " << m_nSharesToTrade << " shares" << std::endl;
        m_stateTrading = TSWaitForEntry;
//      }
    }
  }

}

void ManageStrategy::Stop( void ) {
  HandleCancel();
  HandleGoNeutral();
  HandleAfterRH( m_quotes.last() ); // TODO: won't reflect closed orders properly
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
  m_trades.Append( trade );
  TimeTick( trade );
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
          //double strike {};
          try {
            switch ( m_eTradeDirection ) {
              case ETradeDirection::Up:
                //strike = iterChains->second.Put_OtmAtm( mid );  // may raise an exception
                //std::cout << m_sUnderlying << " quote midpoint " << mid << " calculated put strike " << strike << std::endl;
                break;
              case ETradeDirection::Down:
                //strike = iterChains->second.Call_OtmAtm( mid );  // may raise an exception
                //std::cout << m_sUnderlying << " quote midpoint " << mid << " calculated call strike " << strike << std::endl;
                break;
              case ETradeDirection::None:
                break;
            }
            
            // need to wait for contract info
            if ( 0 == m_pPositionUnderlying->GetInstrument()->GetContract() ) {
              std::cout << m_pPositionUnderlying->GetInstrument()->GetInstrumentName() << " has no contract" << std::endl;
              m_stateTrading = TSNoMore;
            }
            else {
              switch ( m_eTradeDirection ) {
                case ETradeDirection::Up:
                  //std::cout << m_pPositionUnderlying->GetInstrument()->GetInstrumentName() << ": building protective put." << std::endl;
                  //m_fConstructOption( iterChains->second.GetIQFeedNamePut( strike), m_pPositionUnderlying->GetInstrument(),
                  //  [this](pOption_t pOption){
                      //m_PositionOption_Current = m_fConstructPosition( m_pPortfolioStrategy->Id(), pOption );
                      m_pPositionUnderlying->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy,         m_nSharesToTrade );
                      std::cout << m_pPositionUnderlying->GetInstrument()->GetInstrumentName() << " " << quote.DateTime() << ": placing long " << m_nSharesToTrade << std::endl;
                      //m_PositionOption_Current->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 * ( ( m_nSharesToTrade - 100 ) / 100 ) );
                      m_stateTrading = TSMonitorLong;
                    //} );
                  break;
                case ETradeDirection::Down:
                  //std::cout << m_pPositionUnderlying->GetInstrument()->GetInstrumentName() << ": building protective call." << std::endl;
                  //m_fConstructOption( iterChains->second.GetIQFeedNameCall( strike), m_pPositionUnderlying->GetInstrument(),
                    //[this](pOption_t pOption){
                      //m_PositionOption_Current = m_fConstructPosition( m_pPortfolioStrategy->Id(), pOption );
                      m_pPositionUnderlying->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell,         m_nSharesToTrade );
                      std::cout << m_pPositionUnderlying->GetInstrument()->GetInstrumentName() << " " << quote.DateTime() << ": placing short " << m_nSharesToTrade << std::endl;
                      //m_PositionOption_Current->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy,  1 * ( ( m_nSharesToTrade - 100 ) / 100 ) );
                      m_stateTrading = TSMonitorShort;
                    //} );
                  break;
                case ETradeDirection::None:
                  break;
              }
            }
            m_stateTrading = TSWaitForContract;
          }
          catch ( std::runtime_error& e ) {
            std::cout << m_sUnderlying << "found no strike for mid-point " << mid << " on " << date << std::endl;
            m_stateTrading = TSNoMore;
          }
        }
        assert( TSWaitForEntry != m_stateTrading );
      }
      break;
    case TSWaitForContract:
      break;
    case TSMonitorLong:
      // TODO: monitor for delta changes, or being able to roll down to next strike
      // TODO: for over night, roll on one or two days.
      //    on up trend: drop puts once in profit zone, and set trailing stop
      //    on down trend:  keep rolling down each strike or selling on delta change of 1, and revert to up-trend logic
      break;
    case TSMonitorShort:
      break;
  }
}

void ManageStrategy::HandleRHTrading( const ou::tf::Trade& trade ) {
  switch ( m_stateTrading ) {
//    case TSWaitForFirstTrade:
    case TSInitializing:
      m_dblOpen = trade.Price();
      std::cout << m_sUnderlying << " " << trade.DateTime() << ": First Price: " << trade.Price() << std::endl;
      m_fFirstTrade( *this, trade );
      m_stateTrading = TSWaitForCalc;
      break;
    case TSWaitForCalc:
    case TSWaitForEntry:
    default:
      break;
  }
}

void ManageStrategy::HandleRHTrading( const ou::tf::Bar& bar ) {
}

void ManageStrategy::HandleCancel( void ) {
  switch ( m_stateTrading ) {
    case TSNoMore:
      break;
    default:
      std::cout << m_sUnderlying << " cancel" << std::endl;
      if ( nullptr != m_pPositionUnderlying.get() ) m_pPositionUnderlying->CancelOrders();
      if ( nullptr != m_PositionOption_Current.get() ) m_PositionOption_Current->CancelOrders();
      break;
  }
}

void ManageStrategy::HandleGoNeutral( void ) {
  switch ( m_stateTrading ) {
    case TSNoMore:
      break;
    default:
      std::cout << m_sUnderlying << " close" << std::endl;
      if ( nullptr != m_pPositionUnderlying.get() ) m_pPositionUnderlying->ClosePosition();
      if ( nullptr != m_PositionOption_Current.get() ) m_PositionOption_Current->ClosePosition();
      break;
  }
}

void ManageStrategy::HandleAfterRH( const ou::tf::Quote& quote ) {
  switch ( m_stateTrading ) {
    case TSNoMore:
      break;
    default:
      if ( nullptr != m_pPositionUnderlying.get() ) std::cout << m_sUnderlying << " close results underlying " << *m_pPositionUnderlying << std::endl;
      if ( nullptr != m_PositionOption_Current.get() ) std::cout << m_sUnderlying << " close results option " << *m_PositionOption_Current << std::endl;
      m_stateTrading = TSNoMore;
      break;
  }
  // TODO: need to set a state to do this once, rather than the TSNoMore kludge?
}

void ManageStrategy::HandleAfterRH( const ou::tf::Trade& trade ) {
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
  m_fBar( *this, bar );
}

void ManageStrategy::SaveSeries( const std::string& sPrefix ) {
  if ( nullptr != m_pPositionUnderlying.get() ) {
    m_pPositionUnderlying->GetWatch()->SaveSeries( sPrefix );
  }
  if ( nullptr != m_PositionOption_Current.get() ) {
    m_PositionOption_Current->GetWatch()->SaveSeries( sPrefix );
  }
}

void ManageStrategy::HandleBarTrades6Sec( const ou::tf::Bar& bar ) {
  m_cePrice.AppendBar( bar );
  m_ceVolume.Append( bar );
  
  double dblUnRealized;
  double dblRealized;
  double dblCommissionsPaid;
  double dblTotal;
  m_pPortfolioStrategy->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );
  m_ceProfitLoss.Append( bar.DateTime(), dblTotal );
}