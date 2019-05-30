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

    /* 2019/05/06
     * For orders, opening, as well as closing
     * find atm strike (done)
     * create option at strike (done)
     * verify that quotes are within designated spread (done)
     * create position (done)
     * create order from position, submit as limit at midpoint (done)
     * periodically, if order still executing,
     *   update middiff, +/- based upon buy or sell (done)
     * => need call,put spreads to be < 0.10 && >= 0.01 (for a 6s interval)  (done)
     * => adjacent strikes need to be within 0.51 (done)
     * => a roll up or down needs to retain some profit after commission and spread
     * => roll once directional momentum on underlying has changed
     * => check open interest
     * => needs to be multi-day affair to reduce entry/exit spreads/commissions
     * => need end of week calendar roll, preferably when already about to roll on a strike
     *       start wed/thurs on the calendar rolls
     * => autonomously monitor entries, seek confirmation from money manager prior to entry
     * => allow daily and long term portfolios (allows another attempt at the ema strategy)
     * => to reduce symbol count, load up call first to examine spread, then load up put for verification?
     */

#include <algorithm>

#include "ManageStrategy.h"

namespace {
  ou::Colour::enumColour rColour[] = {
    ou::Colour::Fuchsia,
    ou::Colour::DarkCyan,
    ou::Colour::MediumSlateBlue,
    ou::Colour::SteelBlue,
    ou::Colour::DarkOrange,
    ou::Colour::MediumTurquoise,
    ou::Colour::DarkOrchid,
    ou::Colour::DarkMagenta,
    ou::Colour::DeepPink,
    ou::Colour::MediumPurple,
    ou::Colour::MediumBlue
  };
}

ManageStrategy::ManageStrategy(
  const std::string& sUnderlying, const ou::tf::Bar& barPriorDaily,
  pPortfolio_t pPortfolioStrategy,
  fGatherOptionDefinitions_t fGatherOptionDefinitions,
  fConstructWatch_t fConstructWatch,
  fConstructOption_t fConstructOption,
  fConstructPosition_t fConstructPosition,
  fConstructPortfolio_t fConstructPortfolio,
  fStartCalc_t fStartCalc,
  fStopCalc_t fStopCalc,
  fFirstTrade_t fFirstTrade,
  fBar_t fBar,
  pChartDataView_t pcdvStrategyData
  )
: ou::tf::DailyTradeTimeFrame<ManageStrategy>(),
  m_dblOpen {},
  m_nSharesToTrade {}, m_dblFundsToTrade {},
  m_sUnderlying( sUnderlying ),
  m_barPriorDaily( barPriorDaily ),
  m_pPortfolioStrategy( pPortfolioStrategy ),
  m_fConstructWatch( fConstructWatch ),
  m_fConstructOption( fConstructOption ),
  m_fConstructPosition( fConstructPosition ),
  m_fConstructPortfolio( fConstructPortfolio ),
  m_stateTrading( TSInitializing ),
  m_fStartCalc( fStartCalc ),
  m_fStopCalc( fStopCalc ),
  m_fFirstTrade( fFirstTrade ),
  m_fBar( fBar ),
  m_eTradeDirection( ETradeDirection::None ),
  m_bfQuotes01Sec( 1 ),
  m_bfTrades01Sec( 1 ),
  m_bfTrades06Sec( 6 ),
//  m_bfTrades60Sec( 60 ),
//  m_cntUpReturn {}, m_cntDnReturn {},
  m_stateEma( EmaState::EmaUnstable ),
  //m_eOptionState( EOptionState::Initial1 ),
  m_pChartDataView( pcdvStrategyData ),
  m_ixColour {},
  m_nLegs {},
  m_ceShortEntries( ou::ChartEntryShape::EShort, ou::Colour::Red ),
  m_ceLongEntries( ou::ChartEntryShape::ELong, ou::Colour::Blue ),
  m_ceShortFills( ou::ChartEntryShape::EFillShort, ou::Colour::Red ),
  m_ceLongFills( ou::ChartEntryShape::EFillLong, ou::Colour::Blue ),
  m_ceShortExits( ou::ChartEntryShape::EShortStop, ou::Colour::Red ),
  m_ceLongExits( ou::ChartEntryShape::ELongStop, ou::Colour::Blue ),
  m_SpreadValidation( 2 )
{
  //std::cout << m_sUnderlying << " loading up ... " << std::endl;

  assert( nullptr != fGatherOptionDefinitions );
  assert( nullptr != m_fConstructWatch );
  assert( nullptr != m_fConstructOption );
  assert( nullptr != m_fConstructPosition );
  assert( nullptr != m_fConstructPortfolio );
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
  //m_ceUpReturn.SetName( "Up Return" );
  //m_ceDnReturn.SetName( "Dn Return" );
  m_ceProfitLossPortfolio.SetName( "P/L Portfolio" );

  //m_ceUpReturn.SetColour( ou::Colour::Red );
  //m_ceDnReturn.SetColour( ou::Colour::Blue );
  m_ceProfitLossPortfolio.SetColour( ou::Colour::DarkBlue );
  
  pcdvStrategyData->Add( 0, &m_cePrice );
  pcdvStrategyData->Add( 0, &m_cePivots );
  pcdvStrategyData->Add( 1, &m_ceVolume );

  pcdvStrategyData->Add( 2, &m_ceProfitLossPortfolio );

  //pcdvStrategyData->Add( 4, &m_ceUpReturn );
  //pcdvStrategyData->Add( 4, &m_ceDnReturn );

  pcdvStrategyData->Add( 0, &m_ceShortEntries );
  pcdvStrategyData->Add( 0, &m_ceLongEntries );
  pcdvStrategyData->Add( 0, &m_ceShortFills );
  pcdvStrategyData->Add( 0, &m_ceLongFills );
  pcdvStrategyData->Add( 0, &m_ceShortExits );
  pcdvStrategyData->Add( 0, &m_ceLongExits );

  m_bfQuotes01Sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarQuotes01Sec ) );
  m_bfTrades01Sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarTrades01Sec ) );
  m_bfTrades06Sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarTrades06Sec ) );
//  m_bfTrades60Sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarTrades60Sec ) );

  try {

    //std::cout << "Construct Watch: " << m_sUnderlying << std::endl;

    m_fConstructWatch( // underlying construction only
      m_sUnderlying,
      [this,fGatherOptionDefinitions](pWatch_t pWatchUnderlying){

        std::cout << m_sUnderlying << " watch arrived ... " << std::endl;

        assert( m_pPortfolioStrategy );
        assert( pWatchUnderlying );
        assert( 0 != pWatchUnderlying->GetInstrument()->GetContract() );

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

        pInstrument_t pInstrumentUnderlying = m_pPositionUnderlying->GetInstrument();
        if ( 0 == pInstrumentUnderlying->GetContract() ) {
          std::cout << m_pPositionUnderlying->GetInstrument()->GetInstrumentName() << " has no contract" << std::endl;
          m_stateTrading = TSNoMore;
        }
    } ); // m_fConstructWatch

  }
  catch (...) {
    std::cout << "*** " << "something wrong with " << m_sUnderlying << " creation." << std::endl;
  }

 m_stateTrading = TSWaitForFirstTrade;

  //std::cout << m_sUnderlying << " loading done." << std::endl;
}

ManageStrategy::~ManageStrategy( ) {
  m_mapCombo.clear();
  m_vEMA.clear();
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

void ManageStrategy::Add( pPosition_t pPosition ) {
  pInstrument_t pInstrument = pPosition->GetInstrument();
  switch ( pInstrument->GetInstrumentType() ) {
    case ou::tf::InstrumentType::Stock:
      assert( m_pPositionUnderlying );
      assert( pPosition->GetInstrument()->GetInstrumentName() == m_pPositionUnderlying->GetInstrument()->GetInstrumentName() );
      assert( pPosition.get() == m_pPositionUnderlying.get() );
      break;
    case ou::tf::InstrumentType::Option: {

      idPortfolio_t idPortfolio = pPosition->GetRow().idPortfolio;

      mapCombo_t::iterator mapCombo_iter = m_mapCombo.find( idPortfolio );

      if ( m_mapCombo.end() == mapCombo_iter ) {
        // need to construct Combo with first leg
        Strangle strangle;
        strangle.SetPortfolio( m_fConstructPortfolio( idPortfolio, m_pPortfolioStrategy->Id() ) );
        std::pair<mapCombo_t::iterator, bool> result;
        result = m_mapCombo.insert( mapCombo_t::value_type( idPortfolio, std::move( strangle ) ) );
        assert( result.second );
        mapCombo_iter = result.first;
      }

      Strangle& strangle( mapCombo_iter->second );
      switch ( pInstrument->GetOptionSide() ) {
        case ou::tf::OptionSide::Call:
          std::cout << "setcall " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
          strangle.SetPositionCall( pPosition, rColour[ m_ixColour++ ] );
          strangle.SetColourCall( rColour[ m_ixColour++ ] );
          m_nLegs++;
          break;
        case ou::tf::OptionSide::Put:
          std::cout << "setput  " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
          strangle.SetPositionPut( pPosition, rColour[ m_ixColour++ ] );
          strangle.SetColourPut( rColour[ m_ixColour++ ] );
          m_nLegs++;
          break;
      }
      }
      break;
  }
}

void ManageStrategy::Start(  ) {

  //std::cout << m_sUnderlying << " starting up ... " << std::endl;

  //assert( TSWaitForCalc == m_stateTrading );

  if ( !m_pPositionUnderlying ) {
    std::cout << m_sUnderlying << " doesn't have a position ***" << std::endl;
    m_stateTrading = TSNoMore;
  }
  else {
    if ( 0.0 == m_dblFundsToTrade ) {
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

        switch ( m_stateTrading ) {
          case TSWaitForFirstTrade: // can't do anything yet
            break;
          case TSWaitForFundsAllocation:
            m_stateTrading = TSOptionEvaluation; // first trade available, and funds available, so start with options
            break;
        }
    }
  }
}

void ManageStrategy::Stop( void ) {
  HandleCancel();
  std::for_each(
    m_mapCombo.begin(), m_mapCombo.end(),
    [this](mapCombo_t::value_type& entry){
      entry.second.ClosePositions();  // maintain positions over night
    }
    );
}

void ManageStrategy::HandleBellHeard( void ) {
}

void ManageStrategy::HandleQuoteUnderlying( const ou::tf::Quote& quote ) {
//  if ( quote.IsValid() ) {
    m_QuoteUnderlyingLatest = quote;
    m_bfQuotes01Sec.Add( quote.DateTime(), quote.Spread(), 1 );
//    m_quotes.Append( quote );
    TimeTick( quote );
//  }
}

void ManageStrategy::HandleTradeUnderlying( const ou::tf::Trade& trade ) {
//  if ( trade.Price() > m_TradeLatest.Price() ) m_cntUpReturn++;
//  if ( trade.Price() < m_TradeLatest.Price() ) m_cntDnReturn--;
//  m_trades.Append( trade );
  m_bfTrades01Sec.Add( trade );
  m_bfTrades06Sec.Add( trade );
//  m_bfTrades60Sec.Add( trade );
  TimeTick( trade );
  m_TradeUnderlyingLatest = trade; // allow previous one to be used till last moment
}

void ManageStrategy::HandleRHTrading( const ou::tf::Quote& quote ) {
  switch ( m_stateTrading ) {
    case TSWaitForEntry:
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
    case TSWaitForFirstTrade: {
      m_dblOpen = trade.Price();
//      std::cout << m_sUnderlying << " " << trade.DateTime() << ": First Price: " << trade.Price() << std::endl;
      m_fFirstTrade( *this, trade );

      boost::gregorian::date date( trade.DateTime().date() );
      m_iterChainExpiryInUse = std::find_if( m_mapChains.begin(), m_mapChains.end(),
        [date](const mapChains_t::value_type& vt)->bool{
          return boost::gregorian::days( 6 ) < ( vt.first - date );  // first chain where trading date less than expiry date
      } );

      if ( m_mapChains.end() == m_iterChainExpiryInUse ) {
        std::cout << m_sUnderlying << " found no chain for " << date << ", trading disabled" << std::endl;
        m_stateTrading = TSNoMore;
      }
      else {
        if ( 0 != m_nSharesToTrade ) {
          m_stateTrading = TSOptionEvaluation; // ready to trade
        }
        else {
          m_stateTrading = TSWaitForFundsAllocation; // need an allocation first
        }
      }
      }
      break;
    case TSWaitForFundsAllocation: // Start() needs to be called
      break;
    case TSWaitForEntry:
      break;
    case TSOptionEvaluation:
      break;
    case TSMonitorStrangle:
      break;
    case TSMonitorLong: {
      pEMA_t& pEMA( m_vEMA.back() );
      if ( trade.Price() < pEMA->dblEmaLatest ) {
        m_pPositionUnderlying->ClosePosition( ou::tf::OrderType::Market );
        m_ceLongExits.AddLabel( trade.DateTime(), trade.Price(), "Stop" );
        std::cout << m_sUnderlying << " closing long" << std::endl;
        m_stateTrading = TSWaitForEntry;
      }
      }
      break;
    case TSMonitorShort: {
      pEMA_t& pEMA( m_vEMA.back() );
      if ( trade.Price() > pEMA->dblEmaLatest ) {
        m_pPositionUnderlying->ClosePosition( ou::tf::OrderType::Market );
        m_ceShortExits.AddLabel( trade.DateTime(), trade.Price(), "Stop" );
        std::cout << m_sUnderlying << " closing short" << std::endl;
        m_stateTrading = TSWaitForEntry;
      }
      }
      break;
    default:
      break;
  }
}

void ManageStrategy::HandleRHTrading( const ou::tf::Bar& bar ) { // one second bars, currently composite of quote spreads
  // this is one tick behind, so could use m_TradeLatest for latest close-of-last/open-of-next
  //RHEquity( bar );
  RHOption( bar );
}

double ManageStrategy::CurrentAtmStrike( double mid ) { // needs try/catch around this call
  double strikeCall {};
  double strikePut {};
  strikePut = m_iterChainExpiryInUse->second.Put_Atm( mid );  // may raise an exception (on or nearest strike)
  strikeCall = m_iterChainExpiryInUse->second.Call_Atm( mid );  // may raise an exception (on or nearest strike)
  if ( strikePut != strikeCall ) {
    std::cout << m_sUnderlying << ": atm strike not matching - midpoint=" << mid << ",put=" << strikePut << ",call=" << strikeCall << std::endl;
    throw std::runtime_error( "strikePut != strikeCall" );
  }
  return strikePut;
}

void ManageStrategy::RHOption( const ou::tf::Bar& bar ) { // assumes one second bars, currently a bar of quote spreads

//  static const double dblMaxStrikeDistance( 0.51 );  // not 0.50 to prevent rounding problems.
  static const double dblMaxStrikeDistance( 1.01 );  // testing with SPY

  switch ( m_stateTrading ) {
    case TSOptionEvaluation:
      {

        // keep potential options warmed up
        double mid = m_QuoteUnderlyingLatest.Midpoint();

        bool bStrikesFound( false );
        double strikeOtmCall {};
        double strikeOtmPut {};

        try {
          if ( m_mapChains.end() == m_iterChainExpiryInUse ) {
            std::cout << m_sUnderlying << " has no m_iterChainExpiryInUse" << std::endl;
            m_stateTrading = TSNoMore;  // TODO: fix this for multiple combos in place
          }
          else {
            strikeOtmCall = m_iterChainExpiryInUse->second.Call_Otm( mid );
            strikeOtmPut = m_iterChainExpiryInUse->second.Put_Otm( mid );
            bStrikesFound = true;
          }
        }
        catch ( std::runtime_error& e ) {
          if ( m_mapCombo.empty() ) {
            std::cout << m_sUnderlying << " found no strike for mid-point " << mid
                                       << " expiry " << m_iterChainExpiryInUse->first
                                       << " for quote " << m_QuoteUnderlyingLatest.DateTime().date()
                                       << " [" << e.what() << "]"
                                       << std::endl;
            m_stateTrading = TSNoMore;  // TODO: fix this for multiple combos in place
          }
        }

        if ( bStrikesFound ) {
          bool bBuildOptions( false );
          if ( !m_SpreadValidation.IsActive() ) {
            bBuildOptions = true;
          }
          else {  // TODO: need some hysterisis on this calculation
            if ( ( strikeOtmCall != boost::dynamic_pointer_cast<ou::tf::option::Option>( m_SpreadValidation.GetOption( 0 ) )->GetStrike() )
              || ( strikeOtmPut  != boost::dynamic_pointer_cast<ou::tf::option::Option>( m_SpreadValidation.GetOption( 1 ) )->GetStrike() )
            ) {
              m_SpreadValidation.ResetOptions();
              bBuildOptions = true;
            }
          }

          if ( !m_mapCombo.empty() ) bBuildOptions = false; // temporary until multiple combos allowed
          // TODO: additional combos will probably be manually triggered

          if ( bBuildOptions ) {
            double diff = strikeOtmCall - strikeOtmPut; // check that strikes are max 0.50 apart
            if ( 0.0 > diff ) diff = -diff;  // aka absolute value
            if ( dblMaxStrikeDistance > diff ) { // confirming: diff(mid,strike)<0.51
              //if ( ( dblMaxStrikeDistance > ( strikeAtm - strikeLower) ) && ( dblMaxStrikeDistance > ( strikeUpper - strikeAtm ) ) ) {
              if ( ( dblMaxStrikeDistance > ( strikeOtmCall - strikeOtmPut ) ) ) {
                std::cout
                  << m_sUnderlying
                  << ": constructing options for strangle -> quote=" << mid
                  << ",otm call=" << strikeOtmCall
                  << ",otm put=" << strikeOtmPut
                  << std::endl;
                pInstrument_t pInstrumentUnderlying = m_pPositionUnderlying->GetInstrument();
                m_fConstructOption( m_iterChainExpiryInUse->second.GetIQFeedNameCall( strikeOtmCall), pInstrumentUnderlying,
                  [this](pOption_t pOptionCall){
                    //std::cout << pOptionCall->GetInstrument()->GetInstrumentName() << " open interest: " << pOptionCall->Summary().nOpenInterest << std::endl; // too early
                    m_SpreadValidation.SetOption( 0, pOptionCall );
                  } );
                m_fConstructOption( m_iterChainExpiryInUse->second.GetIQFeedNamePut( strikeOtmPut), pInstrumentUnderlying,
                  [this](pOption_t pOptionPut){
                    //std::cout << pOptionPut->GetInstrument()->GetInstrumentName() << " open interest: " << pOptionPut->Summary().nOpenInterest << std::endl; // tool early
                    m_SpreadValidation.SetOption( 1, pOptionPut );
                  } );
              }
            }
          }
        }

        if ( m_mapCombo.empty() ) { // temporary until determine how to properly add additional entries
          if ( m_SpreadValidation.IsActive() ) {
            if ( m_SpreadValidation.Validate( 11 ) ) {
              idPortfolio_t idPortfolio;
              idPortfolio
                = "strangle-"
                + m_sUnderlying
                + "-"
                + boost::lexical_cast<std::string>( strikeOtmCall )
                + "-"
                + boost::lexical_cast<std::string>( strikeOtmPut );
              mapCombo_t::iterator mapCombo_iter = m_mapCombo.find( idPortfolio );
              if ( m_mapCombo.end() == mapCombo_iter ) {
                std::cout << m_sUnderlying << ": option spreads validated, creating positions" << std::endl;
                std::pair<mapCombo_t::iterator,bool> result;
                //result = m_mapCombo.insert( mapCombo_t::value_type( strikeAtm, Strangle( strikeLower, strikeAtm, strikeUpper ) ) );
                result = m_mapCombo.insert( mapCombo_t::value_type( idPortfolio, Strangle() ) );
                assert( result.second );
                assert( m_mapCombo.end() != result.first );
                Strangle& strangle( result.first->second );
                if ( m_ixColour >= ( sizeof( rColour ) - 2 ) ) {
                  std::cout << "WARNING: strategy running out of colours." << std::endl;
                }
                strangle.SetPortfolio( m_fConstructPortfolio( idPortfolio, m_pPortfolioStrategy->Id() ) );
                pPosition_t pPositionCall = m_fConstructPosition( idPortfolio, m_SpreadValidation.GetOption( 0 ) );
                strangle.SetPositionCall( pPositionCall, rColour[ m_ixColour++ ] );
                pPosition_t pPositionPut = m_fConstructPosition( idPortfolio, m_SpreadValidation.GetOption( 1 ) );
                strangle.SetPositionPut( pPositionPut, rColour[ m_ixColour++ ] );
                m_SpreadValidation.ResetOptions();
                strangle.OrderLongStrangle();
                //m_eOptionState = EOptionState::MonitorPosition;
                //m_stateTrading = ETradingState::TSMonitorStrangle;
                //strike.m_state = Strike::State::Executing;
                strangle.AddChartData( m_pChartDataView );
              }
            }
          }
        }

        std::for_each(
          m_mapCombo.begin(), m_mapCombo.end(),
          [this,mid,&bar](mapCombo_t::value_type& entry){
            Strangle& strangle( entry.second );
            switch ( strangle.m_state ) {
              case Strangle::State::Initializing:
                break;
              case Strangle::State::Positions:
                strangle.Tick( true, mid, bar.DateTime() );
                break;
              case Strangle::State::Executing:
                strangle.Tick( true, mid, bar.DateTime() );
                break;
              case Strangle::State::Watching:
                strangle.Tick( true, mid, bar.DateTime() );
                break;
              case Strangle::State::Canceled:
                strangle.Tick( true, mid, bar.DateTime() );
                break;
              case Strangle::State::Closing:
                strangle.Tick( true, mid, bar.DateTime() );
                break;
            }
          }
        );
      }
      break;
    case TSWaitForEntry:
      break;
    default:
      break;
  }
}

void ManageStrategy::RHEquity( const ou::tf::Bar& bar ) {
  switch ( m_stateTrading ) {
    case TSWaitForEntry:
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
      static const size_t nConfirmationIntervalsPreload( 19 );
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
      break;
  }
}

// 4 minutes prior to close
void ManageStrategy::HandleCancel( void ) {
  switch ( m_stateTrading ) {
    case TSNoMore:
      break;
    default:
      std::cout << m_sUnderlying << " cancel" << std::endl;
      if ( m_pPositionUnderlying ) m_pPositionUnderlying->CancelOrders();
      std::for_each(
        m_mapCombo.begin(), m_mapCombo.end(),
        [this](mapCombo_t::value_type& entry){
          entry.second.CancelOrders();
        }
        );
      break;
  }
}

// 3 minutes, 45 seconds prior to close
void ManageStrategy::HandleGoNeutral( void ) {
  switch ( m_stateTrading ) {
    case TSNoMore:
      break;
    default:
//      std::cout << m_sUnderlying << " go neutral" << std::endl;
      if ( m_pPositionUnderlying ) m_pPositionUnderlying->ClosePosition();
      std::for_each(
        m_mapCombo.begin(), m_mapCombo.end(),
        [this](mapCombo_t::value_type& entry){
//          entry.second.ClosePositions();  // maintain positions over night
        }
        );
      break;
  }
}

void ManageStrategy::HandleGoingNeutral( const ou::tf::Bar& bar ) {
  RHOption( bar );
}

void ManageStrategy::HandleAfterRH( const ou::tf::Quote& quote ) {
  switch ( m_stateTrading ) {
    case TSNoMore:
      break;
    default:
//      if ( nullptr != m_pPositionUnderlying )  // no meaning in an option only context
//        std::cout << m_sUnderlying << " close results underlying " << *m_pPositionUnderlying << std::endl;
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
  std::for_each(
    m_mapCombo.begin(), m_mapCombo.end(),
    [this,&sPrefix](mapCombo_t::value_type& entry){
      entry.second.SaveSeries( sPrefix );
    }
    );
}

void ManageStrategy::HandleBarQuotes01Sec( const ou::tf::Bar& bar ) {
  TimeTick( bar );
}

void ManageStrategy::HandleBarTrades01Sec( const ou::tf::Bar& bar ) {
  
  if ( 0 == m_vEMA.size() ) {  // issue here is that as vector is updated, memory is moved, using heap instead
    m_vEMA.push_back( std::make_shared<EMA>(  5, m_pChartDataView, ou::Colour::DarkOrange ) );
    m_vEMA.back().get()->SetName( "Ema 5s" );
    m_vEMA.push_back( std::make_shared<EMA>( 13, m_pChartDataView, ou::Colour::MediumTurquoise ) );
    m_vEMA.back().get()->SetName( "Ema 13s" );
    m_vEMA.push_back( std::make_shared<EMA>( 34, m_pChartDataView, ou::Colour::DarkOrchid ) );
    m_vEMA.back().get()->SetName( "Ema 34s" );
    m_vEMA.push_back( std::make_shared<EMA>( 89, m_pChartDataView, ou::Colour::DarkMagenta ) );
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
  
  //TimeTick( bar );  // using quotes as time tick
   
}

void ManageStrategy::HandleBarTrades06Sec( const ou::tf::Bar& bar ) {

  m_cePrice.AppendBar( bar );
  m_ceVolume.Append( bar );

//  m_rBarDirection[ 0 ] = m_rBarDirection[ 1 ];
//  m_rBarDirection[ 1 ] = m_rBarDirection[ 2 ];
//  m_rBarDirection[ 2 ] = ( bar.Open() == bar.Close() ) ? EBarDirection::None : ( ( bar.Open() < bar.Close() ) ? EBarDirection::Up : EBarDirection::Down );

  double dblUnRealized;
  double dblRealized;
  double dblCommissionsPaid;
  double dblTotal;
  
  m_pPortfolioStrategy->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );
  m_ceProfitLossPortfolio.Append( bar.DateTime(), dblTotal );

//  if ( m_pPositionCall ) {
//    m_pPositionCall->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );
//    m_ceProfitLossCall.Append( bar.DateTime(), dblTotal );
//  }

//  if ( m_pPositionPut ) {
//    m_pPositionPut->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );
//    m_ceProfitLossPut.Append( bar.DateTime(), dblTotal );
//  }

//  m_ceUpReturn.Append( bar.DateTime(), m_cntUpReturn );
//  m_cntUpReturn = 0;

//  m_ceDnReturn.Append( bar.DateTime(), m_cntDnReturn );
//  m_cntDnReturn = 0;
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

double ManageStrategy::EmitInfo() {
  double dblNet {};
  if ( 0 < m_mapCombo.size() ) {
    std::cout << "TakeProfits " << this->m_sUnderlying << std::endl;
    std::for_each(
      m_mapCombo.begin(), m_mapCombo.end(),
      [this,&dblNet](mapCombo_t::value_type& vt){
        Strangle& strangle( vt.second );
        dblNet += strangle.GetNet();
      }
    );
    std::cout << "  net: " << dblNet << std::endl;
  }
  return dblNet;
}

void ManageStrategy::CloseExpiryItm( boost::gregorian::date date ) {
  std::for_each(
    m_mapCombo.begin(), m_mapCombo.end(),
    [this,&date](mapCombo_t::value_type& vt){
      Strangle& strangle( vt.second );
      double price( m_TradeUnderlyingLatest.Price() );
      if ( 0.0 != price ) {
        strangle.CloseExpiryItm( price, date );
      }
    }
  );
}

void ManageStrategy::CloseFarItm() {
  std::for_each(
    m_mapCombo.begin(), m_mapCombo.end(),
    [this](mapCombo_t::value_type& vt){
      Strangle& strangle( vt.second );
      double price( m_TradeUnderlyingLatest.Price() );
      if ( 0.0 != price ) {
        strangle.CloseFarItm( price );
      }
    }
  );
}

void ManageStrategy::CloseForProfits() {
  std::for_each(
    m_mapCombo.begin(), m_mapCombo.end(),
    [this](mapCombo_t::value_type& vt){
      Strangle& strangle( vt.second );
      double price( m_TradeUnderlyingLatest.Price() );
      if ( 0.0 != price ) {
        strangle.CloseForProfits( price );
      }
    }
  );
}

void ManageStrategy::AddStrangle() {
//  std::for_each(
//    m_mapCombo.begin(), m_mapCombo.end(),
//    [this](mapCombo_t::value_type& vt){
//      Strangle& strangle( vt.second );
      //double price( m_TradeUnderlyingLatest.Price() );
      //if ( 0.0 != price ) {
        //strangle.AddStrangle( price );
      //}
      std::cout << m_sUnderlying << ": AddStrangle - to be implemented" << std::endl;
//    }
//  );
}

void ManageStrategy::TakeProfits() {
  std::for_each(
    m_mapCombo.begin(), m_mapCombo.end(),
    [this](mapCombo_t::value_type& vt){
      Strangle& strangle( vt.second );
      double price( m_TradeUnderlyingLatest.Price() );
      if ( 0.0 != price ) {
        strangle.TakeProfits( price );
      }
    }
  );
}
