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

  enum class ELeg { SynthLong = 0, SynthShort, FrontShort, FrontLong };

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

void Collar::Init( boost::gregorian::date date, const mapChains_t* pmapChains ) {

  citerChain_t citerChainSynthetic = Combo::SelectChain( *pmapChains, date, nDaysToExpirySynthetic );
  m_trackerSynthetic.SetChain( &citerChainSynthetic->second );

  citerChain_t citerChainFront = Combo::SelectChain( *pmapChains, date, nDaysToExpiryFront );
  m_trackerFront.SetChain( &citerChainFront->second );

}

namespace {
  using comp_t = std::function<bool(double,double)>;
  bool lt( double a, double b ) { return a < b; }
  bool gt( double a, double b ) { return a > b; }
  bool eq( double a, double b ) { return a == b; }
}

void Collar::Tick( double doubleUnderlyingSlope, double dblPriceUnderlying, ptime dt ) {
  Combo::Tick( doubleUnderlyingSlope, dblPriceUnderlying, dt ); // first or last in sequence?

  // need to manage states:  will need to obtain contract for the option, if not tracking
  // therefore, track options so ready to trade on demand? ... then watch is available as well
  //   set state so tracking with a) available option, b) waiting for option creation
  // track ITM call (for roll-up), ITM put (for roll-down) (with moving average)

  // need to keep track of options, start / stop collection
  // external construction tracks, just provide the name

  // maybe use stops for exits & rolls?

  // at expiry, then exit or roll

  static const vLeg_t::size_type ixSynthLong( (size_t)ELeg::SynthLong );
  static const vLeg_t::size_type ixFrontLong( (size_t)ELeg::FrontLong );

  //   manipulate the long positions
  //   roll profitable long synthetic call up when trend changes downwards
  //   roll profitable long front protective put down when trend changes upwards
  //   buy back short options at 0.10? or 0.05? using GTC trade? (0.10 is probably easier) -- don't bother at expiry

  TestLong( ixFrontLong, dblPriceUnderlying, m_trackerFront );
  TestLong( ixSynthLong, dblPriceUnderlying, m_trackerSynthetic );

}

void Collar::TestLong( vLeg_t::size_type ixLong, double dblUnderlying, Tracker& tracker ) {

  pPosition_t pPosition( m_vLeg[ixLong].GetPosition() ); // TODO: assert this is long, turn into one time operation
  if ( pPosition ) {
    pWatch_t pWatch = pPosition->GetWatch();
    pInstrument_t pInstrument = pWatch->GetInstrument();
    if ( pInstrument->IsOption() ) {

      double strikeItm;

      comp_t comp( nullptr );  // TODO: a one time operation upon assignment of position
      switch ( pInstrument->GetOptionSide() ) {
        case ou::tf::OptionSide::Call:
          comp = &gt;
          strikeItm = tracker.m_pChain->Call_Itm( dblUnderlying );
          break;
        case ou::tf::OptionSide::Put:
          comp = &eq;
          strikeItm = tracker.m_pChain->Put_Itm( dblUnderlying );
          break;
      }
      assert( comp );

      if ( comp( strikeItm, pInstrument->GetStrike() ) ) { // is new strike further itm?
        if ( tracker.m_pOption ) { // if already tracking the option
          if ( comp( strikeItm, tracker.m_pOption->GetStrike() ) ) { // move further itm?
            tracker.m_transition = ETransition::Vacant;
            tracker.m_pOption.reset();
            Construct( pInstrument->GetOptionSide(), strikeItm, tracker );
          }
          else {
            // TODO: if retreating, stay pat, retreat, or try the roll?
            // nothing to do, track in existing option as quotes are updated
          }
        }
        else {
          // need to obtain option, but track via state machine to request only once
          tracker.m_transition = ETransition::Vacant;
          Construct( pInstrument->GetOptionSide(), strikeItm, tracker );
        }
      }
      else {
        // nothing to do, hasn't moved enough itm
      }
    }
    else {
      // notification, requires option
    }
  }
}

void Collar::Construct( ou::tf::OptionSide::enumOptionSide side, double strikeItm, Tracker& tracker ) {
  std::string sName;
  switch ( side ) {
    case ou::tf::OptionSide::Call:
      sName = tracker.m_pChain->Call_Itm( strikeItm );
      break;
    case ou::tf::OptionSide::Put:
      sName = tracker.m_pChain->Put_Itm( strikeItm );
      break;
  }
  tracker.m_transition = ETransition::Acquire;
  Combo::m_fConstructOption(
    sName,
    [this, &tracker]( pOption_t pOption ){
      tracker.m_pOption = pOption;
      tracker.m_transition = ETransition::Track;
    } );
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

        fLegSelected( strikeSyntheticItm, citerChainSynthetic->first, chainSynthetic.GetIQFeedNameCall( strikeSyntheticItm ) );
        fLegSelected( strikeSyntheticItm, citerChainSynthetic->first, chainSynthetic.GetIQFeedNamePut(  strikeSyntheticItm ) );
        fLegSelected( strikeCovered,      citerChainFront->first,     chainFront.GetIQFeedNameCall(     strikeCovered ) );
        fLegSelected( strikeProtective,   citerChainFront->first,     chainFront.GetIQFeedNamePut(      strikeProtective ) );
      }
      break;
    case E20DayDirection::Falling:
      {
        double strikeSyntheticItm( chainSynthetic.Put_Itm( priceUnderlying ) );

        double strikeCovered( chainFront.Put_Otm( strikeSyntheticItm ) );
        strikeCovered = chainFront.Put_Otm( strikeCovered ); // two strikes down

        double strikeProtective( strikeSyntheticItm );

        fLegSelected( strikeSyntheticItm, citerChainSynthetic->first, chainSynthetic.GetIQFeedNamePut(  strikeSyntheticItm ) );
        fLegSelected( strikeSyntheticItm, citerChainSynthetic->first, chainSynthetic.GetIQFeedNameCall( strikeSyntheticItm ) );
        fLegSelected( strikeCovered,      citerChainFront->first,     chainFront.GetIQFeedNamePut(      strikeCovered ) );
        fLegSelected( strikeProtective,   citerChainFront->first,     chainFront.GetIQFeedNameCall(     strikeProtective ) );
      }
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
    direction, chains, date, price, [&sName,&ix](double strike, boost::gregorian::date date, const std::string& sIQFeedName ){
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

void Collar::PlaceOrder( ou::tf::OrderSide::enumOrderSide side ) {
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
