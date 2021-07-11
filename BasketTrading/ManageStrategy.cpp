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

// 2019/05/23 Trading Day
//   ES dropped from 2056 at futures open to about 2016 in the morning (-1.15->-1.2% drop)
//   strangles were profitable on the basket elements
//   profitable legs could be exited
//   TODO: watch ATM IV to see if profitable legs should be rolled-up/down or just exited
//      and new strikes entered when IV returns closer to noral
//      or sell premium(short the same leg?)

#include <algorithm>

#include <TFOptionCombos/Collar.h>
using combo_t = ou::tf::option::Collar;
#include <TFOptionCombos/LegNote.h>

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

// == OptionRepository

class OptionRepository {
public:

  using fRegisterOption_t = ManageStrategy::fRegisterOption_t;
  using fStartCalc_t = ManageStrategy::fStartCalc_t;
  using fStopCalc_t  = ManageStrategy::fStopCalc_t;

  //using pPosition_t  = ou::tf::Position::pPosition_t;
  using pWatch_t      = ou::tf::option::Option::pWatch_t;
  using pOption_t   = ou::tf::option::Option::pOption_t;

  OptionRepository(
    fRegisterOption_t&& fRegisterOption,
    fStartCalc_t&& fStartCalc,
    fStopCalc_t&& fStopCalc
  ) :
    m_fRegisterOption( std::move( fRegisterOption ) ),
    m_fStartCalc( std::move( fStartCalc ) ),
    m_fStopCalc( std::move( fStopCalc ) )
  {
    assert( nullptr != m_fStartCalc );
    assert( nullptr != m_fStopCalc );
    assert( nullptr != m_fRegisterOption );
  }

  ~OptionRepository() {
    for ( mapOption_t::value_type& vt: m_mapOption ) { // TODO: fix, isn't the best place?
      m_fStopCalc( vt.second, m_pWatchUnderlying );
    }
    m_mapOption.clear();
  }

  // don't worry about reference counting, options in a strategy need to be unique
  // however, may need to worry about the option if from previous time frame

  void AssignWatchUnderlying( pWatch_t pWatchUnderlying ) {
    m_pWatchUnderlying = pWatchUnderlying;
  }

  void Add( pOption_t pOption ) {

    const std::string& sOptionName( pOption->GetInstrument()->GetInstrumentName() );

    mapOption_t::iterator iterOption = m_mapOption.find( sOptionName );
    if ( m_mapOption.end() == iterOption ) {

      try {
        m_fRegisterOption( pOption );
      }
      catch( std::runtime_error& e ) {
        std::cout << "OptionRepository::Add: " << e.what() << std::endl;
        // simply telling us we are already registered, convert from error to status?
      }

      m_mapOption[ sOptionName ] = pOption;
      // TODO: activate calc only if option is active
      m_fStartCalc( pOption, m_pWatchUnderlying );
    }
    else {
      std::cout << "OptionRepository::Add: error, duplicate entry: " << sOptionName << std::endl;
    }

  }

  void Remove( pOption_t pOption ) {

    const std::string& sOptionName( pOption->GetInstrument()->GetInstrumentName() );

    mapOption_t::iterator iterOption = m_mapOption.find( sOptionName );
    if ( m_mapOption.end() != iterOption ) {

      m_fStopCalc( pOption, m_pWatchUnderlying );
      // TODO: stop calc only if option is active
      m_mapOption.erase( iterOption );

    }
    else {
      std::cout << "OptionRepository::Remove: error, option not found: " << sOptionName << std::endl;
    }

  }

protected:
private:

  pWatch_t m_pWatchUnderlying;

  fRegisterOption_t m_fRegisterOption;
  fStartCalc_t m_fStartCalc;
  fStopCalc_t m_fStopCalc;

  using mapOption_t = std::map<std::string,pOption_t>; // for m_fStartCalc, m_fStopCalc
  mapOption_t m_mapOption;

};

// == ManageStrategy

ManageStrategy::ManageStrategy(
  //const ou::tf::Bar& barPriorDaily,
  double dblSlope20DayUnderlying,
  pWatch_t pWatchUnderlying,
  pPortfolio_t pPortfolioOwning, // => owning portfolio
  pChartDataView_t pChartDataView,
  fGatherOptionDefinitions_t& fGatherOptionDefinitions,
  //fConstructWatch_t fConstructWatch, // => m_fConstructWatch underlying
  fConstructOption_t fConstructOption, // => m_fConstructOption
  fConstructPosition_t fConstructPosition, // => m_fConstructPosition
  fConstructPortfolio_t fConstructPortfolio, // => m_fConstructPortfolio
  fRegisterWatch_t fRegisterWatch, // => m_fRegisterWatch
  fRegisterOption_t&& fRegisterOption, // => m_fRegisterOption
  fStartCalc_t&& fStartCalc, // => m_fStartCalc
  fStopCalc_t&& fStopCalc, // => m_fStopCalc
  fFirstTrade_t fFirstTrade, // => m_fFirstTrade
  fAuthorizeUnderlying_t fAuthorizeUnderlying, // => m_fAuthorizeUnderlying
  fAuthorizeOption_t fAuthorizeOption, // => m_fAuthorizeOption
  fAuthorizeSimple_t fAuthorizeSimple, // => m_fAuthorizeSimple
  fBar_t fBar
  )
: ou::tf::DailyTradeTimeFrame<ManageStrategy>(),
  m_dblOpen {},
  m_dblSlope20DayUnderlying( dblSlope20DayUnderlying ),
  //m_barPriorDaily( barPriorDaily ),
  m_pWatchUnderlying( pWatchUnderlying ),
  m_pPortfolioOwning( pPortfolioOwning ),
  m_pChartDataView( pChartDataView ),

  //m_fConstructWatch( fConstructWatch ),
  m_fConstructOption( fConstructOption ),
  m_fConstructPosition( fConstructPosition ),
  m_fConstructPortfolio( fConstructPortfolio ),
  m_stateTrading( ETradingState::TSInitializing ),
  m_fRegisterWatch( fRegisterWatch ),
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

  m_ixColour {},
  m_bClosedItmLeg( false ), m_bAllowComboAdd( false ),
  m_ceShortEntries( ou::ChartEntryShape::EShort, ou::Colour::Red ),
  m_ceLongEntries( ou::ChartEntryShape::ELong, ou::Colour::Blue ),
  m_ceShortFills( ou::ChartEntryShape::EFillShort, ou::Colour::Red ),
  m_ceLongFills( ou::ChartEntryShape::EFillLong, ou::Colour::Blue ),
  m_ceShortExits( ou::ChartEntryShape::EShortStop, ou::Colour::Red ),
  m_ceLongExits( ou::ChartEntryShape::ELongStop, ou::Colour::Blue ),
  m_daysToExpiry( 1 ) // will be different for each strategy, to be deprecated
{
  assert( m_pWatchUnderlying );
  assert( m_pPortfolioOwning );

  assert( nullptr != fGatherOptionDefinitions );
  //assert( nullptr != m_fConstructWatch );
  assert( nullptr != m_fConstructOption );
  assert( nullptr != m_fConstructPosition );
  assert( nullptr != m_fConstructPortfolio );
  assert( nullptr != m_fFirstTrade );
  assert( nullptr != m_fBar );
  assert( pChartDataView );

  //m_rBarDirection[ 0 ] = EBarDirection::None;
  //m_rBarDirection[ 1 ] = EBarDirection::None;
  //m_rBarDirection[ 2 ] = EBarDirection::None;

  pChartDataView->SetNames( "Charts", m_pWatchUnderlying->GetInstrument()->GetInstrumentName() );

  //m_ceUpReturn.SetName( "Up Return" );
  //m_ceDnReturn.SetName( "Dn Return" );
  m_ceProfitLossPortfolio.SetName( "P/L Portfolio" );

  //m_ceUpReturn.SetColour( ou::Colour::Red );
  //m_ceDnReturn.SetColour( ou::Colour::Blue );
  m_ceProfitLossPortfolio.SetColour( ou::Colour::Fuchsia );

  pChartDataView->Add( EChartSlot::PL, &m_ceProfitLossPortfolio );

  //pChartDataView->Add( EChartSlot::Tick, &m_ceTickCount );

  //pChartDataView->Add( 4, &m_ceUpReturn );
  //pChartDataView->Add( 4, &m_ceDnReturn );

  pChartDataView->Add( EChartSlot::Price, &m_ceShortEntries );
  pChartDataView->Add( EChartSlot::Price, &m_ceLongEntries );
  pChartDataView->Add( EChartSlot::Price, &m_ceShortFills );
  pChartDataView->Add( EChartSlot::Price, &m_ceLongFills );
  pChartDataView->Add( EChartSlot::Price, &m_ceShortExits );
  pChartDataView->Add( EChartSlot::Price, &m_ceLongExits );

  m_pOptionRepository = std::make_unique<OptionRepository>(
    std::move( fRegisterOption ),
    std::move( fStartCalc ),
    std::move( fStopCalc )
  );

  m_bfQuotes01Sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarQuotes01Sec ) );
  m_bfTrades01Sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarTrades01Sec ) );
  m_bfTrades06Sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarTrades06Sec ) );
  //m_bfTicks06sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarTicks06Sec ) );
  //m_bfTrades60Sec.SetOnBarComplete( MakeDelegate( this, &ManageStrategy::HandleBarTrades60Sec ) );

  pInstrument_t pInstrumentUnderlying = m_pWatchUnderlying->GetInstrument();

  try {

    if ( 0 == pInstrumentUnderlying->GetContract() ) {
      std::cout << pInstrumentUnderlying->GetInstrumentName() << " has no contract" << std::endl;
      m_stateTrading = TSNoMore;
    }

    m_pOptionRepository->AssignWatchUnderlying( m_pWatchUnderlying );

    // collect option chains for the underlying
    // TODO: this will be passed in
    ou::tf::option::PopulateMap<mapChains_t>( m_mapChains, pWatchUnderlying->GetInstrument()->GetInstrumentName(), fGatherOptionDefinitions );

    assert( 0 != m_mapChains.size() );

    //m_fRegisterWatch( pWatchUnderlying );  // TODO: ensure this is performed in caller

    m_pWatchUnderlying->OnQuote.Add( MakeDelegate( this, &ManageStrategy::HandleQuoteUnderlying ) );
    m_pWatchUnderlying->OnTrade.Add( MakeDelegate( this, &ManageStrategy::HandleTradeUnderlying ) );

    m_pValidateOptions = std::make_unique<ValidateOptions>(
        m_pWatchUnderlying,
        m_mapChains,
        m_fConstructOption
      );
    m_pValidateOptions->SetSize( combo_t::LegCount() ); // will need to make this generic

  }
  catch (...) {
    std::cout << "*** " << "something wrong with " << pInstrumentUnderlying->GetInstrumentName() << " creation." << std::endl;
  }

}

ManageStrategy::~ManageStrategy( ) {

  m_pWatchUnderlying->OnQuote.Remove( MakeDelegate( this, &ManageStrategy::HandleQuoteUnderlying ) );
  m_pWatchUnderlying->OnTrade.Remove( MakeDelegate( this, &ManageStrategy::HandleTradeUnderlying ) );

  m_pCombo.reset();
  m_pOptionRepository.reset();
  m_vEMA.clear();
}

void ManageStrategy::Run() {

  m_stateTrading = TSWaitForFirstTrade;
  //std::cout << m_sUnderlying << " loading done." << std::endl;

}

// is this used currently?
ou::tf::DatedDatum::volume_t ManageStrategy::CalcShareCount( double dblFunds ) const {
  volume_t nOptionContractsToTrade {};
  const std::string& sUnderlying( m_pWatchUnderlying->GetInstrument()->GetInstrumentName() );
  if ( 0.0 != m_dblOpen ) {
    nOptionContractsToTrade = ( (volume_t)std::floor( dblFunds / m_dblOpen ) )/ 100;
    std::cout << sUnderlying << " funds on open: " << dblFunds << ", " << m_dblOpen << ", " << nOptionContractsToTrade << std::endl;
  }
  else {
    //nOptionContractsToTrade = ( (volume_t)std::floor( dblFunds / m_barPriorDaily.Close() ) )/ 100;
    nOptionContractsToTrade = 1;
    //std::cout << sUnderlying << " funds on bar close: " << dblFunds << ", " << m_barPriorDaily.Close() << ", " << nOptionContractsToTrade << std::endl;
  }

  volume_t nUnderlyingSharesToTrade = nOptionContractsToTrade * 100;  // round down to nearest 100
  std::cout << sUnderlying << " funds: " << nOptionContractsToTrade << ", " << nUnderlyingSharesToTrade << std::endl;
  return nUnderlyingSharesToTrade;
}

// add pre-existing positions from database
// NOTE: are there out of order problems, as Collar vLeg is ordered in a particular manner?  LegInfo may have now resolved this
void ManageStrategy::AddPosition( pPosition_t pPosition ) {

  pInstrument_t pInstrument = pPosition->GetInstrument();
  pWatch_t pWatch = pPosition->GetWatch();
  switch ( pInstrument->GetInstrumentType() ) {
    case ou::tf::InstrumentType::Stock:
      //assert( m_pPositionUnderlying );
      //assert( pPosition->GetInstrument()->GetInstrumentName() == m_pPositionUnderlying->GetInstrument()->GetInstrumentName() );
      //assert( pPosition.get() == m_pPositionUnderlying.get() );
      std::cout << "ManageStrategy::AddPosition adding underlying position, needs additional code: " << pInstrument->GetInstrumentName() << std::endl;
      try {
        //m_fRegisterWatch( pWatch );
      }
      catch( std::runtime_error& e ) {
        std::cout << e.what() << std::endl;
      }
      break;
    case ou::tf::InstrumentType::Option:
      if ( pPosition->IsActive() ) {

        const idPortfolio_t idPortfolio = pPosition->GetRow().idPortfolio;

        combo_t* pCombo;
        if ( m_pCombo ) { // need to construct empty combo when first leg presented
          pCombo = &dynamic_cast<combo_t&>( *m_pCombo );
          assert( pCombo );
        }
        else {

          m_pCombo = std::make_unique<combo_t>();
          pCombo = &dynamic_cast<combo_t&>( *m_pCombo );
          assert( pCombo );

          pCombo->SetPortfolio( m_fConstructPortfolio( idPortfolio, m_pPortfolioOwning->Id() ) );

        }

        pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( pWatch );
        m_pOptionRepository->Add( pOption );

        std::cout << "set combo position existing: " << pWatch->GetInstrument()->GetInstrumentName() << std::endl;

        // TODO: may need special call for colour for non-Open positions
        using LegNote = ou::tf::option::LegNote;
        const LegNote::values_t& lnValues = pCombo->SetPosition( pPosition, m_pChartDataView, rColour[ m_ixColour++ ] );
        if ( LegNote::State::Open == lnValues.m_state ) {
          m_fAuthorizeSimple( idPortfolio, pInstrument->GetInstrumentName(), true ); // update count
        }
        else {
          m_ixColour--;
        }
      }
      break;
  }
}

void ManageStrategy::ClosePositions( void ) {
  std::cout << m_pWatchUnderlying->GetInstrument()->GetInstrumentName() << " close positions" << std::endl;
  if ( m_pCombo ) {
    combo_t& combo( dynamic_cast<combo_t&>( *m_pCombo ) );
    combo.CancelOrders(); // TODO: generify via Common or Base
    combo.ClosePositions(); // TODO: generify via Common or Base
  }
}

void ManageStrategy::HandleBellHeard( boost::gregorian::date, boost::posix_time::time_duration ) {
}

void ManageStrategy::HandleQuoteUnderlying( const ou::tf::Quote& quote ) {
//  if ( quote.IsValid() ) {  // far out of the money options have a 0.0 bid
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

      m_stateTrading = TSOptionEvaluation; // ready to trade
      }
      break;
//    case TSWaitForFundsAllocation: // Start() needs to be called
//      break;
    case TSWaitForEntry:
      break;
    case TSOptionEvaluation:
      break;
//    case TSMonitorCombo:
//      break;
    case TSMonitorLong: {
      pEMA_t& pEMA( m_vEMA.back() );
      if ( trade.Price() < pEMA->dblEmaLatest ) {
        //m_pPositionUnderlying->ClosePosition( ou::tf::OrderType::Market );
        //m_ceLongExits.AddLabel( trade.DateTime(), trade.Price(), "Stop" );
        //std::cout << m_sUnderlying << " closing long" << std::endl;
        m_stateTrading = TSWaitForEntry;
      }
      }
      break;
    case TSMonitorShort: {
      pEMA_t& pEMA( m_vEMA.back() );
      if ( trade.Price() > pEMA->dblEmaLatest ) {
        //m_pPositionUnderlying->ClosePosition( ou::tf::OrderType::Market );
        //m_ceShortExits.AddLabel( trade.DateTime(), trade.Price(), "Stop" );
        //std::cout << m_sUnderlying << " closing short" << std::endl;
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

  //const double mid = m_QuoteUnderlyingLatest.Midpoint();
  // BollingerTransitions::Crossing( mid ) // TODO: needs to be migrated to Underlying

  RHOption( bar );
}

// turn into a template if needed for other combo types
// Not used at this point
void ManageStrategy::BuildPosition(
  const idPortfolio_t& idPortfolio,
  boost::gregorian::date date,
  ou::tf::OptionSide::enumOptionSide side, double price,
  fBuildPositionCallBack_t&& fBuildPositionCallBack
) {

  mapChains_t::const_iterator iter = std::find_if( m_mapChains.begin(), m_mapChains.end(),
    [this,date](const mapChains_t::value_type& vt)->bool{
      return m_daysToExpiry <= ( vt.first - date );  // first chain where trading date less than expiry date
  } );

  const chain_t& chain( iter->second );

  std::string sIQFeedOptionCode;

  switch ( side ) { // should this be here or in the caller?
    case ou::tf::OptionSide::Call:
      sIQFeedOptionCode = chain.GetIQFeedNameCall( chain.Call_Otm( price ) );
      break;
    case ou::tf::OptionSide::Put:
      sIQFeedOptionCode = chain.GetIQFeedNamePut( chain.Put_Otm( price ) );
      break;
  }

  m_fConstructOption(
    sIQFeedOptionCode,
    m_pWatchUnderlying->GetInstrument(),
    [this,f=std::move(fBuildPositionCallBack),&idPortfolio]( pOption_t pOption ){
      m_pOptionRepository->Add( pOption );
      pPosition_t pPosition = m_fConstructPosition( idPortfolio, pOption, "" );
      f( pPosition, m_pChartDataView, rColour[ m_ixColour++ ] );
    });
}

/*
 * TODO:
 *   split out each option strategy
 *   add additional states
* rules:
 *   expiry day:
 *     atm, itm : roll, same strike
 *     otm: expire
 *          re-enter at atm?
 *          zero-price, zero-cost close/expire for the position
 */

void ManageStrategy::RHOption( const ou::tf::Bar& bar ) { // assumes one second bars, currently a bar of quote spreads

  // what happens with Add()?  What state are we in?  What states are executed to reach here?
  // need to determine states and then sequence to get the combo initialized

  const double mid = m_QuoteUnderlyingLatest.Midpoint();

  switch ( m_stateTrading ) {
    case TSOptionEvaluation:
      {

        if ( !m_pCombo ) {
          m_bAllowComboAdd = true;
          // also might see if combo is 'live' or not
        }

        if ( m_bAllowComboAdd ) {

          const std::string& sUnderlying( m_pWatchUnderlying->GetInstrument()->GetInstrumentName() );

          try {
            const ou::tf::option::Combo::E20DayDirection direction
              = ( 0.0 <= m_dblSlope20DayUnderlying )
              ? ou::tf::option::Combo::E20DayDirection::Rising
              : ou::tf::option::Combo::E20DayDirection::Falling;
            const boost::gregorian::date dateBar( bar.DateTime().date() );
            if ( m_pValidateOptions->ValidateBidAsk(
              dateBar, mid, 11,
              [mid,direction]( const mapChains_t& chains, boost::gregorian::date date, double price, combo_t::fLegSelected_t&& fLegSelected ){
                combo_t::ChooseLegs( direction, chains, date, mid, fLegSelected );
              }
            ) ) {

              // for a collar, always enter long, composition of legs indicates rising or falling momentum

              const idPortfolio_t idPortfolio
                = combo_t::Name( sUnderlying, m_mapChains, dateBar, mid, direction );  // "collar-GLD-rise-20210730-165-20210630-167-165"

              if ( m_fAuthorizeSimple( idPortfolio, sUnderlying, false ) ) {

                m_bAllowComboAdd = false;

                std::cout << sUnderlying << ": bid/ask spread ok, opening positions (slope=" << m_dblSlope20DayUnderlying << ")" << std::endl;

                m_pCombo = std::make_unique<combo_t>();
                assert( m_pCombo );
                combo_t& combo = dynamic_cast<combo_t&>( *m_pCombo );

                if ( m_ixColour >= ( sizeof( rColour ) - 2 ) ) {
                  std::cout << "WARNING: strategy running out of colours." << std::endl;
                }
                std::cout << sUnderlying << " construct portfolio: " << m_pPortfolioOwning->Id() << " adds " << idPortfolio << std::endl;
                combo.SetPortfolio( m_fConstructPortfolio( idPortfolio, m_pPortfolioOwning->Id() ) );

                m_pValidateOptions->Get(
                  [this,&idPortfolio,&combo,direction](size_t ix,pOption_t pOption){  // need Strategy specific naming

                    m_pOptionRepository->Add( pOption );

                    ou::tf::option::LegNote::values_t lnValues;
                    combo_t::FillLegNote( ix, direction, lnValues );
                    ou::tf::option::LegNote ln( lnValues );
                    pPosition_t pPosition = m_fConstructPosition( idPortfolio, pOption, ln.Encode() );
                    assert( pPosition );
                    combo.SetPosition( pPosition, m_pChartDataView, rColour[ m_ixColour++ ] );
                    }
                  );

                m_pValidateOptions->ClearValidation(); // after positions created to keep watch in options from a quick stop/start

                combo.PlaceOrder( ou::tf::OrderSide::Buy, 1 );
                m_stateTrading = ETradingState::TSComboPrepare;

              } // m_fAuthorizeSimple
              else {
                // ?
              }
              // TODO: re-use existing combo?  what if leg is still active? add one or both legs?  if not profitable, no use adding to loss leg
              // TODO: create a trailing stop based upon entry net loss?
            }
          }
          catch ( const ou::tf::option::exception_strike_range_exceeded& e ) {
            // don't worry about this, price is not with in range yet
          }
          catch ( const std::runtime_error& e ) {
            std::cout << sUnderlying << " run time error, stop trading: " << e.what() << std::endl;
            m_pValidateOptions->ClearValidation();
            m_stateTrading = TSNoMore;  // TODO: fix this for multiple combos in place
          }
        } // m_bAllowedComboAdd
        else {
          m_stateTrading = ETradingState::TSComboPrepare;  // state machine needs to be confirmed
        }

      }
      break;
    case TSComboPrepare:
      {
        const std::string& sUnderlying( m_pWatchUnderlying->GetInstrument()->GetInstrumentName() );

        std::cout << "TSComboPrepare: " << sUnderlying << std::endl;
        const boost::gregorian::date dateBar( bar.DateTime().date() );

        m_pCombo->Prepare( // fConstructOption_t?
          dateBar, &m_mapChains,
          [this]( const std::string& sOptionName, combo_t::fConstructedOption_t&& fConstructedOption ){
            // TODO: maintain a local map for quick reference
            m_fConstructOption(
              sOptionName,
              m_pWatchUnderlying->GetInstrument(),
              [ f=std::move( fConstructedOption ) ]( pOption_t pOption ){
                f( pOption );
              }
            );
          },
          [this]( ou::tf::option::Combo* p, pOption_t pOption, const std::string& note )->pPosition_t { // fOpenPosition_t
            combo_t* pCombo = reinterpret_cast<combo_t*>( p );
            m_pOptionRepository->Add( pOption );
            pPosition_t pPosition = m_fConstructPosition( pCombo->GetPortfolio()->GetRow().idPortfolio, pOption, note );
            using LegNote = ou::tf::option::LegNote;
            const LegNote::values_t& lnValues = pCombo->SetPosition( pPosition, m_pChartDataView, rColour[ m_ixColour++ ] );
            pCombo->PlaceOrder( ou::tf::OrderSide::Buy, 1, lnValues.m_type );  // TODO: perform this in the combo, rename to AddPosition?
            return pPosition;
          },
          [this]( pPosition_t pPosition ){ // fRemovePosition_t
            pWatch_t pWatch = pPosition->GetWatch();
            assert( pWatch->GetInstrument()->IsOption() );
            pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( pWatch );
            m_pOptionRepository->Remove( pOption );
          }
          ); // Prepare
        m_stateTrading = ETradingState::TSComboMonitor;
      }
      break;
//    case TSWaitForEntry:
//      break;
    case TSComboMonitor:
      {
        // TODO: track pivot crossing, track momentun
        //   pivot acts as stop, momentum can carry through
        //     if momentum changes first, then roll
        //     if momentum doesn't change, but trades back over pivot, then roll
        //     don't roll if not profitable yet (commision plus bid/ask spread )

        //PivotCrossing::ECrossing crossing = m_pivotCrossing.Update( mid );
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

        if ( 4 == m_vEMA.size() ) { // on second day, is m_vEMA built?
          double slope( m_vEMA[2]->dblEmaLatest - m_vEMA[3]->dblEmaLatest ); // fast - slow
          switch ( m_pCombo->m_state ) {
            case combo_t::State::Initializing:
              break;
            case combo_t::State::Positions:
            case combo_t::State::Executing:
            case combo_t::State::Watching:
            case combo_t::State::Canceled:
            case combo_t::State::Closing:
              // TODO: maybe try send stochastic as well
              m_pCombo->Tick( slope, mid, bar.DateTime() ); // TODO: need to pass slope of underlying
              break;
            default:
              break;
          }
        }

      }
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
        std::cout << m_pWatchUnderlying->GetInstrument()->GetInstrumentName() << ": bAllRising && bAllFalling" << std::endl;
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
//                m_pPositionUnderlying->CancelOrders();
//                m_pPositionUnderlying->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, m_nSharesToTrade );
//                m_ceLongEntries.AddLabel( bar.DateTime(), bar.Close(), "Long" );
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
//                m_pPositionUnderlying->CancelOrders();
//                m_pPositionUnderlying->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, m_nSharesToTrade );
//                m_ceShortEntries.AddLabel( bar.DateTime(), bar.Close(), "Short" );
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
void ManageStrategy::HandleCancel( boost::gregorian::date, boost::posix_time::time_duration ) {
  switch ( m_stateTrading ) {
    case TSNoMore:
      break;
    default:
      {
        std::cout << m_pWatchUnderlying->GetInstrument()->GetInstrumentName() << " cancel" << std::endl;
        if ( m_pCombo ) {
          //if ( m_pPositionUnderlying ) m_pPositionUnderlying->CancelOrders();
          combo_t& combo = dynamic_cast<combo_t&>( *m_pCombo );
          //entry.second.ClosePositions();
          combo.CancelOrders(); // TODO: generify via Common or Base
        }
      }
      break;
  }
}

// one shot, 3 minutes, 45 seconds prior to close
void ManageStrategy::HandleGoNeutral( boost::gregorian::date date, boost::posix_time::time_duration time ) {
  switch ( m_stateTrading ) {
    case TSNoMore:
      break;
    default:
//      std::cout << m_sUnderlying << " go neutral" << std::endl;
      if ( m_pCombo ) {
        m_pCombo->GoNeutral( date, time );
      }
      break;
  }
}

void ManageStrategy::HandleGoingNeutral( const ou::tf::Bar& bar ) {
  RHOption( bar );
}

void ManageStrategy::HandleAtRHClose( boost::gregorian::date, boost::posix_time::time_duration ) {
  switch ( m_stateTrading ) {
    case TSNoMore:
      break;
    default:
      if ( m_pCombo ) {
        m_pCombo->AtClose();      }
      break;
  }
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
  // TODO: pWatchUnderlying should be saved in caller hierarchy
  if ( m_pCombo ) {
    combo_t& combo( dynamic_cast<combo_t&>( *m_pCombo ) );
    //entry.second.ClosePositions();
    combo.SaveSeries( sPrefix ); // TODO: generify via Common or Base
  }
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

  //m_cePrice.AppendBar( bar );
  //m_ceVolume.Append( bar );

//  m_rBarDirection[ 0 ] = m_rBarDirection[ 1 ];
//  m_rBarDirection[ 1 ] = m_rBarDirection[ 2 ];
//  m_rBarDirection[ 2 ] = ( bar.Open() == bar.Close() ) ? EBarDirection::None : ( ( bar.Open() < bar.Close() ) ? EBarDirection::Up : EBarDirection::Down );

  double dblUnRealized;
  double dblRealized;
  double dblCommissionsPaid;
  double dblTotal;

  m_pPortfolioOwning->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );
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
    chain_t& chain( m_mapChains.begin()->second );
    chain.EmitValues();
    chain.Test( 121.5 );
  }
}

double ManageStrategy::EmitInfo() {
  double dblNet {};
  if ( m_pCombo ) {
    double price( m_pWatchUnderlying->LastTrade().Price() );
    combo_t& combo( dynamic_cast<combo_t&>( *m_pCombo ) );
    std::cout
      << "Info "
      << m_pWatchUnderlying->GetInstrument()->GetInstrumentName()
      << "@" << price
      << std::endl;

    std::cout << "  portfolio: " << combo.GetPortfolio()->Id() << std::endl;
    std::cout << "  underlying: ";
    m_pWatchUnderlying->EmitValues( true );
    std::cout << std::endl;

    dblNet += combo.GetNet( price );
    std::cout << "  net: " << dblNet << std::endl;
  }
  return dblNet;
}

void ManageStrategy::CloseExpiryItm( boost::gregorian::date date ) {
  double price( m_TradeUnderlyingLatest.Price() );
  if ( 0.0 != price ) {
    if ( m_pCombo ) {
      combo_t& combo( dynamic_cast<combo_t&>( *m_pCombo ) );
      combo.CloseExpiryItm( price, date );
    }
  }
}

void ManageStrategy::CloseFarItm() {
  double price( m_TradeUnderlyingLatest.Price() );
  if ( 0.0 != price ) {
    if ( m_pCombo ) {
      combo_t& combo( dynamic_cast<combo_t&>( *m_pCombo ) );
      combo.CloseFarItm( price );
    }
  }
}

void ManageStrategy::CloseItmLeg() {
  double price( m_TradeUnderlyingLatest.Price() );
  if ( 0.0 != price ) {
    if ( m_pCombo ) {
      combo_t& combo( dynamic_cast<combo_t&>( *m_pCombo ) );
      m_bClosedItmLeg |= combo.CloseItmLeg( price );
    }
  }
}

void ManageStrategy::AddCombo( bool bForced ) {
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
  if ( 0.0 != price ) {
    if ( m_pCombo ) {
      combo_t& combo( dynamic_cast<combo_t&>( *m_pCombo ) );
      combo.CloseForProfits( price );
    }
  }
}

void ManageStrategy::TakeProfits() {
  double price( m_TradeUnderlyingLatest.Price() );
  if ( 0.0 != price ) {
    if ( m_pCombo ) {
      combo_t& combo( dynamic_cast<combo_t&>( *m_pCombo ) );
      combo.TakeProfits( price );
    }
  }
}

