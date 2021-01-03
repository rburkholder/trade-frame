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
 * Created on July 19, 2020, 05:43 PM
 */

 // 2020/11/12 when rolling a call up, buy a put for the downward journey?
 //            when rolling a put down, buy a call for the upward journey?
 // 2020/11/12 upon return to original strike, buy in again?

#include <array>

#include "LegDef.h"
#include "Collar.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

namespace {

  static const size_t nLegs( 4 );

  // assuming weekly options
  static const boost::gregorian::days nDaysToExpiryFront( 5 );
  static const boost::gregorian::days nDaysToExpirySynthetic( 30 );

  using LegDef = ou::tf::option::LegDef;
  using rLegDef_t = std::array<LegDef,nLegs>;

  static const rLegDef_t m_rLegDefRise = { // rising momentum
    LegDef( 1, LegNote::Type::SynthLong,  LegNote::Side::Long,  LegNote::Option::Call, 0.20 ), // synthetic long
    LegDef( 1, LegNote::Type::SynthShort, LegNote::Side::Short, LegNote::Option::Put,  0.20 ), // synthetic long
    LegDef( 1, LegNote::Type::Cover,      LegNote::Side::Short, LegNote::Option::Call, 0.10 ), // covered
    LegDef( 1, LegNote::Type::Protect,    LegNote::Side::Long,  LegNote::Option::Put,  0.10 )  // protective
  };
  static const rLegDef_t m_rLegDefFall = { // falling momentum
    LegDef( 1, LegNote::Type::SynthLong,  LegNote::Side::Long,  LegNote::Option::Put,  0.20 ), // synthetic short
    LegDef( 1, LegNote::Type::SynthShort, LegNote::Side::Short, LegNote::Option::Call, 0.20 ), // synthetic short
    LegDef( 1, LegNote::Type::Cover,      LegNote::Side::Short, LegNote::Option::Put,  0.10 ), // covered
    LegDef( 1, LegNote::Type::Protect,    LegNote::Side::Long,  LegNote::Option::Call, 0.10 )  // protective
  };

} // namespace anon

Collar::Collar()
: Combo()
{
}

Collar::Collar( const Collar&& rhs )
: Combo( std::move( rhs ) )
{}

Collar::~Collar() {
}

// needs to happen after all Legs have been created
void Collar::Init( boost::gregorian::date date, const mapChains_t* pmapChains ) {

  assert( 4 == m_mapLeg.size() );  // need to verify this, based upon comment

  // TODO: validate that a roll generates a new position

  pPosition_t pPositionSynthetic( m_mapLeg[LegNote::Type::SynthLong].GetPosition() );
  assert( pPositionSynthetic );
  citerChain_t citerChainSynthetic = Combo::SelectChain( *pmapChains, date, nDaysToExpirySynthetic );
  m_trackerSynthetic.Initialize(
    pPositionSynthetic, &citerChainSynthetic->second,
    [this]( const std::string& sName, fConstructedOption_t&& f ){ // m_fConstructOption
      m_fConstructOption( sName, std::move( f ) );
      },
    [this,pPositionSynthetic]( pOption_t pOption ) { // m_fRoll
      m_monitor.SetPosition( pPositionSynthetic );
      m_monitor.ClosePosition();
      m_fRoll( this, pOption, pPositionSynthetic->Notes() );
    }
    );

  pPosition_t pPositionFront( m_mapLeg[LegNote::Type::Protect].GetPosition() );
  assert( pPositionFront );  // TODO: assert this is long,
  citerChain_t citerChainFront = Combo::SelectChain( *pmapChains, date, nDaysToExpiryFront );
  m_trackerFront.Initialize(
    pPositionFront, &citerChainFront->second,
    [this]( const std::string& sName, fConstructedOption_t&& f ){ // m_fConstructOption
      m_fConstructOption( sName, std::move( f ) );
      },
    [this,pPositionFront]( pOption_t pOption ) { // m_fRoll
      m_monitor.SetPosition( pPositionFront );
      m_monitor.ClosePosition();
      m_fRoll( this, pOption, pPositionFront->Notes() );
    }
    );

}

void Collar::Tick( double dblUnderlyingSlope, double dblPriceUnderlying, ptime dt ) {
  Combo::Tick( dblUnderlyingSlope, dblPriceUnderlying, dt ); // first or last in sequence?

  if ( m_monitor.IsOrderActive() ) m_monitor.Tick( dt );

  // need to manage states:  will need to obtain contract for the option, if not tracking
  // therefore, track options so ready to trade on demand? ... then watch is available as well
  //   set state so tracking with a) available option, b) waiting for option creation
  // track ITM call (for roll-up), ITM put (for roll-down) (with moving average)

  // need to keep track of options, start / stop collection
  // external construction tracks, just provide the name

  // maybe use stops for exits & rolls?

  // at expiry, then exit or roll

  //   manipulate the long positions
  //   roll profitable long synthetic call up when trend changes downwards
  //   roll profitable long front protective put down when trend changes upwards
  //   buy back short options at 0.10? or 0.05? using GTC trade? (0.10 is probably easier) -- don't bother at expiry

  m_trackerFront.TestLong( dblUnderlyingSlope, dblPriceUnderlying );
  m_trackerSynthetic.TestLong( dblUnderlyingSlope, dblPriceUnderlying );

}

size_t /* static */ Collar::LegCount() {
  return nLegs;
}

/* static */ void Collar::ChooseLegs( // throw Chain exceptions
    Combo::E20DayDirection direction,
    const mapChains_t& chains,
    boost::gregorian::date date,
    double priceUnderlying,
    fLegSelected_t&& fLegSelected
)
{

  citerChain_t citerChainSynthetic = Combo::SelectChain( chains, date, nDaysToExpirySynthetic );
  const ou::tf::option::Chain& chainSynthetic( citerChainSynthetic->second );

  citerChain_t citerChainFront = Combo::SelectChain( chains, date, nDaysToExpiryFront );
  const ou::tf::option::Chain& chainFront( citerChainFront->second );

  switch ( direction ) {
    case E20DayDirection::Unknown:
      break;
    case E20DayDirection::Rising:
      {
        double strikeSyntheticItm( chainSynthetic.Call_Itm( priceUnderlying ) );

        double strikeCovered( chainFront.Call_Otm( strikeSyntheticItm ) );
        strikeCovered = chainFront.Call_Otm( strikeCovered ); // two strikes up

        double strikeProtective( strikeSyntheticItm );

        fLegSelected( m_rLegDefRise[0].dblSpread, strikeSyntheticItm, citerChainSynthetic->first, chainSynthetic.GetIQFeedNameCall( strikeSyntheticItm ) );
        fLegSelected( m_rLegDefRise[1].dblSpread, strikeSyntheticItm, citerChainSynthetic->first, chainSynthetic.GetIQFeedNamePut(  strikeSyntheticItm ) );
        fLegSelected( m_rLegDefRise[2].dblSpread, strikeCovered,      citerChainFront->first,     chainFront.GetIQFeedNameCall(     strikeCovered ) );
        fLegSelected( m_rLegDefRise[3].dblSpread, strikeProtective,   citerChainFront->first,     chainFront.GetIQFeedNamePut(      strikeProtective ) );
      }
      break;
    case E20DayDirection::Falling:
      {
        double strikeSyntheticItm( chainSynthetic.Put_Itm( priceUnderlying ) );

        double strikeCovered( chainFront.Put_Otm( strikeSyntheticItm ) );
        strikeCovered = chainFront.Put_Otm( strikeCovered ); // two strikes down

        double strikeProtective( strikeSyntheticItm );

        fLegSelected( m_rLegDefFall[0].dblSpread, strikeSyntheticItm, citerChainSynthetic->first, chainSynthetic.GetIQFeedNamePut(  strikeSyntheticItm ) );
        fLegSelected( m_rLegDefFall[1].dblSpread, strikeSyntheticItm, citerChainSynthetic->first, chainSynthetic.GetIQFeedNameCall( strikeSyntheticItm ) );
        fLegSelected( m_rLegDefFall[2].dblSpread, strikeCovered,      citerChainFront->first,     chainFront.GetIQFeedNamePut(      strikeCovered ) );
        fLegSelected( m_rLegDefFall[3].dblSpread, strikeProtective,   citerChainFront->first,     chainFront.GetIQFeedNameCall(     strikeProtective ) );
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

/* static */ const std::string Collar::Name( const std::string& sUnderlying, const mapChains_t& chains, boost::gregorian::date date, double price, Combo::E20DayDirection direction ) {

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
    direction, chains, date, price,
    [&sName,&ix]( double spread, double strike, boost::gregorian::date date, const std::string& sIQFeedName ){
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
          m_mapLeg[LegNote::Type::SynthLong].PlaceOrder( ou::tf::OrderSide::Buy, nOrderQuantity );
          m_mapLeg[LegNote::Type::SynthShort].PlaceOrder( ou::tf::OrderSide::Sell, nOrderQuantity );
          m_mapLeg[LegNote::Type::Cover].PlaceOrder( ou::tf::OrderSide::Sell, nOrderQuantity );
          m_mapLeg[LegNote::Type::Protect].PlaceOrder( ou::tf::OrderSide::Buy, nOrderQuantity );
          break;
        case ou::tf::OrderSide::Sell:
          m_mapLeg[LegNote::Type::SynthLong].PlaceOrder( ou::tf::OrderSide::Sell, nOrderQuantity );
          m_mapLeg[LegNote::Type::SynthShort].PlaceOrder( ou::tf::OrderSide::Buy, nOrderQuantity );
          m_mapLeg[LegNote::Type::Cover].PlaceOrder( ou::tf::OrderSide::Buy, nOrderQuantity );
          m_mapLeg[LegNote::Type::Protect].PlaceOrder( ou::tf::OrderSide::Sell, nOrderQuantity );
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
