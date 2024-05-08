/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
 * File:    VerticalSpread.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created on June 11, 2019, 8:03 PM
 */

#include <map>
#include <array>

#include <TFOptions/Chains.h>

#include "LegDef.h"
#include "SpreadSpecs.h"

#include "VerticalSpread.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // option
namespace spread { // spread

namespace bear_call { // vertical

namespace { // anonymous

  static const size_t c_nLegs( 2 );

  using LegDef = ou::tf::option::LegDef;
  using rLegDef_t = std::array<LegDef,c_nLegs>;

  // TOOD: update leg types to reflect adjustements suggested in book Profiting from Weekly Options

  static const rLegDef_t c_rLegDef = { // rising momentum - bull put - buy side
    LegDef( 1, LegNote::Type::Long,  LegNote::Side::Long,  LegNote::Option::Call )
  , LegDef( 1, LegNote::Type::Short, LegNote::Side::Short, LegNote::Option::Call )
  };

  using mapLegDev_t = std::map<LegNote::Type, size_t>; // lookup into array

  static const mapLegDev_t mapLegDef = {
    { LegNote::Type::Long,  0 }
  , { LegNote::Type::Short, 1 }
  };

} // namespace anon

size_t LegCount() {
  return c_nLegs;
}

void ChooseLegs( // throw Chain exceptions
  ComboTraits::EMarketDirection direction // direction = expected => buy, else sell
, const mapChains_t& chains
, boost::gregorian::date date
, const SpreadSpecs& specs
, double priceUnderlying
, const fLegSelected_t&& fLegSelected
)
{
  using citerChain_t = mapChains_t::const_iterator;

  citerChain_t citerChainVertical = SelectChain( chains, date, specs.nDaysFront );
  const chain_t& chainVertical( citerChainVertical->second );

  const double strikeShort( chainVertical.Call_Atm( priceUnderlying ) ); // ATM
  const double strikeLong(  chainVertical.Call_Otm( strikeShort ) );     // ATM + 1

  fLegSelected( strikeShort, citerChainVertical->first, chainVertical.GetIQFeedNameCall( strikeShort ) );
  fLegSelected( strikeLong,  citerChainVertical->first, chainVertical.GetIQFeedNameCall( strikeLong ) );
}

void FillLegNote( size_t ix, ComboTraits::EMarketDirection direction, LegNote::values_t& values ) {

  assert( ix < c_nLegs );

  values.m_algo = LegNote::Algo::Unknown;
  values.m_state = LegNote::State::Open;
  values.m_lock = false;

  values.m_algo = LegNote::Algo::BearCall;
  values.m_momentum = LegNote::Momentum::Fall;
  values.m_type     = c_rLegDef[ix].type;
  values.m_side     = c_rLegDef[ix].side;
  values.m_option   = c_rLegDef[ix].option;

}

std::string Name(
  ComboTraits::EMarketDirection direction
, const mapChains_t& chains
, boost::gregorian::date date
, const SpreadSpecs& specs
, double price
, const std::string& sUnderlying
) {

  std::string sName;
  sName = "bear-call-";
  sName += sUnderlying;

  size_t ix {};

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

void AddLegOrder(
  const LegNote::Type type
, pOrderCombo_t pOrderCombo
, const ou::tf::OrderSide::EOrderSide side
, uint32_t nOrderQuantity // TODO: need to multiply by what is in static table
, pPosition_t pPosition
) {
  switch ( side ) {
    case ou::tf::OrderSide::Buy:
      {
        mapLegDev_t::const_iterator iter = mapLegDef.find( type );
        assert( mapLegDef.end() != iter );
        const LegDef& leg( c_rLegDef[ iter->second ] ); // note the Caveat at top of file
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
    case ou::tf::OrderSide::Sell:
      {
        mapLegDev_t::const_iterator iter = mapLegDef.find( type );
        assert( mapLegDef.end() != iter );
        const LegDef& leg( c_rLegDef[ iter->second ] ); // note the Caveat at top of file
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

} // namespace bear_call

namespace bull_put { // vertical

namespace { // anonymous

  static const size_t c_nLegs( 2 );

  using LegDef = ou::tf::option::LegDef;
  using rLegDef_t = std::array<LegDef,c_nLegs>;

  // TOOD: update leg types to reflect adjustements suggested in book Profiting from Weekly Options

  static const rLegDef_t c_rLegDef = { // rising momentum - bull put - buy side
    LegDef( 1, LegNote::Type::Long,  LegNote::Side::Long,  LegNote::Option::Put )
  , LegDef( 1, LegNote::Type::Short, LegNote::Side::Short, LegNote::Option::Put )
  };

  using mapLegDev_t = std::map<LegNote::Type, size_t>; // lookup into array

  static const mapLegDev_t mapLegDef = {
    { LegNote::Type::Long,  0 }
  , { LegNote::Type::Short, 1 }
  };

} // namespace anon

size_t LegCount() {
  return c_nLegs;
}

void ChooseLegs( // throw Chain exceptions
  ComboTraits::EMarketDirection direction // direction = expected => buy, else sell
, const mapChains_t& chains
, boost::gregorian::date date
, const SpreadSpecs& specs
, double priceUnderlying
, const fLegSelected_t&& fLegSelected
)
{
  using citerChain_t = mapChains_t::const_iterator;

  citerChain_t citerChainVertical = SelectChain( chains, date, specs.nDaysFront );
  const chain_t& chainVertical( citerChainVertical->second );

  const double strikeShort( chainVertical.Put_Atm( priceUnderlying ) ); // ATM
  const double strikeLong(  chainVertical.Put_Otm( strikeShort ) );     // ATM - 1

  fLegSelected( strikeShort, citerChainVertical->first, chainVertical.GetIQFeedNamePut( strikeShort ) );
  fLegSelected( strikeLong,  citerChainVertical->first, chainVertical.GetIQFeedNamePut( strikeLong ) );
}

void FillLegNote( size_t ix, ComboTraits::EMarketDirection direction, LegNote::values_t& values ) {

  assert( ix < c_nLegs );

  values.m_state = LegNote::State::Open;
  values.m_lock = false;

  values.m_algo = LegNote::Algo::BullPut;
  values.m_momentum = LegNote::Momentum::Rise;
  values.m_type     = c_rLegDef[ix].type;
  values.m_side     = c_rLegDef[ix].side;
  values.m_option   = c_rLegDef[ix].option;

}

std::string Name(
  ComboTraits::EMarketDirection direction
, const mapChains_t& chains
, boost::gregorian::date date
, const SpreadSpecs& specs
, double price
, const std::string& sUnderlying
) {

  std::string sName;
  sName = "bull-put-";
  sName += sUnderlying;

  size_t ix {};

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
// TODO: need to select the bear-call and bull-put properly
void AddLegOrder(
  const LegNote::Type type
, pOrderCombo_t pOrderCombo
, const ou::tf::OrderSide::EOrderSide side
, uint32_t nOrderQuantity // TODO: need to multiply by what is in static table
, pPosition_t pPosition
) {
  switch ( side ) {
    case ou::tf::OrderSide::Buy:  // need a selector for bull put or bear call
      {
        mapLegDev_t::const_iterator iter = mapLegDef.find( type );
        assert( mapLegDef.end() != iter );
        const LegDef& leg( c_rLegDef[ iter->second ] ); // note the Caveat at top of file
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
    case ou::tf::OrderSide::Sell:
      {
        mapLegDev_t::const_iterator iter = mapLegDef.find( type );
        assert( mapLegDef.end() != iter );
        const LegDef& leg( c_rLegDef[ iter->second ] ); // note the Caveat at top of file
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

} // namespace bull_put

} // namespace spread
} // namespace option
} // namespace tf
} // namespace ou
