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

// 2019/05/03 adjust trading for first hour and last hour of trading day?

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
  m_bfTrades01Sec( 1 ),
  m_bfTrades06Sec( 6 ),
//  m_bfTrades60Sec( 60 ),
  m_cntUpReturn {}, m_cntDnReturn {},
  m_stateEma( EmaState::EmaUnstable ),
  m_pcdvStrategyData( pcdvStrategyData ),
  m_ceShortEntries( ou::ChartEntryShape::EShort, ou::Colour::Red ),
  m_ceLongEntries( ou::ChartEntryShape::ELong, ou::Colour::Blue ),
  m_ceShortFills( ou::ChartEntryShape::EFillShort, ou::Colour::Red ),
  m_ceLongFills( ou::ChartEntryShape::EFillLong, ou::Colour::Blue ),
  m_ceShortExits( ou::ChartEntryShape::EShortStop, ou::Colour::Red ),
  m_ceLongExits( ou::ChartEntryShape::ELongStop, ou::Colour::Blue )
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

  //m_rBarDirection[ 0 ] = EBarDirection::None;
  //m_rBarDirection[ 1 ] = EBarDirection::None;
  //m_rBarDirection[ 2 ] = EBarDirection::None;

  pcdvStrategyData->SetNames( "Charts", m_sUnderlying );

  m_cePrice.SetName( "Price" );
  m_ceVolume.SetName( "Volume" );
  m_ceProfitLoss.SetName( "Profit/Loss" );
  m_ceUpReturn.SetName( "Up Return" );
  m_ceDnReturn.SetName( "Dn Return" );
  
  pcdvStrategyData->Add( 0, &m_cePrice );
  pcdvStrategyData->Add( 0, &m_cePivots );
  pcdvStrategyData->Add( 1, &m_ceVolume );
  pcdvStrategyData->Add( 2, &m_ceProfitLoss );
  pcdvStrategyData->Add( 4, &m_ceUpReturn );
  pcdvStrategyData->Add( 4, &m_ceDnReturn );

  pcdvStrategyData->Add( 0, &m_ceShortEntries );
  pcdvStrategyData->Add( 0, &m_ceLongEntries );
  pcdvStrategyData->Add( 0, &m_ceShortFills );
  pcdvStrategyData->Add( 0, &m_ceLongFills );
  pcdvStrategyData->Add( 0, &m_ceShortExits );
  pcdvStrategyData->Add( 0, &m_ceLongExits );

  m_bfTrades01Sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarTrades01Sec ) );
  m_bfTrades06Sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarTrades06Sec ) );
//  m_bfTrades60Sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarTrades60Sec ) );

  try {

    //std::cout << "Construct Watch: " << m_sUnderlying << std::endl;

    m_fConstructWatch(
      m_sUnderlying,
      [this,fGatherOptionDefinitions](pWatch_t pWatchUnderlying){

        //std::cout << m_sUnderlying << " watch arrived ... " << std::endl;

        // create a position with the watch
        m_pPositionUnderlying = m_fConstructPosition( m_pPortfolioStrategy->Id(), pWatchUnderlying );
        assert( m_pPositionUnderlying );
        assert( m_pPositionUnderlying->GetWatch() );

        // collect option chains for the underlying
        fGatherOptionDefinitions(
          m_pPositionUnderlying->GetInstrument()->GetInstrumentName(),
          [this](const ou::tf::iqfeed::MarketSymbol::TableRowDef& row){  // these are iqfeed based symbol names

            if ( ou::tf::iqfeed::MarketSymbol::IEOption == row.sc ) {
              boost::gregorian::date date( row.nYear, row.nMonth, row.nDay );

              mapChains_t::iterator iterChains;

              {
                ou::tf::option::IvAtm ivAtm(
                        m_pPositionUnderlying->GetWatch(),
                      // IvAtm::fConstructOption_t
                        m_fConstructOption,
                      // IvAtm::fStartCalc_t
                        m_fStartCalc,
                      // IvAtm::fStopCalc_t
                        m_fStopCalc
                        );

                iterChains = m_mapChains.find( date ); // see if expiry date exists
                if ( m_mapChains.end() == iterChains ) { // insert new expiry set if not
                  iterChains = m_mapChains.insert(
                    m_mapChains.begin(),
                    mapChains_t::value_type( date, std::move( ivAtm ) )
                    );
                }
              }

              {
                ou::tf::option::IvAtm& ivAtm( iterChains->second );

                //std::cout << "  option: " << row.sSymbol << std::endl;

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
                  std::cout << "ManageStrategy::fGatherOptionDefinitions error" << std::endl;
                }
              }
            }
        });

        assert( 0 != m_mapChains.size() );

        //std::cout << m_sUnderlying << " watch done." << std::endl;

        //pWatch_t pWatch = m_pPositionUnderlying->GetWatch();
        pWatchUnderlying->OnQuote.Add( MakeDelegate( this, &ManageStrategy::HandleQuoteUnderlying ) );
        pWatchUnderlying->OnTrade.Add( MakeDelegate( this, &ManageStrategy::HandleTradeUnderlying ) );
    } ); // m_fConstructWatch

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

void ManageStrategy::SetPivots( double dblS1, double dblPV, double dblR1 ) {
  m_cePivots.AddMark( dblR1, ou::Colour::Blue, "R1" );
  m_cePivots.AddMark( dblPV, ou::Colour::Green, "PV" );
  m_cePivots.AddMark( dblS1, ou::Colour::Red, "S1" );
}

ou::tf::DatedDatum::volume_t ManageStrategy::CalcShareCount( double dblFunds ) const {
  volume_t nOptionContractsToTrade {};
  if ( 0.0 != m_dblOpen ) {
    nOptionContractsToTrade = ( (volume_t)std::floor( dblFunds / m_dblOpen ) )/ 100;
    std::cout << m_sUnderlying << " funds on open: " << dblFunds << ", " << m_dblOpen << ", " << nOptionContractsToTrade << std::endl;
  }
  else {
    nOptionContractsToTrade = ( (volume_t)std::floor( dblFunds / m_barPriorDaily.Close() ) )/ 100;
    std::cout << m_sUnderlying << " funds on bar close: " << dblFunds << ", " << m_barPriorDaily.Close() << ", " << nOptionContractsToTrade << std::endl;
  }
  
  volume_t nUnderlyingSharesToTrade = nOptionContractsToTrade * 100;  // round down to nearest 100
  std::cout << m_sUnderlying << " funds: " << nOptionContractsToTrade << ", " << nUnderlyingSharesToTrade << std::endl;
  return nUnderlyingSharesToTrade;
}

//void ManageStrategy::Start( ETradeDirection direction ) {
void ManageStrategy::Start(  ) {

  //m_eTradeDirection = direction;

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
    m_QuoteLatest = quote;
    m_quotes.Append( quote );
    TimeTick( quote );
  }
}

void ManageStrategy::HandleTradeUnderlying( const ou::tf::Trade& trade ) {
  if ( trade.Price() > m_TradeLatest.Price() ) m_cntUpReturn++;
  if ( trade.Price() < m_TradeLatest.Price() ) m_cntDnReturn++;
  m_trades.Append( trade );
  m_bfTrades01Sec.Add( trade );
  m_bfTrades06Sec.Add( trade );
//  m_bfTrades60Sec.Add( trade );
  TimeTick( trade );
  m_TradeLatest = trade; // allow previous one to be used till last moment
}

void ManageStrategy::HandleRHTrading( const ou::tf::Quote& quote ) {
  switch ( m_stateTrading ) {
    case TSWaitForEntry:
      if ( true ) {
        if ( true ) {
          double mid = quote.Midpoint();
          try {
            //switch ( m_eTradeDirection ) {
            //  case ETradeDirection::Up:
                //strike = iterChains->second.Put_OtmAtm( mid );  // may raise an exception
                //std::cout << m_sUnderlying << " quote midpoint " << mid << " calculated put strike " << strike << std::endl;
            //    break;
            //  case ETradeDirection::Down:
                //strike = iterChains->second.Call_OtmAtm( mid );  // may raise an exception
                //std::cout << m_sUnderlying << " quote midpoint " << mid << " calculated call strike " << strike << std::endl;
            //    break;
            //  case ETradeDirection::None:
            //    break;
            //}

            double strikePut = m_iterChainExpiryInUse->second.Put_Atm( mid );  // may raise an exception (on or nearest strike)
            double strikeCall = m_iterChainExpiryInUse->second.Call_Atm( mid );  // may raise an exception (on or nearest strike)

            // todo:  several steps 1) build watch variables, confirm spreads are small,
            //           enter as basket trade with limit at mid, every six seconds adust limit price by 0.01 until executed

            if ( strikePut != strikeCall ) {
              std::cout << m_sUnderlying << ": atm strike not matching - midpoint=" << mid << ",put=" << strikePut << ",call=" << strikeCall << std::endl;
              m_stateTrading = TSNoMore;
            }
            else {
              double strike = strikePut;
              // need to wait for contract info
              if ( 0 == m_pPositionUnderlying->GetInstrument()->GetContract() ) {
                std::cout << m_pPositionUnderlying->GetInstrument()->GetInstrumentName() << " has no contract" << std::endl;
                m_stateTrading = TSNoMore;
              }
              else {
                std::cout << m_sUnderlying << ": constructing options for straddle -> quote=" << mid << ",strike=" << strike << std::endl;
                m_fConstructOption( m_iterChainExpiryInUse->second.GetIQFeedNamePut( strike), m_pPositionUnderlying->GetInstrument(),
                  [this](pOption_t pOption){
                    m_pPositionPut = m_fConstructPosition( m_pPortfolioStrategy->Id(), pOption );
                    //m_pPositionUnderlying->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy,         m_nSharesToTrade );
                    //std::cout << m_pPositionUnderlying->GetInstrument()->GetInstrumentName() << " " << quote.DateTime() << ": placing long " << m_nSharesToTrade << std::endl;
                    //m_pPositionPut->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 * ( ( m_nSharesToTrade - 100 ) / 100 ) );
                    m_pPositionPut->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
                    m_stateTrading = TSMonitorStraddle;
                  } );
                m_fConstructOption( m_iterChainExpiryInUse->second.GetIQFeedNameCall( strike), m_pPositionUnderlying->GetInstrument(),
                  [this](pOption_t pOption){
                    m_pPositionCall = m_fConstructPosition( m_pPortfolioStrategy->Id(), pOption );
                    //m_pPositionUnderlying->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell,         m_nSharesToTrade );
                    //std::cout << m_pPositionUnderlying->GetInstrument()->GetInstrumentName() << " " << quote.DateTime() << ": placing short " << m_nSharesToTrade << std::endl;
                    //m_pPositionCall->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy,  1 * ( ( m_nSharesToTrade - 100 ) / 100 ) );
                    m_pPositionCall->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
                    m_stateTrading = TSMonitorStraddle;
                  } );
              } // else
              m_stateTrading = TSWaitForContract;
            } // else
          }
          catch ( std::runtime_error& e ) {
            std::cout << m_sUnderlying << "found no strike for mid-point " << mid << " on " << quote.DateTime().date() << std::endl;
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
    case TSInitializing: {
      m_dblOpen = trade.Price();
      std::cout << m_sUnderlying << " " << trade.DateTime() << ": First Price: " << trade.Price() << std::endl;
      m_fFirstTrade( *this, trade );

      boost::gregorian::date date( trade.DateTime().date() );
      m_iterChainExpiryInUse = std::find_if( m_mapChains.begin(), m_mapChains.end(),
        [date](const mapChains_t::value_type& vt)->bool{
          return date < vt.first;  // first chain where trading date less than expiry date TODO: at least one or two days difference?
      } );

      if ( m_mapChains.end() == m_iterChainExpiryInUse ) {
        std::cout << m_sUnderlying << " found no chain for " << date << ", trading disabled" << std::endl;
        m_stateTrading = TSNoMore;
      }
      else {
        m_stateTrading = TSWaitForCalc;
      }
      }
      break;
    case TSWaitForCalc:
      break;
    case TSWaitForEntry:
      break;
    case TSMonitorStraddle:
      break;
    case TSMonitorLong: {
//      pEMA_t& pEMA( m_vEMA.back() );
//      if ( trade.Price() < pEMA->dblEmaLatest ) {
//        m_pPositionUnderlying->ClosePosition( ou::tf::OrderType::Market );
//        m_ceLongExits.AddLabel( trade.DateTime(), trade.Price(), "Stop" );
//        std::cout << m_sUnderlying << " closing long" << std::endl;
//        m_stateTrading = TSWaitForEntry;
//      }
      }
      break;
    case TSMonitorShort: {
//      pEMA_t& pEMA( m_vEMA.back() );
//      if ( trade.Price() > pEMA->dblEmaLatest ) {
//        m_pPositionUnderlying->ClosePosition( ou::tf::OrderType::Market );
//        m_ceShortExits.AddLabel( trade.DateTime(), trade.Price(), "Stop" );
//        std::cout << m_sUnderlying << " closing short" << std::endl;
//        m_stateTrading = TSWaitForEntry;
//      }
      }
      break;
    default:
      break;
  }
}

void ManageStrategy::HandleRHTrading( const ou::tf::Bar& bar ) { // one second bars
  switch ( m_stateTrading ) {
    case TSWaitForEntry: if ( false ) {
      bool bFirstFound( false );
      double dblPrevious {};
      bool bAllRising( true );
      bool bAllFalling( true );
      std::for_each(  // calculate relative ema
        m_vEMA.begin(), m_vEMA.end(),
        [&,this]( pEMA_t& p ){
          if ( bFirstFound ) {
            double dblLatest = p->dblEmaLatest;
            bAllRising  &= dblLatest < dblPrevious;
            bAllFalling &= dblLatest > dblPrevious;
            dblPrevious = dblLatest;
          }
          else {
            dblPrevious = p->dblEmaLatest;
            bFirstFound = true;
          }
//          bAllRising  &= EMA::State::rising == p->state;
//          bAllFalling &= EMA::State::falling == p->state;
      } );
      // need three consecutive bars in the trending direction
//      bAllRising &= ( EBarDirection::Up == m_rBarDirection[ 0 ] );
//      bAllRising &= ( EBarDirection::Up == m_rBarDirection[ 1 ] );
//      bAllRising &= ( EBarDirection::Up == m_rBarDirection[ 2 ] );
//      bAllFalling &= ( EBarDirection::Down == m_rBarDirection[ 0 ] );
//      bAllFalling &= ( EBarDirection::Down == m_rBarDirection[ 1 ] );
//      bAllFalling &= ( EBarDirection::Down == m_rBarDirection[ 2 ] );
      static const size_t nConfirmationIntervalsPreload( 21 );
      if ( bAllRising && bAllFalling ) { // special message for questionable result
        std::cout << m_sUnderlying << ": bAllRising && bAllFalling" << std::endl;
        m_stateEma = EmaState::EmaUnstable;
        m_nConfirmationIntervals = nConfirmationIntervalsPreload;
      }
      else {
        if ( !bAllRising && !bAllFalling ) {
          m_stateEma = EmaState::EmaUnstable;
          m_nConfirmationIntervals = nConfirmationIntervalsPreload;
        }
        else {
          if ( bAllRising ) {
            if ( EmaState::EmaUp == m_stateEma ) {
              m_nConfirmationIntervals--;
              if ( 0 == m_nConfirmationIntervals ) {
                std::cout << m_pPositionUnderlying->GetInstrument()->GetInstrumentName() << " " << bar.DateTime() << ": placing long " << m_nSharesToTrade << std::endl;
                m_pPositionUnderlying->CancelOrders();
                m_pPositionUnderlying->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, m_nSharesToTrade );
                m_ceLongEntries.AddLabel( bar.DateTime(), bar.Close(), "Long" );
                m_stateTrading = TSMonitorLong;
                m_stateEma = EmaState::EmaUnstable;
              }
            }
            else {
              m_stateEma = EmaState::EmaUp;
              m_nConfirmationIntervals = nConfirmationIntervalsPreload;
            }
          }
          if ( bAllFalling ) {
            if ( EmaState::EmaDown == m_stateEma ) {
              m_nConfirmationIntervals--;
              if ( 0 == m_nConfirmationIntervals ) {
                std::cout << m_pPositionUnderlying->GetInstrument()->GetInstrumentName() << " " << bar.DateTime() << ": placing short " << m_nSharesToTrade << std::endl;
                m_pPositionUnderlying->CancelOrders();
                m_pPositionUnderlying->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, m_nSharesToTrade );
                m_ceShortEntries.AddLabel( bar.DateTime(), bar.Close(), "Short" );
                m_stateTrading = TSMonitorShort;
                m_stateEma = EmaState::EmaUnstable;
              }
            }
            else {
              m_stateEma = EmaState::EmaDown;
              m_nConfirmationIntervals = nConfirmationIntervalsPreload;
            }
          }
        }
      }
      }
      break;
  }
}

void ManageStrategy::HandleCancel( void ) {
  switch ( m_stateTrading ) {
    case TSNoMore:
      break;
    default:
      std::cout << m_sUnderlying << " cancel" << std::endl;
      if ( nullptr != m_pPositionUnderlying.get() ) m_pPositionUnderlying->CancelOrders();
      if ( nullptr != m_pPositionCall.get() ) m_pPositionCall->CancelOrders();
      if ( nullptr != m_pPositionPut.get() ) m_pPositionPut->CancelOrders();
      break;
  }
}

void ManageStrategy::HandleGoNeutral( void ) {
  switch ( m_stateTrading ) {
    case TSNoMore:
      break;
    default:
      std::cout << m_sUnderlying << " go neutral" << std::endl;
      if ( nullptr != m_pPositionUnderlying.get() ) m_pPositionUnderlying->ClosePosition();
      if ( nullptr != m_pPositionCall.get() ) m_pPositionCall->ClosePosition();
      if ( nullptr != m_pPositionPut.get() ) m_pPositionPut->ClosePosition();
      break;
  }
}

void ManageStrategy::HandleAfterRH( const ou::tf::Quote& quote ) {
  switch ( m_stateTrading ) {
    case TSNoMore:
      break;
    default:
      if ( nullptr != m_pPositionUnderlying.get() ) std::cout << m_sUnderlying << " close results underlying " << *m_pPositionUnderlying << std::endl;
      //if ( nullptr != m_PositionOption_Current.get() ) std::cout << m_sUnderlying << " close results option " << *m_PositionOption_Current << std::endl;
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

void ManageStrategy::SaveSeries( const std::string& sPrefix ) {
  if ( nullptr != m_pPositionUnderlying.get() ) {
    m_pPositionUnderlying->GetWatch()->SaveSeries( sPrefix );
  }
  if ( nullptr != m_pPositionCall.get() ) {
    m_pPositionCall->GetWatch()->SaveSeries( sPrefix );
  }
  if ( nullptr != m_pPositionPut.get() ) {
    m_pPositionPut->GetWatch()->SaveSeries( sPrefix );
  }
}

void ManageStrategy::HandleBarTrades01Sec( const ou::tf::Bar& bar ) {
  
  if ( 0 == m_vEMA.size() ) {  // issue here is that as vector is updated, memory is moved, using heap instead
    m_vEMA.push_back( std::make_shared<EMA>(  5, m_pcdvStrategyData, ou::Colour::SpringGreen ) );
    m_vEMA.back().get()->SetName( "Ema 5s" );
    m_vEMA.push_back( std::make_shared<EMA>( 13, m_pcdvStrategyData, ou::Colour::MediumTurquoise ) );
    m_vEMA.back().get()->SetName( "Ema 13s" );
    m_vEMA.push_back( std::make_shared<EMA>( 34, m_pcdvStrategyData, ou::Colour::DarkOrchid ) );
    m_vEMA.back().get()->SetName( "Ema 34s" );
    m_vEMA.push_back( std::make_shared<EMA>( 89, m_pcdvStrategyData, ou::Colour::DarkMagenta ) );
    m_vEMA.back().get()->SetName( "Ema 89s" );
    std::for_each(
      m_vEMA.begin(), m_vEMA.end(),
      [&bar]( pEMA_t& p ){
        p->First( bar.DateTime(), bar.Close() );
      } );
  }
  else {
    std::for_each(
      m_vEMA.begin(), m_vEMA.end(),
      [&bar]( pEMA_t& p ){
        p->Update( bar.DateTime(), bar.Close() );
      } );
  }
  
  TimeTick( bar );
   
}

void ManageStrategy::HandleBarTrades06Sec( const ou::tf::Bar& bar ) {
  m_cePrice.AppendBar( bar );
  m_ceVolume.Append( bar );

  // was in 60 second, but was supposed to be here
  //m_rBarDirection[ 0 ] = m_rBarDirection[ 1 ];
  //m_rBarDirection[ 1 ] = m_rBarDirection[ 2 ];
  //m_rBarDirection[ 2 ] = ( bar.Open() == bar.Close() ) ? EBarDirection::None : ( ( bar.Open() < bar.Close() ) ? EBarDirection::Up : EBarDirection::Down );

  double dblUnRealized;
  double dblRealized;
  double dblCommissionsPaid;
  double dblTotal;
  m_pPortfolioStrategy->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );
  m_ceProfitLoss.Append( bar.DateTime(), dblTotal );

  m_ceUpReturn.Append( bar.DateTime(), m_cntUpReturn );
  m_cntUpReturn = 0;

  m_ceDnReturn.Append( bar.DateTime(), -m_cntDnReturn );
  m_cntDnReturn = 0;
}

// unused without m_bfTrades60Sec
void ManageStrategy::HandleBarTrades60Sec( const ou::tf::Bar& bar ) { // sentiment event trigger for MasterPortfolio
  //m_fBar( *this, bar );
}

void ManageStrategy::Test() {
  if ( 0 != m_mapChains.size() ) {
    ou::tf::option::IvAtm& iv( m_mapChains.begin()->second );
    double value( 121.5 );
    iv.EmitValues();
    try {
      std::cout << "Put_Itm: "     << iv.Put_Itm( value ) << std::endl;
      std::cout << "Put_ItmAtm: "  << iv.Put_ItmAtm( value ) << std::endl;
      std::cout << "Put_Atm: "     << iv.Put_Atm( value ) << std::endl;
      std::cout << "Put_OtmAtm: "  << iv.Put_OtmAtm( value ) << std::endl;
      std::cout << "Put_Otm: "     << iv.Put_Otm( value ) << std::endl;

      std::cout << "Call_Itm: "    << iv.Call_Itm( value ) << std::endl;
      std::cout << "Call_ItmAtm: " << iv.Call_ItmAtm( value ) << std::endl;
      std::cout << "Call_Atm: "    << iv.Call_Atm( value ) << std::endl;
      std::cout << "Call_OtmAtm: " << iv.Call_OtmAtm( value ) << std::endl;
      std::cout << "Call_Otm: "    << iv.Call_Otm( value ) << std::endl;
    }
    catch ( std::runtime_error& e ) {
      std::cout << "runtime error: " << e.what() << std::endl;
    }
  }
}

