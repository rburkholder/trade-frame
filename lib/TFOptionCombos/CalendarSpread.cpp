/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    CalendarSpread.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created: 2023/07/03 13:19:41
 */

#include <map>
#include <array>

#include <TFOptions/Chains.h>

#include "LegDef.h"
#include "SpreadSpecs.h"

#include "CalendarSpread.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // option
namespace spread { // spread
namespace calendar { // calendar

namespace { // anonymous

  static const size_t c_nLegs( 2 );

  using LegDef = ou::tf::option::LegDef;
  using rLegDef_t = std::array<LegDef,c_nLegs>;

  // NOTE/Caveat: AddLegOrder requires that c_rLegDefRise & c_rLegDefFall have identical LegNote::Side for each entry

  // TOOD: update leg types to reflect adjustements suggested in book Profiting from Weekly Options

  static const rLegDef_t c_rLegDefRise = { // rising momentum - call
    LegDef( 1, LegNote::Type::Long,  LegNote::Side::Long,  LegNote::Option::Call )
  , LegDef( 1, LegNote::Type::Short, LegNote::Side::Short, LegNote::Option::Call )
  };

  static const rLegDef_t c_rLegDefFall = { // falling momentum - put
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
  ComboTraits::EMarketDirection direction
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
    case ComboTraits::EMarketDirection::Select:
      assert( false );
      break;
    case ComboTraits::EMarketDirection::Rising: // call
      {
        //Basically, a trader with a bullish longer-term outlook can reduce the cost of purchasing a longer-term call option.

        const double strikeLong( chainBack.Call_Otm( priceUnderlying ) ); // long
        const double strikeShort( chainFront.Call_Otm( priceUnderlying ) ); // short

        fLegSelected( strikeLong,  citerChainBack->first, chainBack.GetIQFeedNameCall( strikeLong ) );
        fLegSelected( strikeShort, citerChainFront->first, chainFront.GetIQFeedNameCall( strikeShort ) );
      }
      break;
    case ComboTraits::EMarketDirection::Falling: // bear call
      {
        const double strikeLong(  chainBack.Put_Otm( priceUnderlying ) ); // long
        const double strikeShort( chainFront.Put_Otm( priceUnderlying ) );  // short

        fLegSelected( strikeLong,  citerChainBack->first, chainBack.GetIQFeedNameCall( strikeLong ) );
        fLegSelected( strikeShort, citerChainFront->first, chainFront.GetIQFeedNameCall( strikeShort ) );
      }
      break;
  }
}

void FillLegNote( size_t ix, ComboTraits::EMarketDirection direction, LegNote::values_t& values ) {

  assert( ix < c_nLegs );

  values.m_algo = LegNote::Algo::Unknown;
  values.m_state = LegNote::State::Open;
  values.m_lock = false;

  switch ( direction ) {
    case ComboTraits::EMarketDirection::Rising:
      values.m_algo = LegNote::Algo::CalendarCall;
      values.m_momentum = LegNote::Momentum::Rise;
      values.m_type     = c_rLegDefRise[ix].type;
      values.m_side     = c_rLegDefRise[ix].side;
      values.m_option   = c_rLegDefRise[ix].option;
      break;
    case ComboTraits::EMarketDirection::Falling:
      values.m_algo = LegNote::Algo::CalendarPut;
      values.m_momentum = LegNote::Momentum::Fall;
      values.m_type     = c_rLegDefFall[ix].type;
      values.m_side     = c_rLegDefFall[ix].side;
      values.m_option   = c_rLegDefFall[ix].option;
      break;
    case ComboTraits::EMarketDirection::Select:
      assert( false );
      break;
  }

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
  switch ( direction ) {
    case ComboTraits::EMarketDirection::Rising:
      sName = "cal-call-";
    case ComboTraits::EMarketDirection::Falling:
      sName = "cal-put-";
  }
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
            +  ou::tf::Instrument::BuildDate( date.year(), date.month(), date.day() )
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

} // namespace calendar
} // namespace spread
} // namespace option
} // namespace tf
} // namespace ou
