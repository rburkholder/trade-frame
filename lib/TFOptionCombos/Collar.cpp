/************************************************************************
 * Copyright(c) 2020, One Unified. All rights reserved.                 *
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

/*
 * File:    Collar.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created: July 19, 2020, 05:43 PM
 */

#include <array>

#include <TFTrading/PortfolioManager.h>

#include "LegDef.h"
#include "Collar.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

namespace {

  static const size_t nLegs( 4 );

  using LegDef = ou::tf::option::LegDef;
  using rLegDef_t = std::array<LegDef,nLegs>;

  static const rLegDef_t m_rLegDefRise = { // rising momentum
    LegDef( 1, LegNote::Type::SynthLong,  LegNote::Side::Long,  LegNote::Option::Call ), // synthetic long
    LegDef( 1, LegNote::Type::SynthShort, LegNote::Side::Short, LegNote::Option::Put  ), // synthetic long
    LegDef( 1, LegNote::Type::Cover,      LegNote::Side::Short, LegNote::Option::Call ), // covered
    LegDef( 1, LegNote::Type::Protect,    LegNote::Side::Long,  LegNote::Option::Put  )  // protective
  };
  static const rLegDef_t m_rLegDefFall = { // falling momentum
    LegDef( 1, LegNote::Type::SynthLong,  LegNote::Side::Long,  LegNote::Option::Put  ), // synthetic short
    LegDef( 1, LegNote::Type::SynthShort, LegNote::Side::Short, LegNote::Option::Call ), // synthetic short
    LegDef( 1, LegNote::Type::Cover,      LegNote::Side::Short, LegNote::Option::Put  ), // covered
    LegDef( 1, LegNote::Type::Protect,    LegNote::Side::Long,  LegNote::Option::Call )  // protective
  };

} // namespace anon

Collar::Collar()
: Combo() {}

Collar::Collar( Collar&& rhs )
: m_mapCollarLeg( std::move( rhs.m_mapCollarLeg ) ),
  m_mapInitTrackOption( std::move( rhs.m_mapInitTrackOption ) )
{}

Collar::~Collar() {
  m_mapCollarLeg.clear();
}

// needs to happen before all Legs have been created
// called from Combo::Prepare
void Collar::Init( boost::gregorian::date date, const mapChains_t* pmapChains, const SpreadSpecs& specs ) {

  // TODO: check if position is active prior to Initialize
  // TODO: so much happening, almost ready to start firing events on state change
  // TODO: refactor date, pmapChains

  // === vertical/diagonal roll for profitable long synthetic when trend is in wrong direction
  m_mapInitTrackOption.emplace(
    std::make_pair(
      LegNote::Type::SynthLong,
      [this,date,pmapChains,days=specs.nDaysBack](){
        // TODO: check that the leg is active
        InitTrackLongOption( LegNote::Type::SynthLong, pmapChains, date, days );
      }
    )
  );

  // === vertical/diagonal roll for profitable long protective when trend is in wrong direction
  m_mapInitTrackOption.emplace(
    std::make_pair(
      LegNote::Type::Protect,
      [this,date,pmapChains,days=specs.nDaysFront](){
        // TODO: check that the leg is active
        InitTrackLongOption( LegNote::Type::Protect, pmapChains, date, days );
      }
    )
  );

  // === close out at 0.10
  m_mapInitTrackOption.emplace(
    std::make_pair(
      LegNote::Type::SynthShort,
      [this,date,pmapChains,days= specs.nDaysBack](){
        // TODO: check that the leg is active
        InitTrackShortOption( LegNote::Type::SynthShort, pmapChains, date, days );
      }
    )
  );

  // === close out at 0.10
  m_mapInitTrackOption.emplace(
    std::make_pair(
      LegNote::Type::Cover,
      [this,date,pmapChains,days=specs.nDaysFront](){
        // TODO: check that the leg is active
        InitTrackShortOption( LegNote::Type::Cover, pmapChains, date, days );
      }
    )
  );

}

Collar::CollarLeg& Collar::InitTracker(
  LegNote::Type type,
  const mapChains_t* pmapChains,
  boost::gregorian::date date,
  boost::gregorian::days days_to_expiry
) {

  mapCollarLeg_t::iterator iterMapCollarLeg = m_mapCollarLeg.find( type );
  if ( m_mapCollarLeg.end() == iterMapCollarLeg ) {
    auto pair = m_mapCollarLeg.emplace( std::make_pair( type, CollarLeg() ) );
    assert( pair.second );
    iterMapCollarLeg = pair.first;
  }
  CollarLeg& cleg( iterMapCollarLeg->second );

  pPosition_t pPosition( m_mapLeg[type].GetPosition() );
  assert( pPosition );
  citerChain_t citerChain = Combo::SelectChain( *pmapChains, date, days_to_expiry );
  const chain_t& chain( citerChain->second );

  cleg.m_tracker.Initialize(
    pPosition, &chain,
    [this]( const std::string& sName, fConstructedOption_t&& f ){ // m_fConstructOption
      m_fConstructOption( sName, std::move( f ) );
      },
    [this,&cleg]( pPosition_t pPositionOld ) { // m_fCloseLeg

      const std::string sNotes( pPositionOld->Notes() );
      LegNote ln( sNotes );

      LegNote::values_t values( ln.Values() );
      values.m_state = LegNote::State::Closed;
      ln.Assign( values );
      pPositionOld->SetNotes( ln.Encode() );
      auto& instance( ou::tf::PortfolioManager::Instance() ); // NOTE this direct call!!
      instance.PositionUpdateNotes( pPositionOld );

      cleg.m_monitor.SetPosition( pPositionOld );
      cleg.m_monitor.ClosePosition();

    },
    [this]( pOption_t pOption, const std::string& sNotes )->pPosition_t { // m_fOpenLeg

      // TODO: will need to supply previous option => stop calc, may need a clean up lambda
      //   then the note change above can be performed elsewhere

      pPosition_t pPosition = m_fOpenPosition( this, pOption, sNotes );
      // Combo::OverwritePosition( pPosition ); - not needed, performed in fOpenPosition
      return pPosition;
    }
  );

  return cleg;

}

// NOTE: may require delayed reaction on this, as a roll will call back into this with new position
void Collar::InitTrackLongOption(
    LegNote::Type type,
    const mapChains_t* pmapChains,
    boost::gregorian::date date,
    boost::gregorian::days days_to_expiry
    ) {

  CollarLeg& cleg( InitTracker( type, pmapChains, date, days_to_expiry ) );

  cleg.vfTest.emplace( // invalidates on new size() > capacity()
    cleg.vfTest.end(),
    [ tracker = &cleg.m_tracker ]( boost::posix_time::ptime dt, double dblUnderlyingSlope, double dblUnderlyingPrice ){
      tracker->TestLong( dt, dblUnderlyingSlope, dblUnderlyingPrice );
    }
  );
}

void Collar::InitTrackShortOption(
    LegNote::Type type,
    const mapChains_t* pmapChains,
    boost::gregorian::date date,
    boost::gregorian::days days_to_expiry
) {

  CollarLeg& cleg( InitTracker( type, pmapChains, date, days_to_expiry ) );

  // a) buy out 0.10 (simply closing the position)
  // b) rotate if itm (somewhere else, affects long & short)
  // c) stop monitoring out of hours

  cleg.vfTest.emplace(
    cleg.vfTest.end(),
    [ tracker = &cleg.m_tracker ]( boost::posix_time::ptime dt,double dblUnderlyingSlope, double dblUnderlyingPrice ){
      tracker->TestShort( dt, dblUnderlyingSlope, dblUnderlyingPrice );
    }
  );

}

void Collar::Init( LegNote::Type type ) {
  mapInitTrackOption_t::iterator iter = m_mapInitTrackOption.find( type );
  assert( m_mapInitTrackOption.end() != iter );
  iter->second();
}

void Collar::CancelOrders() {
  Combo::CancelOrders();
  for ( mapCollarLeg_t::value_type& cleg: m_mapCollarLeg ) {
    cleg.second.m_tracker.Quiesce();
    cleg.second.m_monitor.CancelOrder(); // or wait for completion?
  }
}

void Collar::GoNeutral( boost::gregorian::date date, boost::posix_time::time_duration time ) {
  // relies on tracker having been quiesced
  // TODO: is the tracker/position active?
  for ( mapCollarLeg_t::value_type& cleg: m_mapCollarLeg ) {
    cleg.second.m_tracker.TestItmRoll( date, time );
  }
}

// TODO: need to disable Tracker monitoring out of hours
void Collar::AtClose() {
  // maybe remove options?
}

void Collar::Tick( double dblUnderlyingSlope, double dblUnderlyingPrice, ptime dt ) {
  Combo::Tick( dblUnderlyingSlope, dblUnderlyingPrice, dt ); // first or last in sequence?

  for ( mapCollarLeg_t::value_type& entry: m_mapCollarLeg ) {
    CollarLeg& cleg( entry.second );
    if ( cleg.m_monitor.IsOrderActive() ) cleg.m_monitor.Tick( dt );

    for ( vfTest_t::value_type& fTest: cleg.vfTest ) {
      fTest( dt, dblUnderlyingSlope, dblUnderlyingPrice );
    }
  }

  // TODO:
  //   at expiry:
  //     otm: accounting adjustment for expiring, enter new position (itm or at expiring strike?)
  //     itm: horizontal calendar roll
  //   buy back short options at 0.10? using GTC trade? (0.10 is probably easier) -- don't bother at expiry
  //     re-enter, or just keep the leg expired?
  //  position note needs to be updated on roll, and such

  // manual accounting:
  //  was otm, but crossed itm and was assigned

  // 2021/01/03 set old positions to State=Expired/Closed
  //  * trigger on Leg over-write
  //  * should always have four active legs

  // 2021/01/03 need to close combos, and no longer load them
  //  those in the correct direction, keep
  //  those in the wrong direction, close

  // To Consider:

  // 2020/11/12 when rolling a call up, buy a put for the downward journey?
  //            when rolling a put down, buy a call for the upward journey?
  // 2020/11/12 upon return to original strike, buy in again?

  // change 20 day slope to 5 day slope

  // when closing a dead short, consider going long on the same price (buy-out then buy-in)

}

size_t /* static */ Collar::LegCount() {
  return nLegs;
}

/* static */ void Collar::ChooseLegs( // throw Chain exceptions
    Combo::E20DayDirection direction,
    const mapChains_t& chains,
    boost::gregorian::date date,
    double priceUnderlying,
    const SpreadSpecs& specs,
    const fLegSelected_t& fLegSelected
)
{

  citerChain_t citerChainSynthetic = Combo::SelectChain( chains, date, specs.nDaysBack );
  const chain_t& chainSynthetic( citerChainSynthetic->second );

  citerChain_t citerChainFront = Combo::SelectChain( chains, date, specs.nDaysFront );
  const chain_t& chainFront( citerChainFront->second );

  switch ( direction ) {
    case E20DayDirection::Unknown:
      break;
    case E20DayDirection::Rising:
      {
        double strikeSyntheticItm( chainSynthetic.Call_Itm( priceUnderlying ) );

        double strikeCovered( chainFront.Call_Otm( strikeSyntheticItm ) );
        strikeCovered = chainFront.Call_Otm( strikeCovered ); // two strikes up

        double strikeProtective( chainFront.Put_Atm( strikeSyntheticItm ) ); // rounding problem across chains

        fLegSelected( strikeSyntheticItm, citerChainSynthetic->first, chainSynthetic.GetIQFeedNameCall( strikeSyntheticItm ) );
        fLegSelected( strikeSyntheticItm, citerChainSynthetic->first, chainSynthetic.GetIQFeedNamePut(  strikeSyntheticItm ) );
        fLegSelected( strikeCovered,      citerChainFront->first,         chainFront.GetIQFeedNameCall( strikeCovered ) );
        fLegSelected( strikeProtective,   citerChainFront->first,         chainFront.GetIQFeedNamePut(  strikeProtective ) );
      }
      break;
    case E20DayDirection::Falling:
      {
        double strikeSyntheticItm( chainSynthetic.Put_Itm( priceUnderlying ) );

        double strikeCovered( chainFront.Put_Otm( strikeSyntheticItm ) );
        strikeCovered = chainFront.Put_Otm( strikeCovered ); // two strikes down

        double strikeProtective( chainFront.Call_Atm( strikeSyntheticItm ) ); // rounding problem across chains

        fLegSelected( strikeSyntheticItm, citerChainSynthetic->first, chainSynthetic.GetIQFeedNamePut(  strikeSyntheticItm ) );
        fLegSelected( strikeSyntheticItm, citerChainSynthetic->first, chainSynthetic.GetIQFeedNameCall( strikeSyntheticItm ) );
        fLegSelected( strikeCovered,      citerChainFront->first,         chainFront.GetIQFeedNamePut(  strikeCovered ) );
        fLegSelected( strikeProtective,   citerChainFront->first,         chainFront.GetIQFeedNameCall( strikeProtective ) );
      }
      break;
  }
}

/* static */ void Collar::FillLegNote( size_t ix, Combo::E20DayDirection direction, LegNote::values_t& values ) {

  assert( ix < nLegs );

  values.m_algo = LegNote::Algo::Collar;
  values.m_state = LegNote::State::Open;

  switch ( direction ) {
    case E20DayDirection::Unknown:
      break;
    case E20DayDirection::Rising:
      values.m_momentum = LegNote::Momentum::Rise;
      values.m_type     = m_rLegDefRise[ix].type;
      values.m_side     = m_rLegDefRise[ix].side;
      values.m_option   = m_rLegDefRise[ix].option;
      break;
    case E20DayDirection::Falling:
      values.m_momentum = LegNote::Momentum::Fall;
      values.m_type     = m_rLegDefFall[ix].type;
      values.m_side     = m_rLegDefFall[ix].side;
      values.m_option   = m_rLegDefFall[ix].option;
      break;
  }

}

/* static */ const std::string Collar::Name(
     const std::string& sUnderlying,
     const mapChains_t& chains,
     boost::gregorian::date date,
     double price,
     Combo::E20DayDirection direction,
     const SpreadSpecs& specs
     ) {

  std::string sName( "collar-" + sUnderlying );
  size_t ix {};

  switch ( direction ) {
    case Combo::E20DayDirection::Rising:
      sName += "-rise";
      break;
    case Combo::E20DayDirection::Falling:
      sName += "-fall";
      break;
  }

  ChooseLegs(
    direction, chains, date, price, specs,
    [&sName,&ix]( double strike, boost::gregorian::date date, const std::string& sIQFeedName ){
      switch ( ix ) {
        case 0:
          sName
            += "-"
            +  ou::tf::Instrument::BuildDate( date.year(), date.month(), date.day() )
            +  "-"
            +  boost::lexical_cast<std::string>( strike );
          break;
        case 1:
          break;
        case 2:
          sName
            += "-"
            +  ou::tf::Instrument::BuildDate( date.year(), date.month(), date.day() )
            +  "-"
            +  boost::lexical_cast<std::string>( strike );
          break;
        case 3:
          sName
            += "-"
            +  boost::lexical_cast<std::string>( strike );
          break;
      }
      ix++;
    }
    );
  return sName;
}

void Collar::PlaceOrder( ou::tf::OrderSide::enumOrderSide side, uint32_t nOrderQuantity ) {
  switch ( m_state ) {
    case State::Positions: // doesn't confirm both put/call are available
    case State::Watching:
      switch ( side ) {
        case ou::tf::OrderSide::Buy:
          m_mapLeg[ LegNote::Type::SynthLong ].PlaceOrder( ou::tf::OrderSide::Buy, nOrderQuantity );
          m_mapLeg[ LegNote::Type::SynthShort ].PlaceOrder( ou::tf::OrderSide::Sell, nOrderQuantity );
          m_mapLeg[ LegNote::Type::Cover ].PlaceOrder( ou::tf::OrderSide::Sell, nOrderQuantity );
          m_mapLeg[ LegNote::Type::Protect ].PlaceOrder( ou::tf::OrderSide::Buy, nOrderQuantity );
          break;
        case ou::tf::OrderSide::Sell:
          m_mapLeg[ LegNote::Type::SynthLong ].PlaceOrder( ou::tf::OrderSide::Sell, nOrderQuantity );
          m_mapLeg[ LegNote::Type::SynthShort ].PlaceOrder( ou::tf::OrderSide::Buy, nOrderQuantity );
          m_mapLeg[ LegNote::Type::Cover ].PlaceOrder( ou::tf::OrderSide::Buy, nOrderQuantity );
          m_mapLeg[ LegNote::Type::Protect ].PlaceOrder( ou::tf::OrderSide::Sell, nOrderQuantity );
          break;
      }
      m_state = State::Executing;
      break;
  }
}

void Collar::PlaceOrder( ou::tf::OrderSide::enumOrderSide order_side, uint32_t nOrderQuantity, LegNote::Type type ) {

  LegNote::Side ln_side = m_mapLeg[type].GetLegNote().Values().m_side; // this is normal entry with order_side as buy

  if ( ou::tf::OrderSide::Buy == order_side ) {
    switch ( ln_side ) { // normal mapping
      case LegNote::Side::Long:
        order_side = ou::tf::OrderSide::Buy;
        break;
      case LegNote::Side::Short:
        order_side = ou::tf::OrderSide::Sell;
        break;
    }
  }
  else { // reverse the mapping
    switch ( ln_side ) {
      case LegNote::Side::Long:
        order_side = ou::tf::OrderSide::Sell;
        break;
      case LegNote::Side::Short:
        order_side = ou::tf::OrderSide::Buy;
        break;
    }
  }

  switch ( m_state ) {
    case State::Positions: // doesn't confirm both put/call are available
    case State::Watching:
      m_mapLeg[type].PlaceOrder( order_side, nOrderQuantity );
      m_state = State::Executing;
      break;
  }
}

} // namespace option
} // namespace tf
} // namespace ou

/*

2020/11/09

Use Stochastic for entry at the edges, and improve the mean reversion

2020/07/13

long collar: synthetic long, covered call, long put
short collar: synthetic short, covered put, long call

trigger when to rollup / rolldown, then watch sma crossover for actual execution
need to flag for day cross over, if it doesn't happen with in the trading day

synthetic long:  roll call up to covered call (try to do at maximum premium)
synthetic short: roll put down to covered put (try to do at maximum premium)

long put: roll down every two strikes (on the sma xover)
long call: roll up every two strikes (on the sma xover)

covered call: close out at 10%
covered put: close out at 10%

==

(upward trend) start with the synthetic long, test handling the adjustments
(downward trend) start with the synthetic short, test handling the adjustments

exit on some sort of profit
watch for xover for exit at chosen position

therefore double trigger: basic algorithm, then watch for the subsequent sma xover (if it hasn't happened yet)

==

need end of day clean up, roll to current strike

need to add trend xover for IV a) roll longs on higher IV, b) exit shorts on lower IV

could use db entries to track hi/lo IV, delta, gamma at entry/exit/rolls, or record with transaction comment

*/
