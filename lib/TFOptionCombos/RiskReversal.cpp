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
 * File:    RiskReversal.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created: 2023/06/29 10:23:00
 */

#include <map>
#include <array>

#include <TFOptions/Chains.h>

#include "LegDef.h"
#include "SpreadSpecs.h"

#include "RiskReversal.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // option
namespace reversal { // reversal

namespace { // anonymous

  static const size_t c_nLegs( 3 );

  using LegDef = ou::tf::option::LegDef;
  using rLegDef_t = std::array<LegDef,c_nLegs>;

  // NOTE/Caveat: AddLegOrder requires that c_rLegDefRise & c_rLegDefFall have identical LegNote::Side for each entry

  // TOOD: update leg types to reflect adjustements suggested in book Profiting from Weekly Options

  static const rLegDef_t c_rLegDefRise = { // rising momentum - risk reversal
    LegDef( 1, LegNote::Type::SynthLong,  LegNote::Side::Long,  LegNote::Option::Call )
  , LegDef( 1, LegNote::Type::SynthShort, LegNote::Side::Short, LegNote::Option::Put )
  , LegDef( 1, LegNote::Type::Protect,    LegNote::Side::Long,  LegNote::Option::Put )
  };

  static const rLegDef_t c_rLegDefFall = { // falling momentum - risk conversion
    LegDef( 1, LegNote::Type::SynthLong,  LegNote::Side::Long,  LegNote::Option::Put )
  , LegDef( 1, LegNote::Type::SynthShort, LegNote::Side::Short, LegNote::Option::Call )
  , LegDef( 1, LegNote::Type::Protect,    LegNote::Side::Long,  LegNote::Option::Call )
  };

  using mapLegDev_t = std::map<LegNote::Type, size_t>; // lookup into array

  static const mapLegDev_t mapLegDef = {
    { LegNote::Type::SynthLong,  0 }
  , { LegNote::Type::SynthShort, 1 }
  , { LegNote::Type::Protect,    2 }
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

  //citerChain_t citerChainBack = SelectChain( chains, date, specs.nDaysBack );
  //const chain_t& chainBack( citerChainBack->second );

  citerChain_t citerChainFront = SelectChain( chains, date, specs.nDaysFront );
  const chain_t& chainFront( citerChainFront->second );

  switch ( direction ) {
    case ComboTraits::EMarketDirection::Select:
      assert( false );
      break;
    case ComboTraits::EMarketDirection::Rising: // call
      {
        const double strikeItm1( chainFront.Call_Itm( priceUnderlying ) ); // ITM 1
        const double strikeItm2( chainFront.Call_Itm( strikeItm1 ) );      // ITM 2
        const double strikeCall1(strikeItm2 );
        const double strikePut1( chainFront.Put_Atm( strikeCall1 ) );
        const double strikePut2( chainFront.Put_Otm( strikePut1 ) ); // protection

        fLegSelected( strikeCall1, citerChainFront->first, chainFront.GetIQFeedNameCall( strikeCall1 ) );
        fLegSelected( strikePut1,  citerChainFront->first, chainFront.GetIQFeedNamePut( strikePut1 ) );
        fLegSelected( strikePut2,  citerChainFront->first, chainFront.GetIQFeedNamePut( strikePut2 ) );
      }
      break;
    case ComboTraits::EMarketDirection::Falling: // put
      {
        const double strikeItm1( chainFront.Put_Itm( priceUnderlying ) ); // ITM 1
        const double strikeItm2( chainFront.Put_Itm( strikeItm1 ) );      // ITM 2
        const double strikePut1( strikeItm2 );
        const double strikeCall1(chainFront.Call_Atm( strikePut1 ) );
        const double strikeCall2(chainFront.Call_Otm( strikeCall1 ) ); // protection

        fLegSelected( strikePut1,  citerChainFront->first, chainFront.GetIQFeedNamePut( strikePut1 ) );
        fLegSelected( strikeCall1, citerChainFront->first, chainFront.GetIQFeedNameCall( strikeCall1 ) );
        fLegSelected( strikeCall2, citerChainFront->first, chainFront.GetIQFeedNameCall( strikeCall2 ) );
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
      values.m_algo = LegNote::Algo::RiskReversal;
      values.m_momentum = LegNote::Momentum::Rise;
      values.m_type     = c_rLegDefRise[ix].type;
      values.m_side     = c_rLegDefRise[ix].side;
      values.m_option   = c_rLegDefRise[ix].option;
      break;
    case ComboTraits::EMarketDirection::Falling:
      values.m_algo = LegNote::Algo::RiskConversion;
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
      sName = "risk-rev-";
    case ComboTraits::EMarketDirection::Falling:
      sName = "risk-conv-";
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
          break;
        case 2:
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

} // namespace reversal
} // namespace option
} // namespace tf
} // namespace ou
