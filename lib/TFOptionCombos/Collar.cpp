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

/*
  Having said all that, this is not a underlying + two option strategy, this is a four option strategy, so
  different rules probably apply.  Skew is balanace, I think, and this is designed to capture mean reversion scenarios
  along with 0dte and 1dte premium capture
*/

/*
  2023/06/28 Experimentation indicates that this strategy is a mean reversion strategy
    * on longer trends, the total delta becomes inverse of direction and is counter productive.
    * needs a mechanism to re-zero the delta (for possible protection) in prep for trend continuation
      but reduces profit during mean reversion

    * via the 1118980581 book (see Combo.cpp), this could be mitigated via a reversal or conversion
      at the time of the roll of one of the longs.  this should boost the delta in the correct direction
      while, at the same time, protecting the conversion/reversal froma premature mean reversion
      - use a 7? day chain and roll forward, and risk manage the spread approriately
*/

#include <map>
#include <array>

#include <TFOptions/Chains.h>

#include "LegDef.h"
#include "SpreadSpecs.h"

#include "Collar.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options
namespace collar { // collar
namespace flex { // flex

namespace {

  static const size_t c_nLegs( 4 );

  using LegDef = ou::tf::option::LegDef;
  using rLegDef_t = std::array<LegDef,c_nLegs>;

  // NOTE/Caveat: AddLegOrder requires that c_rLegDefRise & c_rLegDefFall have identical LegNote::Side for each entry

  //long collar: synthetic long, covered call, long put
  static const rLegDef_t c_rLegDefRise = { // rising momentum
    LegDef( 1, LegNote::Type::SynthLong,  LegNote::Side::Long,  LegNote::Option::Call ) // synthetic long
  , LegDef( 1, LegNote::Type::SynthShort, LegNote::Side::Short, LegNote::Option::Put  ) // synthetic long
  , LegDef( 1, LegNote::Type::Cover,      LegNote::Side::Short, LegNote::Option::Call ) // covered
  , LegDef( 1, LegNote::Type::Protect,    LegNote::Side::Long,  LegNote::Option::Put  ) // protective
  };

  //short collar: synthetic short, covered put, long call
  static const rLegDef_t c_rLegDefFall = { // falling momentum
    LegDef( 1, LegNote::Type::SynthLong,  LegNote::Side::Long,  LegNote::Option::Put  ) // synthetic short
  , LegDef( 1, LegNote::Type::SynthShort, LegNote::Side::Short, LegNote::Option::Call ) // synthetic short
  , LegDef( 1, LegNote::Type::Cover,      LegNote::Side::Short, LegNote::Option::Put  ) // covered
  , LegDef( 1, LegNote::Type::Protect,    LegNote::Side::Long,  LegNote::Option::Call ) // protective
  };

  using mapLegDev_t = std::map<LegNote::Type, size_t>; // lookup into array

  static const mapLegDev_t mapLegDef = {
    { LegNote::Type::SynthLong,  0 }
  , { LegNote::Type::SynthShort, 1 }
  , { LegNote::Type::Cover,      2 }
  , { LegNote::Type::Protect,    3 }
  };

} // namespace anon

size_t LegCount() {
  return c_nLegs;
}

void ChooseLegs( // throw Chain exceptions
  ComboTraits::E20DayDirection direction
, const mapChains_t& chains
, boost::gregorian::date date
, const SpreadSpecs& specs
, double priceUnderlying
, const fLegSelected_t&& fLegSelected
)
{

  using citerChain_t = mapChains_t::const_iterator;

  citerChain_t citerChainBack = SelectChain( chains, date, specs.nDaysBack );
  const chain_t& chainBack( citerChainBack->second );

  citerChain_t citerChainFront = SelectChain( chains, date, specs.nDaysFront );
  const chain_t& chainFront( citerChainFront->second );

  switch ( direction ) {
    case ComboTraits::E20DayDirection::Unknown:
      assert( false );
      break;
    case ComboTraits::E20DayDirection::Rising:
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
    case ComboTraits::E20DayDirection::Falling:
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

}

void FillLegNote( size_t ix, ComboTraits::E20DayDirection direction, LegNote::values_t& values ) {

  assert( ix < c_nLegs );

  values.m_algo = LegNote::Algo::Collar;
  values.m_state = LegNote::State::Open;
  values.m_lock = false;

  switch ( direction ) {
    case ComboTraits::E20DayDirection::Rising:
      values.m_momentum = LegNote::Momentum::Rise;
      values.m_type     = c_rLegDefRise[ix].type;
      values.m_side     = c_rLegDefRise[ix].side;
      values.m_option   = c_rLegDefRise[ix].option;
      break;
    case ComboTraits::E20DayDirection::Falling:
      values.m_momentum = LegNote::Momentum::Fall;
      values.m_type     = c_rLegDefFall[ix].type;
      values.m_side     = c_rLegDefFall[ix].side;
      values.m_option   = c_rLegDefFall[ix].option;
      break;
    case ComboTraits::E20DayDirection::Unknown:
      assert( false );
      break;
  }

}

std::string Name(
  ComboTraits::E20DayDirection direction
, const mapChains_t& chains
, boost::gregorian::date date
, const SpreadSpecs& specs
, double price
, const std::string& sUnderlying
) {

  std::string sName( "clr-flx-" + sUnderlying );
  size_t ix {};

  switch ( direction ) {
    case ComboTraits::E20DayDirection::Rising:
      sName += "-rise";
      break;
    case ComboTraits::E20DayDirection::Falling:
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

// long by default for entry, short doesn't make much sense due to combo combinations
void AddLegOrder(
  const LegNote::Type type
, pOrderCombo_t pOrderCombo
, const ou::tf::OrderSide::EOrderSide side
, uint32_t nOrderQuantity
, pPosition_t pPosition
) {
  switch ( side ) {
    case ou::tf::OrderSide::Buy: // usual entry
      {
        mapLegDev_t::const_iterator iter = mapLegDef.find( type );
        assert( mapLegDef.end() != iter );
        const LegDef& leg( c_rLegDefRise[ iter->second ] ); // note the Caveat at top of file
        switch ( leg.side ) {
          case LegNote::Side::Long:
            pOrderCombo->AddLeg( pPosition, nOrderQuantity, ou::tf::OrderSide::Buy, [](){} );
            break;
          case LegNote::Side::Short:
            pOrderCombo->AddLeg( pPosition, nOrderQuantity, ou::tf::OrderSide::Sell, [](){} );
            break;
        }
      }
      break;
    case ou::tf::OrderSide::Sell: // unusual entry
      {
        mapLegDev_t::const_iterator iter = mapLegDef.find( type );
        assert( mapLegDef.end() != iter );
        const LegDef& leg( c_rLegDefFall[ iter->second ] ); // note the Caveat at top of file
        switch ( leg.side ) {
          case LegNote::Side::Long:
            pOrderCombo->AddLeg( pPosition, nOrderQuantity, ou::tf::OrderSide::Sell, [](){} );
            break;
          case LegNote::Side::Short:
            pOrderCombo->AddLeg( pPosition, nOrderQuantity, ou::tf::OrderSide::Buy, [](){} );
            break;
        }
      }
      break;
    default:
      assert( false );
  }
}

namespace ph = std::placeholders;
void Bind( ComboTraits& traits ) {
  traits.fLegCount = std::bind( &LegCount );
  traits.fChooseLegs = std::bind( &ChooseLegs, ph::_1, ph::_2, ph::_3, ph::_4, ph::_5, ph::_6 );
  traits.fFillLegNote = std::bind( &FillLegNote, ph::_1, ph::_2, ph::_3 );
  traits.fName = std::bind( &Name, ph::_1, ph::_2, ph::_3, ph::_4, ph::_5, ph::_6 );
  traits.fAddLegOrder = std::bind( &AddLegOrder, ph::_1, ph::_2, ph::_3, ph::_4, ph::_5 );
}

} // namespace flex

namespace locked { // locked

namespace {

  static const size_t c_nLegs( 4 );

  using LegDef = ou::tf::option::LegDef;
  using rLegDef_t = std::array<LegDef,c_nLegs>;

  // NOTE/Caveat: AddLegOrder requires that c_rLegDefRise & c_rLegDefFall have identical LegNote::Side for each entry

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

  using mapLegDev_t = std::map<LegNote::Type, size_t>; // lookup into array

  static const mapLegDev_t mapLegDef = {
    { LegNote::Type::SynthLong,  0 }
  , { LegNote::Type::SynthShort, 1 }
  , { LegNote::Type::Cover,      2 }
  , { LegNote::Type::Protect,    3 }
  };

} // namespace anon

size_t LegCount() {
  return c_nLegs;
}

void ChooseLegs( // throw Chain exceptions
  ComboTraits::E20DayDirection direction
, const mapChains_t& chains
, boost::gregorian::date date
, const SpreadSpecs& specs
, double priceUnderlying
, const fLegSelected_t&& fLegSelected
)
{

  using citerChain_t = mapChains_t::const_iterator;

  citerChain_t citerChainBack = SelectChain( chains, date, specs.nDaysBack );
  const chain_t& chainBack( citerChainBack->second );

  citerChain_t citerChainFront = SelectChain( chains, date, specs.nDaysFront );
  const chain_t& chainFront( citerChainFront->second );

  switch ( direction ) {
    case ComboTraits::E20DayDirection::Unknown:
      assert( false );
      break;
    case ComboTraits::E20DayDirection::Rising:
      {
        const double strikeSyntheticBack(  chainBack.Call_Atm( priceUnderlying ) ); // long call
        const double strikeSyntheticFront( chainFront.Put_Atm( strikeSyntheticBack ) ); // short put

        //double strikeCovered( chainFront.Call_Otm( strikeSyntheticBack ) );
        //strikeCovered = chainFront.Call_Otm( strikeCovered ); // two strikes up
        const double strikeCovered( chainFront.Call_Atm( strikeSyntheticBack ) );

        const double strikeProtective( chainBack.Put_Atm( strikeSyntheticBack ) );

        fLegSelected( strikeSyntheticBack,  citerChainBack->first,  chainBack.GetIQFeedNameCall(  strikeSyntheticBack ) );
        fLegSelected( strikeSyntheticFront, citerChainFront->first, chainFront.GetIQFeedNamePut(  strikeSyntheticFront ) );
        fLegSelected( strikeCovered,        citerChainFront->first, chainFront.GetIQFeedNameCall( strikeCovered ) );
        fLegSelected( strikeProtective,     citerChainBack->first,  chainBack.GetIQFeedNamePut(   strikeProtective ) );
      }
      break;
    case ComboTraits::E20DayDirection::Falling:
      {
        const double strikeSyntheticBack(  chainBack.Put_Itm( priceUnderlying ) ); // long put
        const double strikeSyntheticFront( chainFront.Call_Atm( strikeSyntheticBack ) ); // short call

        //double strikeCovered( chainFront.Put_Otm( strikeSyntheticBack ) );
        //strikeCovered = chainFront.Put_Otm( strikeCovered ); // two strikes down
        const double strikeCovered( chainFront.Put_Atm( strikeSyntheticBack ) );

        const double strikeProtective( chainBack.Call_Atm( strikeSyntheticBack ) );

        fLegSelected( strikeSyntheticBack,  citerChainBack->first,  chainBack.GetIQFeedNamePut(   strikeSyntheticBack ) );
        fLegSelected( strikeSyntheticFront, citerChainFront->first, chainFront.GetIQFeedNameCall( strikeSyntheticFront ) );
        fLegSelected( strikeCovered,        citerChainFront->first, chainFront.GetIQFeedNamePut(  strikeCovered ) );
        fLegSelected( strikeProtective,     citerChainBack->first,  chainBack.GetIQFeedNameCall(  strikeProtective ) );
      }
      break;
  }

}

void FillLegNote( size_t ix, ComboTraits::E20DayDirection direction, LegNote::values_t& values ) {

  assert( ix < c_nLegs );

  values.m_algo = LegNote::Algo::Collar;
  values.m_state = LegNote::State::Open;
  values.m_lock = true;

  switch ( direction ) {
    case ComboTraits::E20DayDirection::Rising:
      values.m_momentum = LegNote::Momentum::Rise;
      values.m_type     = c_rLegDefRise[ix].type;
      values.m_side     = c_rLegDefRise[ix].side;
      values.m_option   = c_rLegDefRise[ix].option;
      break;
    case ComboTraits::E20DayDirection::Falling:
      values.m_momentum = LegNote::Momentum::Fall;
      values.m_type     = c_rLegDefFall[ix].type;
      values.m_side     = c_rLegDefFall[ix].side;
      values.m_option   = c_rLegDefFall[ix].option;
      break;
    case ComboTraits::E20DayDirection::Unknown:
      assert( false );
      break;
  }
}

std::string Name(
  ComboTraits::E20DayDirection direction
, const mapChains_t& chains
, boost::gregorian::date date
, const SpreadSpecs& specs
, double price
, const std::string& sUnderlying
) {

  std::string sName( "clr-lck-" + sUnderlying );
  size_t ix {};

  switch ( direction ) {
    case ComboTraits::E20DayDirection::Rising:
      sName += "-rise";
      break;
    case ComboTraits::E20DayDirection::Falling:
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

// long by default for entry, short doesn't make much sense due to combo combinations
void AddLegOrder(
  const LegNote::Type type
, pOrderCombo_t pOrderCombo
, const ou::tf::OrderSide::EOrderSide side
, uint32_t nOrderQuantity
, pPosition_t pPosition
) {
  switch ( side ) {
    case ou::tf::OrderSide::Buy: // usual entry
      {
        mapLegDev_t::const_iterator iter = mapLegDef.find( type );
        assert( mapLegDef.end() != iter );
        const LegDef& leg( c_rLegDefRise[ iter->second ] ); // note the Caveat at top of file
        switch ( leg.side ) {
          case LegNote::Side::Long:
            pOrderCombo->AddLeg( pPosition, nOrderQuantity, ou::tf::OrderSide::Buy, [](){} );
            break;
          case LegNote::Side::Short:
            pOrderCombo->AddLeg( pPosition, nOrderQuantity, ou::tf::OrderSide::Sell, [](){} );
            break;
        }
      }
      break;
    case ou::tf::OrderSide::Sell: // unusual entry
      {
        mapLegDev_t::const_iterator iter = mapLegDef.find( type );
        assert( mapLegDef.end() != iter );
        const LegDef& leg( c_rLegDefFall[ iter->second ] ); // note the Caveat at top of file
        switch ( leg.side ) {
          case LegNote::Side::Long:
            pOrderCombo->AddLeg( pPosition, nOrderQuantity, ou::tf::OrderSide::Sell, [](){} );
            break;
          case LegNote::Side::Short:
            pOrderCombo->AddLeg( pPosition, nOrderQuantity, ou::tf::OrderSide::Buy, [](){} );
            break;
        }
      }
      break;
    default:
      assert( false );
  }
}

namespace ph = std::placeholders;
void Bind( ComboTraits& traits ) {
  traits.fLegCount = std::bind( &LegCount );
  traits.fChooseLegs = std::bind( &ChooseLegs, ph::_1, ph::_2, ph::_3, ph::_4, ph::_5, ph::_6 );
  traits.fFillLegNote = std::bind( &FillLegNote, ph::_1, ph::_2, ph::_3 );
  traits.fName = std::bind( &Name, ph::_1, ph::_2, ph::_3, ph::_4, ph::_5, ph::_6 );
  traits.fAddLegOrder = std::bind( &AddLegOrder, ph::_1, ph::_2, ph::_3, ph::_4, ph::_5 );
}

} // namespace locked

} // namespace collar
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
