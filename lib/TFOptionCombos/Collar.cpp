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

#include <array>

#include "LegDef.h"
#include "Collar.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

namespace {

  static const size_t nLegs( 4 );

  // assuming weekly options
  static const boost::gregorian::days nDaysToExpiryFront( 1 );
  static const boost::gregorian::days nDaysToExpirySynthetic( 30 );

  using LegDef = ou::tf::option::LegDef;
  using rLegDef_t = std::array<LegDef,nLegs>;

  static const rLegDef_t m_rLegDefLong = {
    LegDef( LegDef::EOrderSide::Buy,  1, LegDef::EOptionSide::Call ), // synthetic long
    LegDef( LegDef::EOrderSide::Sell, 1, LegDef::EOptionSide::Put  ), // synthetic long
    LegDef( LegDef::EOrderSide::Sell, 1, LegDef::EOptionSide::Call ), // covered
    LegDef( LegDef::EOrderSide::Buy,  1, LegDef::EOptionSide::Put  )  // protective
  };
  static const rLegDef_t m_rLegDefShort = {
    LegDef( LegDef::EOrderSide::Buy,  1, LegDef::EOptionSide::Put  ), // synthetic short
    LegDef( LegDef::EOrderSide::Sell, 1, LegDef::EOptionSide::Call ), // synthetic short
    LegDef( LegDef::EOrderSide::Sell, 1, LegDef::EOptionSide::Put  ), // covered
    LegDef( LegDef::EOrderSide::Buy,  1, LegDef::EOptionSide::Call )  // protective
  };

} // namespace anon

Collar::Collar()
: Combo()
{
}

Collar::Collar( const Collar& rhs )
: Combo( rhs )
{}

Collar::Collar( const Collar&& rhs )
: Combo( std::move( rhs ) )
{}

Collar::~Collar() {
}

void Collar::Initialize( boost::gregorian::date date, const mapChains_t* pmapChains ) {

  citerChain_t citerChainSynthetic = Combo::SelectChain( *pmapChains, date, nDaysToExpirySynthetic );
  m_pchainSynthetic = &citerChainSynthetic->second;

  citerChain_t citerChainFront = Combo::SelectChain( *pmapChains, date, nDaysToExpiryFront );
  m_pchainFront = &citerChainFront->second;

}

void Collar::Tick( double doubleUnderlyingSlope, double dblPriceUnderlying, ptime dt ) {
  Combo::Tick( doubleUnderlyingSlope, dblPriceUnderlying, dt ); // first or last in sequence?

  // bInTrend should be slope instead, then can check sign and magnitude

  // need to manage states:  will need to obtain contract for the option, if not tracking
  // therefore, track options so ready to trade on demand? ... then watch is available as well
  //   set state so tracking with a) available option, b) waiting for option creation
  // track ITM call (for roll-up), ITM put (for roll-down) (with moving average)

  // need to keep track of options, start / stop collection
  // external construction tracks, just provide the name

  // m_eDirection has initial entry direction to use for determining leg type comparisons

  // at expiry, then exit or roll
  switch ( Combo::m_e20DayDirection ) {
    case Combo::E20DayDirection::Rising:
      {
        //   manipulate the long positions
        //   roll profitable long synthetic call up when trend changes downwards
        double strikeSyntheticItm( m_pchainSynthetic->Call_Itm( dblPriceUnderlying ) );
        //   roll profitable long front protective put down when trend changes upwards
        double strikeProtective( strikeSyntheticItm );
        //   buy back short options at 0.10? or 0.05? using GTC trade? (0.10 is probably easier)
      }
      break;
    case Combo::E20DayDirection::Falling:
      {
        //   manipulate the long positions
        //   roll profitable long synthetic put down when trend changed upwards
        double strikeSyntheticItm( m_pchainSynthetic->Put_Itm( dblPriceUnderlying ) );
        //   roll profitable long front protective call up when trend chagnes downwards
        double strikeProtective( strikeSyntheticItm );
        //   buy back short options at 0.10? or 0.05? using GTC trade? (0.10 is probably easier)
      }
      break;
    case Combo::E20DayDirection::Unknown:
      // probably still waiting for entry at this point
      break;
  }

}

size_t /* static */ Collar::LegCount() {
  return nLegs;
}

/* static */ void Collar::ChooseLegs( // throw Chain exceptions
    double slope,
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

  if ( 0.0 <= slope ) { // momentum rising

    double strikeSyntheticItm( chainSynthetic.Call_Itm( priceUnderlying ) );

    double strikeCovered( chainFront.Call_Otm( strikeSyntheticItm ) );
    strikeCovered = chainFront.Call_Otm( strikeCovered ); // two strikes up

    double strikeProtective( strikeSyntheticItm );

    fLegSelected( strikeSyntheticItm, citerChainSynthetic->first, chainSynthetic.GetIQFeedNameCall( strikeSyntheticItm ) );
    fLegSelected( strikeSyntheticItm, citerChainSynthetic->first, chainSynthetic.GetIQFeedNamePut(  strikeSyntheticItm ) );
    fLegSelected( strikeCovered,      citerChainFront->first,     chainFront.GetIQFeedNameCall(     strikeCovered ) );
    fLegSelected( strikeProtective,   citerChainFront->first,     chainFront.GetIQFeedNamePut(      strikeProtective ) );
  }
  else { // momentum falling

    double strikeSyntheticItm( chainSynthetic.Put_Itm( priceUnderlying ) );

    double strikeCovered( chainFront.Put_Otm( strikeSyntheticItm ) );
    strikeCovered = chainFront.Put_Otm( strikeCovered ); // two strikes down

    double strikeProtective( strikeSyntheticItm );

    fLegSelected( strikeSyntheticItm, citerChainSynthetic->first, chainSynthetic.GetIQFeedNamePut(  strikeSyntheticItm ) );
    fLegSelected( strikeSyntheticItm, citerChainSynthetic->first, chainSynthetic.GetIQFeedNameCall( strikeSyntheticItm ) );
    fLegSelected( strikeCovered,      citerChainFront->first,     chainFront.GetIQFeedNamePut(      strikeCovered ) );
    fLegSelected( strikeProtective,   citerChainFront->first,     chainFront.GetIQFeedNameCall(     strikeProtective ) );
  }

}

/* static */ const std::string Collar::Name( const std::string& sUnderlying, const mapChains_t& chains, boost::gregorian::date date, double price, double slope ) {

  std::string sName( "collar-" + sUnderlying );
  size_t ix {};

  if ( 0.0 <= slope ) {
    sName += "-rise";
  }
  else {
    sName += "-fall";
  }

  ChooseLegs(
    slope, chains, date, price, [&sName,&ix](double strike, boost::gregorian::date date, const std::string& sIQFeedName ){
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

void Collar::PlaceOrder( double slope20Day, ou::tf::OrderSide::enumOrderSide side ) {
  Combo::SetDirection( slope20Day );
  switch ( m_state ) {
    case State::Positions: // doesn't confirm both put/call are available
    case State::Watching:
      switch ( side ) {
        case ou::tf::OrderSide::Buy:
          m_vLeg[0].PlaceOrder( ou::tf::OrderSide::Buy,  1 );
          m_vLeg[1].PlaceOrder( ou::tf::OrderSide::Sell, 1 );
          m_vLeg[2].PlaceOrder( ou::tf::OrderSide::Sell, 1 );
          m_vLeg[3].PlaceOrder( ou::tf::OrderSide::Buy,  1 );
          break;
        case ou::tf::OrderSide::Sell:
          m_vLeg[0].PlaceOrder( ou::tf::OrderSide::Sell, 1 );
          m_vLeg[1].PlaceOrder( ou::tf::OrderSide::Buy,  1 );
          m_vLeg[2].PlaceOrder( ou::tf::OrderSide::Buy,  1 );
          m_vLeg[3].PlaceOrder( ou::tf::OrderSide::Sell, 1 );
          break;
      }
      m_state = State::Executing;
      break;
  }
}

double Collar::GetNet( double price ) {

  double dblNet {};
  double dblConstructedValue {};

  for ( Leg& leg: m_vLeg ) {
    dblNet += leg.GetNet( price ); // out: leg stats
    double dblLegConstructedValue = leg.ConstructedValue();
    std::cout << ",constructed@" << dblLegConstructedValue;
    dblConstructedValue += dblLegConstructedValue;
    std::cout << std::endl;
  }

  double profitTotal {};

  return profitTotal;

}

} // namespace option
} // namespace tf
} // namespace ou


/*

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
