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

/*
978-1499206074 2014 Trading Volatility, Correlation, Term Structure and Skew by Colin Bennett.pdf

Collars are more sensitive to skew than implied volatility page 26

Selling a call against a long put suffers from buying skew. The effect of skew is greater for a
collar than for a put spread, as skew affects both legs of the structure the same way (whereas
the effect of skew on the long and short put of a put spread partly cancels). If skew was flat,
the cost of a collar typically reduces by 1% of spot. The level of volatility for near-zero cost
collars is not normally significant, as the long volatility of the put cancels the short volatility
of the call.

Capping performance should only be used when a long rally is unlikely

A collar or put spread collar caps the performance of the portfolio at the strike of the OTM
call sold. They should only therefore be used when the likelihood of a strong, long-lasting
rally (or significant bounce) is perceived to be relatively small.
*/

#include <array>

#include <TFOptions/Chains.h>

#include "LegDef.h"
#include "Collar.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

namespace {

  static const size_t c_nLegs( 4 );

  using LegDef = ou::tf::option::LegDef;
  using rLegDef_t = std::array<LegDef,c_nLegs>;

  //long collar: synthetic long, covered call, long put
  static const rLegDef_t c_rLegDefRise = { // rising momentum
    LegDef( 1, LegNote::Type::SynthLong,  LegNote::Side::Long,  LegNote::Option::Call ), // synthetic long
    LegDef( 1, LegNote::Type::SynthShort, LegNote::Side::Short, LegNote::Option::Put  ), // synthetic long
    LegDef( 1, LegNote::Type::Cover,      LegNote::Side::Short, LegNote::Option::Call ), // covered
    LegDef( 1, LegNote::Type::Protect,    LegNote::Side::Long,  LegNote::Option::Put  )  // protective
  };

  //short collar: synthetic short, covered put, long call
  static const rLegDef_t c_rLegDefFall = { // falling momentum
    LegDef( 1, LegNote::Type::SynthLong,  LegNote::Side::Long,  LegNote::Option::Put  ), // synthetic short
    LegDef( 1, LegNote::Type::SynthShort, LegNote::Side::Short, LegNote::Option::Call ), // synthetic short
    LegDef( 1, LegNote::Type::Cover,      LegNote::Side::Short, LegNote::Option::Put  ), // covered
    LegDef( 1, LegNote::Type::Protect,    LegNote::Side::Long,  LegNote::Option::Call )  // protective
  };

} // namespace anon

Collar::Collar()
: Combo() {}

Collar::Collar( Collar&& rhs )
: m_mapInitTrackOption( std::move( rhs.m_mapInitTrackOption ) )
{}

Collar::~Collar() {
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
      [this,date,pmapChains,days=specs.nDaysBack](){
        // TODO: check that the leg is active
        InitTrackLongOption( LegNote::Type::Protect, pmapChains, date, days );
      }
    )
  );

  // === close out at minium value, calendar roll to continue (auto or manual?)
  m_mapInitTrackOption.emplace(
    std::make_pair(
      LegNote::Type::SynthShort,
      [this,date,pmapChains,days=specs.nDaysFront](){ // make money on the sold premium
        // TODO: check that the leg is active
        InitTrackShortOption( LegNote::Type::SynthShort, pmapChains, date, days );
      }
    )
  );

  // === close out at minimum value, calendar roll to continue (auto or manual?)
  m_mapInitTrackOption.emplace(
    std::make_pair(
      LegNote::Type::Cover,
      [this,date,pmapChains,days=specs.nDaysFront](){ // make money on the sold premium
        // TODO: check that the leg is active
        InitTrackShortOption( LegNote::Type::Cover, pmapChains, date, days );
      }
    )
  );

}

void Collar::Init( LegNote::Type type ) {
  mapInitTrackOption_t::iterator iter = m_mapInitTrackOption.find( type );
  assert( m_mapInitTrackOption.end() != iter );
  iter->second();
}

size_t /* static */ Collar::LegCount() {
  return c_nLegs;
}

/* static */ void Collar::ChooseLegs( // throw Chain exceptions
  Combo::E20DayDirection direction
, const mapChains_t& chains
, boost::gregorian::date date
, const SpreadSpecs& specs
, double priceUnderlying
, const fLegSelected_t& fLegSelected
)
{

  citerChain_t citerChainBack = SelectChain( chains, date, specs.nDaysBack );
  const chain_t& chainBack( citerChainBack->second );

  citerChain_t citerChainFront = SelectChain( chains, date, specs.nDaysFront );
  const chain_t& chainFront( citerChainFront->second );

  bool bOk( true );

  switch ( direction ) {
    case E20DayDirection::Unknown:
      break;
    case E20DayDirection::Rising:
      {
        const double strikeSyntheticBack(  chainBack.Call_Itm( priceUnderlying ) ); // long call
        const double strikeSyntheticFront( chainFront.Put_Atm( strikeSyntheticBack ) ); // short put

        double strikeCovered( chainFront.Call_Otm( strikeSyntheticBack ) );
        //strikeCovered = chainFront.Call_Otm( strikeCovered ); // two strikes up

        const double strikeProtective( chainBack.Put_Atm( strikeSyntheticBack ) ); // rounding problem across chains

        fLegSelected( strikeSyntheticBack,  citerChainBack->first,  chainBack.GetIQFeedNameCall(  strikeSyntheticBack ) );
        fLegSelected( strikeSyntheticFront, citerChainFront->first, chainFront.GetIQFeedNamePut(  strikeSyntheticFront ) );
        fLegSelected( strikeCovered,        citerChainFront->first, chainFront.GetIQFeedNameCall( strikeCovered ) );
        fLegSelected( strikeProtective,     citerChainBack->first,  chainBack.GetIQFeedNamePut(   strikeProtective ) );
      }
      break;
    case E20DayDirection::Falling:
      {
        const double strikeSyntheticBack(  chainBack.Put_Itm( priceUnderlying ) ); // long put
        const double strikeSyntheticFront( chainFront.Call_Atm( strikeSyntheticBack ) ); // short call

        double strikeCovered( chainFront.Put_Otm( strikeSyntheticBack ) );
        //strikeCovered = chainFront.Put_Otm( strikeCovered ); // two strikes down

        const double strikeProtective( chainBack.Call_Atm( strikeSyntheticBack ) ); // rounding problem across chains

        fLegSelected( strikeSyntheticBack,  citerChainBack->first,  chainBack.GetIQFeedNamePut(   strikeSyntheticBack ) );
        fLegSelected( strikeSyntheticFront, citerChainFront->first, chainFront.GetIQFeedNameCall( strikeSyntheticFront ) );
        fLegSelected( strikeCovered,        citerChainFront->first, chainFront.GetIQFeedNamePut(  strikeCovered ) );
        fLegSelected( strikeProtective,     citerChainBack->first,  chainBack.GetIQFeedNameCall(  strikeProtective ) );
      }
      break;
  }

  if ( !bOk ) {
    size_t sum {};
    for ( const mapChains_t::value_type& vt: chains ) {
      sum += vt.second.Size();
      std::cout
        << "chain: "
        << vt.first << " "
        << vt.second.Size()
        << std::endl;
    }
    std::cout << "  sum: " << sum << std::endl;
  }
}

/* static */ void Collar::FillLegNote( size_t ix, Combo::E20DayDirection direction, LegNote::values_t& values ) {

  assert( ix < c_nLegs );

  values.m_algo = LegNote::Algo::Collar;
  values.m_state = LegNote::State::Open;

  switch ( direction ) {
    case E20DayDirection::Unknown:
      break;
    case E20DayDirection::Rising:
      values.m_momentum = LegNote::Momentum::Rise;
      values.m_type     = c_rLegDefRise[ix].type;
      values.m_side     = c_rLegDefRise[ix].side;
      values.m_option   = c_rLegDefRise[ix].option;
      break;
    case E20DayDirection::Falling:
      values.m_momentum = LegNote::Momentum::Fall;
      values.m_type     = c_rLegDefFall[ix].type;
      values.m_side     = c_rLegDefFall[ix].side;
      values.m_option   = c_rLegDefFall[ix].option;
      break;
  }

}

/* static */ std::string Collar::Name(
    Combo::E20DayDirection direction
  , const mapChains_t& chains
  , boost::gregorian::date date
  , const SpreadSpecs& specs
  , double price
  , const std::string& sUnderlying
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
    direction, chains, date, specs, price,
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

void Collar::PlaceOrder( ou::tf::OrderSide::EOrderSide side, uint32_t nOrderQuantity ) {

  assert( !m_pOrderCombo );
  m_pOrderCombo = ou::tf::OrderCombo::Factory();
  // legs are already in place, so create order for the position

  switch ( side ) {
    case ou::tf::OrderSide::Buy: // typical entry
      m_pOrderCombo->AddLeg( LU( LegNote::Type::SynthLong )->second.m_leg.GetPosition(), nOrderQuantity, ou::tf::OrderSide::Buy, [](){} ); // fLegDone_t
      m_pOrderCombo->AddLeg( LU( LegNote::Type::SynthShort )->second.m_leg.GetPosition(), nOrderQuantity, ou::tf::OrderSide::Sell, [](){} ); // fLegDone_t
      m_pOrderCombo->AddLeg( LU( LegNote::Type::Cover )->second.m_leg.GetPosition(), nOrderQuantity, ou::tf::OrderSide::Sell, [](){} ); // fLegDone_t
      m_pOrderCombo->AddLeg( LU( LegNote::Type::Protect )->second.m_leg.GetPosition(), nOrderQuantity, ou::tf::OrderSide::Buy, [](){} ); // fLegDone_t
      break;
    case ou::tf::OrderSide::Sell: // unusual
      m_pOrderCombo->AddLeg( LU( LegNote::Type::SynthLong )->second.m_leg.GetPosition(), nOrderQuantity, ou::tf::OrderSide::Sell, [](){} ); // fLegDone_t
      m_pOrderCombo->AddLeg( LU( LegNote::Type::SynthShort )->second.m_leg.GetPosition(), nOrderQuantity, ou::tf::OrderSide::Buy, [](){} ); // fLegDone_t
      m_pOrderCombo->AddLeg( LU( LegNote::Type::Cover )->second.m_leg.GetPosition(), nOrderQuantity, ou::tf::OrderSide::Buy, [](){} ); // fLegDone_t
      m_pOrderCombo->AddLeg( LU( LegNote::Type::Protect )->second.m_leg.GetPosition(), nOrderQuantity, ou::tf::OrderSide::Sell, [](){} ); // fLegDone_t
      break;
    default:
      assert( false );
  }

  m_pOrderCombo->Submit(
    [this](){ // fComboDone_t
      std::cout << "Collar::PlaceOrder complete" << std::endl;
      m_pOrderCombo_Kill = std::move( m_pOrderCombo );
    } );
}

} // namespace option
} // namespace tf
} // namespace ou

/*

2020/11/09

Use Stochastic for entry at the edges, and improve the mean reversion

2020/07/13

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
