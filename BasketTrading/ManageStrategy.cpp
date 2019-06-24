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

// 2019/06/11 add zig-zag - based on historical 2sd range / 10?
// 2019/06/11 use 2sd historical range for determining edges of spreads and 1 by 2 back spreads

#include <algorithm>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

#include "ManageStrategy.h"

namespace {
  ou::Colour::enumColour rColour[] = {
    ou::Colour::DarkBlue,
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
  const std::string& sUnderlying,
  const std::string& sDailyBarPath,
  const ou::tf::Bar& barPriorDaily,
  pPortfolio_t pPortfolioStrategy,
  fGatherOptionDefinitions_t fGatherOptionDefinitions,
  fConstructWatch_t fConstructWatch,
  fConstructOption_t fConstructOption,
  fConstructPosition_t fConstructPosition,
  fConstructPortfolio_t fConstructPortfolio,
  fRegisterWatch_t fRegisterWatch,
  fRegisterOption_t fRegisterOption,
  fStartCalc_t fStartCalc,
  fStopCalc_t fStopCalc,
  fFirstTrade_t fFirstTrade,
  fAuthorizeUnderlying_t fAuthorizeUnderlying,
  fAuthorizeOption_t fAuthorizeOption,
  fAuthorizeSimple_t fAuthorizeSimple,
  fBar_t fBar,
  pChartDataView_t pcdvStrategyData
  )
: ou::tf::DailyTradeTimeFrame<ManageStrategy>(),
  m_dblOpen {},
  m_sUnderlying( sUnderlying ),
  m_sDailyBarPath( sDailyBarPath ),
  m_barPriorDaily( barPriorDaily ),
  m_pPortfolioStrategy( pPortfolioStrategy ),
  m_fConstructWatch( fConstructWatch ),
  m_fConstructOption( fConstructOption ),
  m_fConstructPosition( fConstructPosition ),
  m_fConstructPortfolio( fConstructPortfolio ),
  m_stateTrading( TSInitializing ),
  m_fRegisterWatch( fRegisterWatch ),
  m_fRegisterOption( fRegisterOption ),
  m_fStartCalc( fStartCalc ),
  m_fStopCalc( fStopCalc ),
  m_fFirstTrade( fFirstTrade ),
  m_fAuthorizeUnderlying( fAuthorizeUnderlying ),
  m_fAuthorizeOption( fAuthorizeOption ),
  m_fAuthorizeSimple( fAuthorizeSimple ),
  m_fBar( fBar ),
  m_eTradeDirection( ETradeDirection::None ),
  m_bfQuotes01Sec( 1 ),
  m_bfTrades01Sec( 1 ),
  m_bfTrades06Sec( 6 ),
//  m_bfTicks06sec( 6 ),
//  m_bfTrades60Sec( 60 ),
//  m_cntUpReturn {}, m_cntDnReturn {},
  m_stateEma( EmaState::EmaUnstable ),
  //m_eOptionState( EOptionState::Initial1 ),
  m_pChartDataView( pcdvStrategyData ),
  m_ixColour {},
  m_bClosedItmLeg( false ), m_bAllowComboAdd( false ),
  m_ceShortEntries( ou::ChartEntryShape::EShort, ou::Colour::Red ),
  m_ceLongEntries( ou::ChartEntryShape::ELong, ou::Colour::Blue ),
  m_ceShortFills( ou::ChartEntryShape::EFillShort, ou::Colour::Red ),
  m_ceLongFills( ou::ChartEntryShape::EFillLong, ou::Colour::Blue ),
  m_ceShortExits( ou::ChartEntryShape::EShortStop, ou::Colour::Red ),
  m_ceLongExits( ou::ChartEntryShape::ELongStop, ou::Colour::Blue ),
  m_DefaultOrderSide( ou::tf::OrderSide::Buy ),
  m_daysToExpiry( 6 ),
  m_pricesDailyCloseBollinger20( m_pricesDailyClose, time_duration( 0, 0, 0 ), 20 )
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
  m_ceProfitLossPortfolio.SetColour( ou::Colour::Fuchsia );
  
  pcdvStrategyData->Add( EChartSlot::Price, &m_cePrice );
  pcdvStrategyData->Add( EChartSlot::Price, &m_cePivots );
  pcdvStrategyData->Add( EChartSlot::Volume, &m_ceVolume );

  pcdvStrategyData->Add( EChartSlot::PL, &m_ceProfitLossPortfolio );

  //pcdvStrategyData->Add( EChartSlot::Tick, &m_ceTickCount );

  //pcdvStrategyData->Add( 4, &m_ceUpReturn );
  //pcdvStrategyData->Add( 4, &m_ceDnReturn );

  pcdvStrategyData->Add( EChartSlot::Price, &m_ceShortEntries );
  pcdvStrategyData->Add( EChartSlot::Price, &m_ceLongEntries );
  pcdvStrategyData->Add( EChartSlot::Price, &m_ceShortFills );
  pcdvStrategyData->Add( EChartSlot::Price, &m_ceLongFills );
  pcdvStrategyData->Add( EChartSlot::Price, &m_ceShortExits );
  pcdvStrategyData->Add( EChartSlot::Price, &m_ceLongExits );

  m_bfQuotes01Sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarQuotes01Sec ) );
  m_bfTrades01Sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarTrades01Sec ) );
  m_bfTrades06Sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarTrades06Sec ) );
  //m_bfTicks06sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarTicks06Sec ) );
//  m_bfTrades60Sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarTrades60Sec ) );

  ReadDailyBars( m_sDailyBarPath );

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
                ou::tf::option::Chain chain;

                iterChains = m_mapChains.find( date ); // see if expiry date exists
                if ( m_mapChains.end() == iterChains ) { // insert new expiry set if not
                  iterChains = m_mapChains.insert(
                    m_mapChains.begin(),
                    mapChains_t::value_type( date, std::move( chain ) )
                    );
                }
              }

              {
                ou::tf::option::Chain& chain( iterChains->second );

                //std::cout << "  option: " << row.sSymbol << std::endl;

                try {
                  switch ( row.eOptionSide ) {
                    case ou::tf::OptionSide::Call:
                      chain.SetIQFeedNameCall( row.dblStrike, row.sSymbol );
                      break;
                    case ou::tf::OptionSide::Put:
                      chain.SetIQFeedNamePut( row.dblStrike, row.sSymbol );
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

        m_fRegisterWatch( pWatchUnderlying );

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
  for ( mapOption_t::value_type& vt: m_mapOption ) { // TODO: fix, isn't the best place?
    m_fStopCalc( vt.second, m_pPositionUnderlying->GetWatch() );
  }
  m_mapOption.clear();
  m_vEMA.clear();
  if ( m_pPositionUnderlying ) {
    pWatch_t pWatch = m_pPositionUnderlying->GetWatch();
    if ( pWatch ) {
      pWatch->OnQuote.Remove( MakeDelegate( this, &ManageStrategy::HandleQuoteUnderlying ) );
      pWatch->OnTrade.Remove( MakeDelegate( this, &ManageStrategy::HandleTradeUnderlying ) );
    }
  }
}

void ManageStrategy::SetPivots( double dblR2, double dblR1, double dblPV, double dblS1, double dblS2 ) {
  // TFIndicators/Pivots.h has R3, S3 plus colour assignments
  m_pivotCrossing.Set( dblR2, dblR1, dblPV, dblS1, dblS2 );
  m_cePivots.AddMark( dblR2, ou::Colour::Red, "R2" );
  m_cePivots.AddMark( dblR1, ou::Colour::Red, "R1" );
  m_cePivots.AddMark( dblPV, ou::Colour::Green, "PV" );
  m_cePivots.AddMark( dblS1, ou::Colour::Blue, "S1" );
  m_cePivots.AddMark( dblS2, ou::Colour::Blue, "S2" );
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
  pWatch_t pWatch = pPosition->GetWatch();
  switch ( pInstrument->GetInstrumentType() ) {
    case ou::tf::InstrumentType::Stock:
      assert( m_pPositionUnderlying );
      assert( pPosition->GetInstrument()->GetInstrumentName() == m_pPositionUnderlying->GetInstrument()->GetInstrumentName() );
      assert( pPosition.get() == m_pPositionUnderlying.get() );
      try {
        m_fRegisterWatch( pWatch );
      }
      catch( std::runtime_error& e ) {
        std::cout << e.what() << std::endl;
      }
      break;
    case ou::tf::InstrumentType::Option:
      if ( pPosition->IsActive() ) {
        idPortfolio_t idPortfolio = pPosition->GetRow().idPortfolio;

        mapCombo_t::iterator mapCombo_iter = m_mapCombo.find( idPortfolio );

        if ( m_mapCombo.end() == mapCombo_iter ) {
          // need to construct empty combo when first leg presented
          Strangle strangle;
          strangle.SetPortfolio( m_fConstructPortfolio( idPortfolio, m_pPortfolioStrategy->Id() ) );
          std::pair<mapCombo_t::iterator, bool> result;
          result = m_mapCombo.insert( mapCombo_t::value_type( idPortfolio, std::move( strangle ) ) );
          assert( result.second );
          mapCombo_iter = result.first;
        }

        pOption_t pOption = boost::dynamic_pointer_cast<ou::tf::option::Option>( pWatch );
        const std::string& sOptionName = pOption->GetInstrument()->GetInstrumentName();

        try {
          m_fRegisterOption( pOption );
        }
        catch( std::runtime_error& e ) {
          std::cout << "ManageStrategy::Add: " << e.what() << std::endl;
        }

        mapOption_t::iterator iterOption = m_mapOption.find( sOptionName );
        if ( m_mapOption.end() == iterOption ) {
          m_mapOption[ sOptionName ] = pOption;
          m_fStartCalc( pOption, m_pPositionUnderlying->GetWatch() );
        }

        Strangle& strangle( mapCombo_iter->second );
        switch ( pInstrument->GetOptionSide() ) {
          case ou::tf::OptionSide::Call:
            std::cout << "setcall " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
            strangle.AddPosition( pPosition, m_pChartDataView, rColour[ m_ixColour++ ] );
            break;
          case ou::tf::OptionSide::Put:
            std::cout << "setput  " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
            strangle.AddPosition( pPosition, m_pChartDataView, rColour[ m_ixColour++ ] );
            break;
        }

//        if ( pPosition->IsActive() ) {
          m_fAuthorizeSimple( m_sUnderlying, true ); // update count
//        }
      }
      break;
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
  //m_bfTicks06sec.Add( trade.DateTime(), 0, 1 );
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
        [this,date](const mapChains_t::value_type& vt)->bool{
          return m_daysToExpiry <= ( vt.first - date );  // first chain where trading date less than expiry date
      } );

      if ( m_mapChains.end() == m_iterChainExpiryInUse ) {
        std::cout << m_sUnderlying << " found no chain for " << date << ", trading disabled" << std::endl;
        m_stateTrading = TSNoMore;
      }
      else {
//        if ( 0 != m_nSharesToTrade ) {
          m_stateTrading = TSOptionEvaluation; // ready to trade
//        }
//        else {
//          m_stateTrading = TSWaitForFundsAllocation; // need an allocation first
//        }
      }
      }
      break;
//    case TSWaitForFundsAllocation: // Start() needs to be called
//      break;
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

// turn into a template if needed for other combo types
void ManageStrategy::BuildPosition(
  const idPortfolio_t& idPortfolio, ou::tf::OptionSide::enumOptionSide side, double price,
  fBuildPositionCallBack_t&& fBuildPositionCallBack
) {

  ou::tf::option::Chain& chain( m_iterChainExpiryInUse->second );

  std::string sName;

  switch ( side ) { // should this be here or in the caller?
    case ou::tf::OptionSide::Call:
      sName = chain.GetIQFeedNameCall( chain.Call_Otm( price ) );
      break;
    case ou::tf::OptionSide::Put:
      sName = chain.GetIQFeedNamePut( chain.Put_Otm( price ) );
      break;
  }

  m_fConstructOption(
    sName,
    m_pPositionUnderlying->GetWatch()->GetInstrument(),
    [this,f=std::move(fBuildPositionCallBack),&idPortfolio]( pOption_t pOption ){
      const std::string& sOptionName = pOption->GetInstrument()->GetInstrumentName();
      mapOption_t::iterator iterOption = m_mapOption.find( sOptionName );
      if ( m_mapOption.end() == iterOption ) {
        m_mapOption[ sOptionName ] = pOption;
        m_fRegisterOption( pOption );
        m_fStartCalc( pOption, m_pPositionUnderlying->GetWatch() );
      }
      pPosition_t pPosition = m_fConstructPosition( idPortfolio, pOption );
      f( pPosition, m_pChartDataView, rColour[ m_ixColour++ ] );
    });

}

/*
 * TODO:
 *   split out each option strategy
 *   add additional states
 *
 */
void ManageStrategy::RHOption( const ou::tf::Bar& bar ) { // assumes one second bars, currently a bar of quote spreads

  switch ( m_stateTrading ) {
    case TSOptionEvaluation:
      {

        const double mid = m_QuoteUnderlyingLatest.Midpoint();

        if ( m_mapCombo.empty() ) {
          m_bAllowComboAdd = true;
        }

        if ( m_bAllowComboAdd ) {

          if ( m_mapChains.end() == m_iterChainExpiryInUse ) {
            std::cout << m_sUnderlying << " has no m_iterChainExpiryInUse" << std::endl;
            m_stateTrading = TSNoMore;  // TODO: fix this for multiple combos in place
          }
          else {

            ou::tf::option::ConstructionTools tools(
              m_iterChainExpiryInUse->first, // expiry
              m_iterChainExpiryInUse->second,  // Chain
              m_pPositionUnderlying->GetWatch(),
              m_fConstructOption
              );

            try {
              if ( m_strangleEvaluation.ValidateSpread( tools, Strangle::m_legDefLong, mid, 11 ) ) { // 11 periods

                idPortfolio_t idPortfolio; // also name of combo (strangle)
                boost::gregorian::date date( m_iterChainExpiryInUse->first );
                idPortfolio
                  = "strangle-"
                  + m_sUnderlying
                  + "-"
                  + ou::tf::Instrument::BuildDate( date.year(), date.month(), date.day() ) // date stamp when first initiated
  //                comment out to allow legs to change with dates, track profit over time
  //                + "-"
  //                + boost::lexical_cast<std::string>( strikeOtmCall ) // no longer available with refactoring
  //                + "-"
  //                + boost::lexical_cast<std::string>( strikeOtmPut ) // no longer available with refactoring
                  ;
                mapCombo_t::iterator mapCombo_iter = m_mapCombo.find( idPortfolio );
                if ( m_mapCombo.end() == mapCombo_iter ) {
                  if ( m_fAuthorizeSimple( m_sUnderlying, false ) ) {
                    if ( ou::tf::OrderSide::Unknown != m_DefaultOrderSide ) {
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

                      pOption_t pOption;
                      mapOption_t::iterator iterOption;
                      std::string sOptionName;

                      Strangle::pOptionPair_t pair = m_strangleEvaluation.ValidatedOptions();

                      pOption = boost::dynamic_pointer_cast<ou::tf::option::Option>( pair.first );
                      sOptionName = pOption->GetInstrument()->GetInstrumentName();
                      iterOption = m_mapOption.find( sOptionName );
                      if ( m_mapOption.end() == iterOption ) {
                        m_mapOption[ sOptionName ] = pOption;
                        m_fRegisterOption( pOption );
                        m_fStartCalc( pOption, m_pPositionUnderlying->GetWatch() );
                      }
                      pPosition_t pPositionCall = m_fConstructPosition( idPortfolio, pOption );
                      strangle.AddPosition( pPositionCall, m_pChartDataView, rColour[ m_ixColour++ ] );

                      pOption = boost::dynamic_pointer_cast<ou::tf::option::Option>( pair.second );
                      sOptionName = pOption->GetInstrument()->GetInstrumentName();
                      iterOption = m_mapOption.find( sOptionName );
                      if ( m_mapOption.end() == iterOption ) {
                        m_mapOption[ sOptionName ] = pOption;
                        m_fRegisterOption( pOption );
                        m_fStartCalc( pOption, m_pPositionUnderlying->GetWatch() );
                      }
                      pPosition_t pPositionPut = m_fConstructPosition( idPortfolio, pOption );
                      strangle.AddPosition( pPositionPut, m_pChartDataView, rColour[ m_ixColour++ ] );

                      strangle.PlaceOrder( m_DefaultOrderSide );
                    }
                  }
                  else {
                    // ?
                  }
                  m_bAllowComboAdd = false;
                }
                // TODO: re-use existing combo?  what if leg is still active? add one or both legs?  if not profitable, no use adding to loss leg
                // TODO: create a trailing stop based upon entry net loss?
              }
            }
            catch ( std::runtime_error& e ) {
              std::cout << m_sUnderlying << " stop trading." << std::endl;
              m_strangleEvaluation.ClearValidation();
              m_stateTrading = TSNoMore;  // TODO: fix this for multiple combos in place
            }
          }
        } // m_bAllowedComboAdd

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

        //bool bClosed( false );
        PivotCrossing::ECrossing crossing = m_pivotCrossing.Update( mid );
        // TODO: need to cross upwards for calls, cross downwards for puts (for long strangle)
//        if ( PivotCrossing::ECrossing::none != crossing ) {
//          for ( mapCombo_t::value_type& vt: m_mapCombo ) {
            //bClosed |= vt.second.CloseItmLegForProfit( mid );
//            namespace ph = std::placeholders;
//            vt.second.CloseItmLegForProfit(
//              mid,
//              m_DefaultOrderSide, // for new entry
//              std::bind( &ManageStrategy::BuildPosition, this, ph::_1, ph::_2, ph::_3, std::move( ph::_4 ) )
//              );
            // implement trailing stop or parabolic SAR
            // how wide to set the stop?  double the average jitter in price?
            // maybe the roll should be to sell the next otm.  depends on how fast moving
            // use the crossing for the trigger for the trailing stop
//          }
//        }
//        if ( bClosed ) { // handled above
          // choice:
          //   a) for now: open another otm leg in the same direction
          //   b) if sufficient total profit, open a new strangle
//        }
      }
      break;
    case TSWaitForEntry:
      break;
    default:
      break;
  }
}

void ManageStrategy::StrategyCondor( const ou::tf::Bar& bar ) {

}

void ManageStrategy::StrategyStrangle( const ou::tf::Bar& bar ) {

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
//                std::cout << m_pPositionUnderlying->GetInstrument()->GetInstrumentName() << " " << bar.DateTime() << ": placing long " << m_nSharesToTrade << std::endl;
                m_pPositionUnderlying->CancelOrders();
//                m_pPositionUnderlying->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, m_nSharesToTrade );
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
//                std::cout << m_pPositionUnderlying->GetInstrument()->GetInstrumentName() << " " << bar.DateTime() << ": placing short " << m_nSharesToTrade << std::endl;
                m_pPositionUnderlying->CancelOrders();
//                m_pPositionUnderlying->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, m_nSharesToTrade );
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

void ManageStrategy::HandleBarTicks06Sec( const ou::tf::Bar& bar ) {
  m_ceTickCount.Append( bar.DateTime(), bar.Volume() );
}

// unused without m_bfTrades60Sec
void ManageStrategy::HandleBarTrades60Sec( const ou::tf::Bar& bar ) { // sentiment event trigger for MasterPortfolio
  //m_fBar( *this, bar );
}

void ManageStrategy::Test() {
  if ( 0 != m_mapChains.size() ) {
    ou::tf::option::Chain& chain( m_mapChains.begin()->second );
    chain.EmitValues();
    chain.Test( 121.5 );
  }
}

double ManageStrategy::EmitInfo() {
  double dblNet {};
  double price( m_pPositionUnderlying->GetWatch()->LastTrade().Price() );
  if ( 0 < m_mapCombo.size() ) {
    std::cout 
      << "Info "
      << m_sUnderlying
      << "@" << price
      << std::endl;
    for ( mapCombo_t::value_type& vt: m_mapCombo ) {
      Strangle& strangle( vt.second );
      std::cout << "  portfolio: " << strangle.GetPortfolio()->Id() << std::endl;
      dblNet += strangle.GetNet( price );
    }
    std::cout << "  net: " << dblNet << std::endl;
  }
  return dblNet;
}

void ManageStrategy::CloseExpiryItm( boost::gregorian::date date ) {
  double price( m_TradeUnderlyingLatest.Price() );
  for ( mapCombo_t::value_type& vt: m_mapCombo ) {
    Strangle& strangle( vt.second );
    if ( 0.0 != price ) {
      strangle.CloseExpiryItm( price, date );
    }
  }
}

void ManageStrategy::CloseFarItm() {
  double price( m_TradeUnderlyingLatest.Price() );
  for ( mapCombo_t::value_type& vt: m_mapCombo ) {
    Strangle& strangle( vt.second );
    if ( 0.0 != price ) {
      strangle.CloseFarItm( price );
    }
  }
}

void ManageStrategy::CloseItmLeg() {
  double price( m_TradeUnderlyingLatest.Price() );
  for ( mapCombo_t::value_type& vt: m_mapCombo ) {
    Strangle& strangle( vt.second );
    if ( 0.0 != price ) {
      m_bClosedItmLeg |= strangle.CloseItmLeg( price );
    }
  }
}

void ManageStrategy::AddStrangle( bool bForced ) {
  if ( bForced ) {
    m_bAllowComboAdd = true;
  }
  else {
    m_bAllowComboAdd = m_bClosedItmLeg;
  }
  m_bClosedItmLeg = false;
}

void ManageStrategy::CloseForProfits() {
  double price( m_TradeUnderlyingLatest.Price() );
  for ( mapCombo_t::value_type& vt: m_mapCombo ) {
    Strangle& strangle( vt.second );
    if ( 0.0 != price ) {
      strangle.CloseForProfits( price );
    }
  }
}

void ManageStrategy::TakeProfits() {
  double price( m_TradeUnderlyingLatest.Price() );
  for ( mapCombo_t::value_type& vt: m_mapCombo ) {
    Strangle& strangle( vt.second );
    if ( 0.0 != price ) {
      strangle.TakeProfits( price );
    }
  }
}

void ManageStrategy::ReadDailyBars( const std::string& sPath ) {

  //void ChartTimeSeries( ou::tf::HDF5DataManager* pdm, ou::ChartDataView* pChartDataView, const std::string& sName, const std::string& sPath )

  ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RO );
  ou::tf::HDF5TimeSeriesContainer<ou::tf::Bar> tsRepository( dm, sPath );
  ou::tf::HDF5TimeSeriesContainer<ou::tf::Bar>::iterator begin, end;
  begin = tsRepository.begin();
  end = tsRepository.end();
  m_barsDaily.Clear();
  const hsize_t cnt = end - begin;
  m_barsDaily.Resize( cnt );
  tsRepository.Read( begin, end, &m_barsDaily );

  m_pricesDailyClose.Clear();
  m_pricesDailyCloseBollinger20.Reset();
  //m_cePivots.Clear(); // need to separate out differently (marks for daily bars only)

  bool bLoopStarted( false );
  bool bPassedUpper( false );
  bool bPassedLower( false );

  m_nPassedUpper = m_nPassedLower = 0;
  m_ixSdMax = m_ixSdMin = 0;
  size_t cntMarking = cnt;

  for ( ou::tf::Bars::const_iterator iterBars = m_barsDaily.begin(); m_barsDaily.end() != iterBars; ++iterBars ) {

    if ( bLoopStarted ) { // calculations use previous day's bollinger
      bPassedUpper = bPassedLower = false;
      if ( iterBars->High() >= m_pricesDailyCloseBollinger20.BBUpper() ) {
        bPassedUpper = true;
      }
      if ( iterBars->Low() <= m_pricesDailyCloseBollinger20.BBLower() ) {
        bPassedLower = true;
      }

      if ( bPassedUpper || bPassedLower ) {
        if ( bPassedUpper && bPassedLower ) {
          m_nPassedUpper++;
          m_nPassedLower++;
        }
        else {
          if ( bPassedUpper ) {
            m_nPassedUpper++;
            m_nPassedLower = 0;
          }
          if ( bPassedLower ) {
            m_nPassedUpper = 0;
            m_nPassedLower++;
          }
        }
      }
      else {
        m_nPassedUpper = 0;
        m_nPassedLower = 0;
      }
    }
    else {
      bLoopStarted = true;
    }

    ou::tf::Price price( iterBars->DateTime(), iterBars->Close() );
    m_pricesDailyClose.Append( price ); // automatically updates indicators (bollinger)
    if ( 55 >= cntMarking ) { // only last 50 bars show attractors
      m_cePivots.AddMark( iterBars->High(), ou::Colour::LightSalmon, "High" );
      m_cePivots.AddMark( iterBars->Low(),  ou::Colour::LightPink,   "Low" );

      const double sd = m_pricesDailyCloseBollinger20.SD();
      if ( sd > m_dblBollingerSDMax ) {
        m_dblBollingerSDMax = sd;
        m_ixSdMax = cntMarking;
      }
      if ( sd < m_dblBollingerSDMin ) {
        m_dblBollingerSDMin = sd;
        m_ixSdMin = cntMarking;
      }
    }
    else {
      m_dblBollingerSDMax = m_dblBollingerSDMin = m_pricesDailyCloseBollinger20.SD();
      m_ixSdMax = m_ixSdMin = cntMarking;
    }
    cntMarking--;
  } // end for

  m_cePivots.AddMark( m_pricesDailyCloseBollinger20.BBUpper(), ou::Colour::Purple,     "BollUp" );
  m_cePivots.AddMark( m_pricesDailyCloseBollinger20.MeanY(),   ou::Colour::Salmon,     "BollMn" );
  m_cePivots.AddMark( m_pricesDailyCloseBollinger20.BBLower(), ou::Colour::PowderBlue, "BollLo" );

  std::cout
    << m_sUnderlying
    << " sd min=" << m_dblBollingerSDMin << "@" << m_ixSdMin
    << ",cur= " << m_pricesDailyCloseBollinger20.SD()
    << ",max=" << m_dblBollingerSDMax << "@" << m_ixSdMax;
  if ( 1 == m_nPassedUpper ) std::cout << " - first touch on upper bollinger";
  if ( 1 == m_nPassedLower ) std::cout << " - first touch on lower bollinger";
  std::cout << std::endl;

  // trigger: if cross a bollinger band today, with m_nPassedxx 0, then a successful trigger for entry
  //          and ixSDmxx is not 1, then probably best time for backspread on near band as volatility has range to increase.
  //          and a normal spread on far band
  //          (within 6 days of expiry, allows some time for movement)
  // trigger: if crosses bollinger mean, then run a bull-put and bear-call
  //          (within min of 1 day of expiry to catch last day, or weekend theta decay)
  // trigger: darvas, initiate synthetic with protective option
  //          (synthetic 13 days, protective 4 days)
  // trigger:  sell strangle on high volatility, and exit on normal volatility
  //           will need database to indicate historical implied volatility
  //           will need to run options all the time in order to monitor implied volatility
  //           (1 day to expiry to reduce duration risk)

//    AddChartEntries( pChartDataView, series );

}