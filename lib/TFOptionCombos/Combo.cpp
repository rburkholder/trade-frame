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
 * File:    Combo.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created on June 7, 2019, 5:08 PM
 */

#include <boost/log/trivial.hpp>

#include <TFTrading/PortfolioManager.h>

#include <TFOptions/Chains.h>

#include "Combo.hpp"

// this is the one:
//   ___1118980581 2015 Profiting from Weekly Options:
//     How to Earn Consistent Income Trading Weekly Option Serials (Wiley Trading)
//    page 97

// __0977869172 2006 (Options_ Perception and Deception & Coulda Woulda Shoulda revised & expanded,
//   Printed in Color) Charles M Cottle, Charles M Cottle, Sarah E Cottle -
//   Options Trading_ The Hidden Reality-RiskDoctor

// TODO:
//   need a mechansism to prevent over-write/duplication of options
//     ie, when a manual option rolls into an existing one, or an existing rolls into a manual one
//     all the legs are there, should be able to flag or gate the transition

// use the reversal/conversion in the book to conteract the problems in the Collar.
//
//  need to track the number of strikes moved from start
//  will need to track aggregate delta across multiple strategy instances

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

Combo::Combo()
: m_stateNeutral( ENeutral::no_leg )
, m_stateCandidate( LegNote::Type::Neutral )
, m_fConstructOption( nullptr )
, m_fOptionRegistryAdd( nullptr )
, m_fActivateOption( nullptr )
, m_fConstructPosition( nullptr )
, m_fDeactivateOption( nullptr )
, m_fOptionRegistryRemove( nullptr )
{}

Combo::Combo( Combo&& rhs )
: m_mapComboLeg( std::move( rhs.m_mapComboLeg ) )
, m_setpOrderCombo_Active( std::move( rhs.m_setpOrderCombo_Active ) )
, m_pPortfolio( std::move( rhs.m_pPortfolio ) )
, m_fConstructOption( std::move( rhs.m_fConstructOption ) )
, m_fOptionRegistryAdd( std::move( rhs.m_fOptionRegistryAdd ) )
, m_fActivateOption( std::move( rhs.m_fActivateOption ) )
, m_fConstructPosition( std::move( rhs.m_fConstructPosition ) )
, m_fDeactivateOption( std::move( rhs.m_fDeactivateOption ) )
, m_fOptionRegistryRemove( std::move( rhs.m_fOptionRegistryRemove ) )
, m_stateNeutral( rhs.m_stateNeutral )
, m_stateCandidate( rhs.m_stateCandidate )
{
  assert( rhs.m_mapInitTrackOption.empty() );
  assert( ENeutral::no_leg == m_stateNeutral );
}

Combo::~Combo() {
  if ( m_pChartDataView ) {
    DelChartData();
  }

  for ( mapComboLeg_t::value_type& vt: m_mapComboLeg ) {
    DeactivatePositionOption( vt.second.m_leg.GetPosition() );
  }
  m_mapComboLeg.clear();
}

// needs to happen before any Legs have been created
void Combo::Prepare(
  boost::gregorian::date date
, const mapChains_t* pmapChains
, const SpreadSpecs& specs
, fConstructOption_t&& fConstructOption
, fOptionRegistryAdd_t&& fOptionRegistryAdd
, fActivateOption_t&& fActivateOption
, fConstructPosition_t&& fConstructPosition
, fDeactivateOption_t&& fDeactivateOption
, fOptionRegistryRemove_t&& fOptionRegistryRemove
) {
  m_fConstructOption = std::move( fConstructOption );
  m_fOptionRegistryAdd = std::move( fOptionRegistryAdd );
  m_fActivateOption = std::move( fActivateOption );
  m_fConstructPosition = std::move( fConstructPosition );
  m_fDeactivateOption = std::move( fDeactivateOption );
  m_fOptionRegistryRemove = std::move( fOptionRegistryRemove );
  Init( date, pmapChains, specs );
}

void Combo::Init( boost::gregorian::date date, const mapChains_t* pmapChains, const SpreadSpecs& specs ) {

  // TODO: check if position is active prior to Initialize
  // TODO: so much happening, almost ready to start firing events on state change

  m_iterChainFront = SelectChain( *pmapChains, date, specs.nDaysFront );
  m_iterChainBack  = SelectChain( *pmapChains, date, specs.nDaysBack );

  // === vertical/diagonal roll for profitable long synthetic when trend is in wrong direction
  m_mapInitTrackOption.emplace(
    std::make_pair(
      LegNote::Type::SynthLong,
      [this]( ComboLeg& cleg ){
        InitTrackLongOption( cleg, m_iterChainBack );
      }
    )
  );

  // === vertical/diagonal roll for profitable long protective when trend is in wrong direction
  m_mapInitTrackOption.emplace(
    std::make_pair(
      LegNote::Type::Protect,
      [this]( ComboLeg& cleg ){
        InitTrackLongOption( cleg, m_iterChainBack );
      }
    )
  );

  // === close out at minium value, calendar roll to continue (auto or manual?)
  m_mapInitTrackOption.emplace(
    std::make_pair(
      LegNote::Type::SynthShort,
      [this]( ComboLeg& cleg ){
        InitTrackShortOption( cleg, m_iterChainFront );
      }
    )
  );

  // === close out at minimum value, calendar roll to continue (auto or manual?)
  m_mapInitTrackOption.emplace(
    std::make_pair(
      LegNote::Type::Cover,
      [this]( ComboLeg& cleg ){
        InitTrackShortOption( cleg, m_iterChainFront );
      }
    )
  );

  // === vertical/diagonal roll for profitable long synthetic when trend is in wrong direction
  m_mapInitTrackOption.emplace(
    std::make_pair(
      LegNote::Type::DltaPlsGmPls, // long call
      [this]( ComboLeg& cleg ){
        InitTrackLongOption( cleg, m_iterChainBack );
      }
    )
  );

  // === close out at minimum value, calendar roll to continue (auto or manual?)
  m_mapInitTrackOption.emplace(
    std::make_pair(
      LegNote::Type::DltaPlsGmMns, // short put
      [this]( ComboLeg& cleg ){
        InitTrackLongOption( cleg, m_iterChainFront );
      }
    )
  );

  // === vertical/diagonal roll for profitable long synthetic when trend is in wrong direction
  m_mapInitTrackOption.emplace(
    std::make_pair(
      LegNote::Type::DltaMnsGmPls, // long put
      [this]( ComboLeg& cleg ){
        InitTrackShortOption( cleg, m_iterChainBack );
      }
    )
  );

  // === close out at minimum value, calendar roll to continue (auto or manual?)
  m_mapInitTrackOption.emplace(
    std::make_pair(
      LegNote::Type::DltaMnsGmMns, // short call
      [this]( ComboLeg& cleg ){
        InitTrackShortOption( cleg, m_iterChainFront );
      }
    )
  );

  // === vertical/diagonal roll for profitable long synthetic when trend is in wrong direction
  m_mapInitTrackOption.emplace(
    std::make_pair(
      LegNote::Type::Long,
      [this]( ComboLeg& cleg ){
        InitTrackLongOption( cleg, m_iterChainBack );
      }
    )
  );

  // === close out at minimum value, calendar roll to continue (auto or manual?)
  m_mapInitTrackOption.emplace(
    std::make_pair(
      LegNote::Type::Short,
      [this]( ComboLeg& cleg ){
        InitTrackShortOption( cleg, m_iterChainFront );
      }
    )
  );

}

void Combo::SetPortfolio( pPortfolio_t pPortfolio ) {
  assert( m_mapComboLeg.empty() );
  m_pPortfolio = pPortfolio;
}

const LegNote::values_t& Combo::AddPosition(  pPosition_t pPositionNew ) {

  assert( pPositionNew );
  assert( m_pPortfolio->Id() == pPositionNew->GetRow().idPortfolio );

  ou::tf::Leg leg;
  const LegNote::values_t& legValues( leg.SetPosition( pPositionNew ) );

  if ( LegNote::State::Open == legValues.m_state ) {

    mapComboLeg_t::iterator iterLeg;

    // this will emplace duplicate LegNote::Type
    iterLeg = m_mapComboLeg.emplace( std::move( mapComboLeg_t::value_type( legValues.m_type, std::move( leg ) ) ) );
    ComboLeg& cleg( iterLeg->second );

    // assign Test to leg
    mapInitTrackOption_t::iterator iter = m_mapInitTrackOption.find( legValues.m_type );
    assert( m_mapInitTrackOption.end() != iter );
    iter->second( cleg );

    pWatch_t pWatch = pPositionNew->GetWatch();
    pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( pWatch );

    const std::string& sName( pWatch->GetInstrumentName() );

    vMenuActivation_t ma;
    ma.emplace_back( MenuActivation(
      "Statistics",
      [this,&cleg,&sName](){
        cleg.m_tracker.Emit();
      } ) );
    ma.emplace_back( MenuActivation(
      "Force Roll",
      [this,&cleg,&sName](){
        BOOST_LOG_TRIVIAL(info) << "Force Roll: " << sName;
        cleg.m_tracker.ForceRoll();
      } ) );
    ma.emplace_back( MenuActivation(
      "Force Close",
      [this,&cleg,&sName](){
        BOOST_LOG_TRIVIAL(info) << "Force Close: " << sName;
        cleg.m_tracker.ForceClose();
      } ) );
    //ma.emplace_back( MenuActivation(
    //  "Calendar Roll",
    //  [this,&cleg,&sName](){
    //    std::cout << "Calendar Roll: " << sName << "(todo)" << std::endl;
    //    CalendarRoll( cleg );
    //  } ) );
    //ma.emplace_back( MenuActivation(
    //  "Diagonal Roll",
    //  [this,&cleg,&sName](){
    //    std::cout << "Diagonal Roll: " << sName << "(todo)" << std::endl;
    //    DiagonalRoll( cleg );
    //  } ) );
    //ma.emplace_back( MenuActivation(
    //  "Lock Leg",
    //  [this,&cleg,&sName](){
    //    std::cout << "Lock Leg: " << sName << "(todo)" << std::endl;
    //    LegLock( cleg );
    //  } ) );
    //ma.emplace_back( MenuActivation(
    //  "Close Leg",
    //  [this,&cleg,&sName](){
    //    std::cout << "Close Leg: " << sName << "(todo)" << std::endl;
    //    LegClose( cleg );
    //  } ) );

    m_fActivateOption( pOption, ou::tf::option::LegNote::LU( legValues.m_type ), std::move( ma ) );

  }
  else {
    BOOST_LOG_TRIVIAL(info)
      << "Combo::SetPosition "
      << pPositionNew->GetInstrument()->GetInstrumentName()
      << " not open"
      ;
  }

  return legValues;
}

namespace { // where is the primary table?  is there a primary table? duplicated in Leg.cpp for now
  constexpr size_t ixPL = 2;
  constexpr size_t ixIV = 11;
  constexpr size_t ixDelta = 12;
  constexpr size_t ixGamma = 13;
  constexpr size_t ixTheta = 14;
  constexpr size_t ixVega = 15;
}

void Combo::SetChartData( pChartDataView_t pChartDataView, ou::Colour::EColour colour ) {

  assert( !m_pChartDataView );
  m_pChartDataView = pChartDataView;

  m_ceProfitLoss.SetName( "P/L Aggregate" );
  m_ceProfitLoss.SetColour( colour );
  m_pChartDataView->Add( ixPL, &m_ceProfitLoss );

  m_ceImpliedVolatility.SetName( "IV Average" );
  m_ceImpliedVolatility.SetColour( colour );
  m_pChartDataView->Add( ixIV, &m_ceImpliedVolatility );

  m_ceDelta.SetName( "Delta Aggregate" );
  m_ceDelta.SetColour( colour );
  m_pChartDataView->Add( ixDelta, &m_ceDelta );

  m_ceGamma.SetName( "Gamma Aggregate" );
  m_ceGamma.SetColour( colour );
  m_pChartDataView->Add( ixGamma, &m_ceGamma );

  m_ceTheta.SetName( "Theta Aggregate" );
  m_ceTheta.SetColour( colour );
  m_pChartDataView->Add( ixTheta, &m_ceTheta );

  m_ceVega.SetName( "Vega Aggregate" );
  m_ceVega.SetColour( colour );
  m_pChartDataView->Add( ixVega, &m_ceVega );

}

void Combo::DelChartData() {

  assert( m_pChartDataView );

  m_pChartDataView->Remove( ixPL, &m_ceProfitLoss );
  m_ceProfitLoss.Clear();

  m_pChartDataView->Remove( ixIV, &m_ceImpliedVolatility );
  m_ceImpliedVolatility.Clear();

  m_pChartDataView->Remove( ixDelta, &m_ceDelta );
  m_ceDelta.Clear();

  m_pChartDataView->Remove( ixGamma, &m_ceGamma );
  m_ceGamma.Clear();

  m_pChartDataView->Remove( ixTheta, &m_ceTheta );
  m_ceTheta.Clear();

  m_pChartDataView->Remove( ixVega, &m_ceVega );
  m_ceVega.Clear();

  m_pChartDataView.reset();

}

void Combo::InitTracker(
  ComboLeg& cleg,
  citerChain_t citerChain
) {

  pPosition_t pPosition( cleg.m_leg.GetPosition() );
  assert( pPosition );
  const chain_t& chain( citerChain->second );

  const std::string sNotes( pPosition->Notes() );
  LegNote ln( sNotes );
  LegNote::values_t notes( ln.Values() );

  cleg.m_tracker.Initialize(
    pPosition, notes.m_lock, &chain,
    [this]( const std::string& sName, fConstructedOption_t&& f ){ // m_fConstructOption
      m_fConstructOption( sName, std::move( f ) );
    },
    [this]( pPosition_t pPositionOld, pOption_t pOption )->pPosition_t { // fLegRoll_t - response to Tick
      // leg will be removed in Tick

      const std::string sNotes( pPositionOld->Notes() );

      pOrderCombo_t pOrderCombo = ou::tf::OrderCombo::Factory();
      pOrderCombo->CloseLeg(
        pPositionOld,
        [](){} );

      pPosition_t pPosition;
      pPosition = m_fConstructPosition( this, pOption, sNotes );
      pOrderCombo->AddLeg(
        pPosition,
        pPositionOld->GetActiveSize(),  pPositionOld->GetActiveSide(),
        [](){} );

      PositionNote( pPositionOld, LegNote::State::Closed );
      DeactivatePositionOption( pPositionOld );

      Submit( pOrderCombo, "Combo::InitTracker position rolled" );

      return pPosition;
    },
    [this]( pPosition_t pPositionOld ) { // fLegClose_t - response to Tick
      // leg will be removed in Tick

      PositionNote( pPositionOld, LegNote::State::Closed );
      DeactivatePositionOption( pPositionOld );

      pOrderCombo_t pOrderCombo = ou::tf::OrderCombo::Factory();
      pOrderCombo->CloseLeg(
        pPositionOld,
        [](){} );

      Submit( pOrderCombo, "Combo::InitTracker old position closed" );

    }
  );

}

void Combo::PositionNote( pPosition_t& pPosition, LegNote::State state ) {
  const std::string sNotes( pPosition->Notes() );
  LegNote ln( sNotes );
  LegNote::values_t values( ln.Values() );

  values.m_state = state;
  ln.Assign( values );
  pPosition->SetNotes( ln.Encode() );

  auto& instance( ou::tf::PortfolioManager::GlobalInstance() ); // NOTE this direct call!!
  instance.PositionUpdateNotes( pPosition );
}

void Combo::InitTrackLongOption(
  ComboLeg& cleg,
  citerChain_t citerChain
) {

  InitTracker( cleg, citerChain );

  namespace ph = std::placeholders;
  cleg.AddTest( std::bind( &ou::tf::option::Tracker::TestLong, &cleg.m_tracker, ph::_1, ph::_2, ph::_3 ) );
}

void Combo::InitTrackShortOption(
  ComboLeg& cleg,
  citerChain_t citerChain
) {

  InitTracker( cleg, citerChain );

  // a) buy out 0.10 (simply closing the position)
  // b) rotate if itm (somewhere else, affects long & short)
  // c) stop monitoring out of hours

  namespace ph = std::placeholders;
  cleg.AddTest( std::bind( &ou::tf::option::Tracker::TestShort, &cleg.m_tracker, ph::_1, ph::_2, ph::_3 ) );
}

void Combo::DeactivatePositionOption( pPosition_t pPosition ) {
  pWatch_t pWatch = pPosition->GetWatch();
  //assert( pWatch->GetInstrument()->IsOption() ); // TODO may need to change based upon other combo types
  pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( pWatch );
  m_fDeactivateOption( pOption );
}

void Combo::Submit( pOrderCombo_t pOrderCombo, const std::string& sComment ) {

  // TODO: need to confirm legs exist

  auto pair = m_setpOrderCombo_Active.insert( pOrderCombo );

  assert( pair.second );
  setpOrderCombo_t::iterator iter = pair.first;

  pOrderCombo->Submit(
    [this,iter,sComment](){ // fComboDone_t
      BOOST_LOG_TRIVIAL(info) << "Combo::Submit: " << sComment;
      m_vOrderComboIter_CleanUp.push_back( iter );
    } );
}

// TODO: make use of doubleUnderlyingSlope to trigger exit latch
void Combo::Tick( double dblUnderlyingSlope, double dblUnderlyingPrice, ptime dt ) {

  for ( setpOrderCombo_t::value_type vt: m_setpOrderCombo_Active ) {
    vt->Tick( dt );
  }

  // clear the set of completed orders
  for ( setpOrderCombo_t::iterator iter: m_vOrderComboIter_CleanUp ) { // clean up
    m_setpOrderCombo_Active.erase( iter );
  }
  m_vOrderComboIter_CleanUp.clear();

  using vRemove_t = std::vector<mapComboLeg_t::iterator>;
  vRemove_t vRemove;

  double pl {};
  double iv {};
  double delta {};
  double gamma {};
  double theta {};
  double vega {};
  double rho {};

  double nLegs {};
  for ( mapComboLeg_t::iterator iter = m_mapComboLeg.begin(); m_mapComboLeg.end() != iter; ++iter ) {

    ComboLeg& cleg( iter->second );
    Leg& leg( cleg.m_leg );

    //leg.Tick( dt, dblUnderlyingPrice );
    leg.NetGreeks( pl, iv, delta, gamma, theta, vega, rho );
    nLegs += 1.0;

    bool bRemove = cleg.Test( dt, dblUnderlyingSlope, dblUnderlyingPrice );
    if ( bRemove ) {
       //BOOST_LOG_TRIVIAL(info) << "bRemove found " << cleg.m_leg.GetPosition()->GetInstrument()->GetInstrumentName();
      vRemove.push_back( iter );
    }
  }

  m_ceProfitLoss.Append( dt, pl );
  m_ceImpliedVolatility.Append( dt, iv / nLegs ); // average
  m_ceDelta.Append( dt, delta );
  m_ceGamma.Append( dt, gamma );
  m_ceTheta.Append( dt, theta );
  m_ceVega.Append( dt, vega );

  for ( vRemove_t::value_type iter: vRemove ) { // NOTE: the lambdas above affect this
    m_mapComboLeg.erase( iter );
  };

  //NeutralCandidate( dblUnderlyingSlope, dblUnderlyingPrice, delta, gamma );
}

void Combo::NeutralCandidate( double slope, double price, double delta, double gamma ) {
  // TODO: track & add delta/gamma restoring legs
  //   lib/TFOptionCombos/LegNote.h has neutralizing entry suggestions
  //   create a tracking class operating with OptionRegistry to
  //     track in a sliding fashion a series of three options,
  //   depending upon direction, signs & strikes, use some hysterisis to shift the center strike
  //     to match current delta requirements
  //   shorts are near expiry, longs are far expiry... reuse the chains tables for selection
  //   starting at ATM, walk the threesome to the appropriate delta

  static const double c_neutral( 0.05 );
  static const double c_trigger( 0.20 );

  citerChain_t citerChain;
  enum class EType { unknown, call, put } leg_type( EType::unknown );
  enum class EDir { neutral, converge, diverge } action( EDir::neutral ); // enter position when diverging

  double sign = slope * delta;
  action = ( 0.0 <= sign ) ? EDir::converge : EDir::diverge;

  // the actively used states for quadrant management
  //auto state0 = LegNote::Type::Neutral;
  //auto state1 = LegNote::Type::DltaPlsGmPls;
  //auto state2 = LegNote::Type::DltaMnsGmPls;
  //auto state3 = LegNote::Type::DltaPlsGmMns;
  //auto state4 = LegNote::Type::DltaMnsGmMns;

  LegNote::Type quadrant( LegNote::Type::Neutral );

  if ( 0 == m_setpOrderCombo_Active.size() ) { // no evaluation while orders are outstanding
    if ( c_neutral < delta ) {
      if ( 0.0 <= gamma ) {
        // delta plus, gamma plus = long call -> short call to balance ( near date )
        quadrant = LegNote::Type::DltaPlsGmPls;
        citerChain = m_iterChainFront;
        leg_type = EType::call;
      }
      else { // 0.0 > gamma
        // delta plus, gamma minus = short put -> long put to balance ( far date )
        quadrant = LegNote::Type::DltaPlsGmMns;
        citerChain = m_iterChainBack;
        leg_type = EType::put;
      }
    }
    else {
      if ( -c_neutral > delta ) {
        if ( 0.0 <= gamma ) {
          // delta minus, gamma plus = long put -> short put to balance ( near date )
          quadrant = LegNote::Type::DltaMnsGmPls;
          citerChain = m_iterChainFront;
          leg_type = EType::put;
        }
        else { // 0.0 > gamma
          // delta minus, gamma minus = short call -> long call to balance ( far date )
          quadrant = LegNote::Type::DltaMnsGmMns;
          citerChain = m_iterChainBack;
          leg_type = EType::call;
        }
      }
    }
  }

  if ( ( LegNote::Type::Neutral != quadrant ) and ( quadrant != m_stateCandidate ) ) {
    if ( m_pCandidateHigh ) {
      m_fOptionRegistryRemove( m_pCandidateHigh->pOption );
      m_pCandidateHigh.reset();
    }
    if ( m_pCandidateLow ) {
      m_fOptionRegistryRemove( m_pCandidateLow->pOption );
      m_pCandidateLow.reset();
    }

    m_stateNeutral = ENeutral::no_leg;
    m_stateCandidate = quadrant;
  }

  // action when sign of slope is opposite of sign of delta
  //   therefore, sign of aggregate delta should change with the added position

  // TODO: need to test for crossing amongst the four quadrants,
  //   if crossed, then clear out candidates & start again

  double strike {};
  std::string name;

  switch ( m_stateNeutral ) {
    case ENeutral::no_leg:
      switch ( leg_type ) {
        case EType::call:
          m_stateNeutral = ENeutral::transition;
          strike = citerChain->second.Call_ItmAtm( price );
          name = citerChain->second.GetIQFeedNameCall( strike );
          m_fConstructOption(
            name,
            [this]( pOption_t pOption ){
              assert( !m_pCandidateLow );
              m_pCandidateLow = std::make_unique<OptionNeutralCandidate>( pOption );
              m_fOptionRegistryAdd( pOption );
              m_stateNeutral = ENeutral::leg_one_call;
            });
          break;
        case EType::put:
          m_stateNeutral = ENeutral::transition;
          strike = citerChain->second.Put_ItmAtm( price );
          name = citerChain->second.GetIQFeedNamePut( strike );
          m_fConstructOption(
            name,
            [this]( pOption_t pOption ){
              assert( !m_pCandidateLow );
              m_pCandidateLow = std::make_unique<OptionNeutralCandidate>( pOption );
              m_fOptionRegistryAdd( pOption );
              m_stateNeutral = ENeutral::leg_one_put;
            });
          break;
        case EType:: unknown:
          break;
      }
      break;
    case ENeutral::transition:
      // simply waiting
      break;
    case ENeutral::leg_one_call:
      if ( m_pCandidateLow->bGreekFound ) {

      }
      break;
    case ENeutral::leg_one_put:
      break;
    case ENeutral::find_leg_two_call:
      break;
    case ENeutral::find_leg_two_put:
      break;
    case ENeutral::search_in_call:
      break;
    case ENeutral::search_in_put:
      break;
    case ENeutral::stable:
      if ( EDir::diverge == action ) {
        if ( c_trigger < delta ) {
          // enter into position
          // and reset candidates
        }
        else {
          if ( -c_trigger > delta ) {
            // enter into position
            // and reset candidates
          }
        }
      }
      break;
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

//}

double Combo::GetNet( double price ) {

  double dblNet {};
  double dblConstructedValue {};
  double dblDelta {};
  double dblGamma {};

  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    ou::tf::Leg& leg( entry.second.m_leg );
    dblNet += leg.GetNet( price );
    leg.NetGreeks( dblDelta, dblGamma );
    double dblLegConstructedValue = leg.ConstructedValue();
    std::cout << ",constructed@" << dblLegConstructedValue;
    dblConstructedValue += dblLegConstructedValue;
    std::cout << std::endl;
  }

  std::cout
    << "  combo constructed=" << dblConstructedValue
    << ",delta=" << dblDelta
    << ",gamma=" << dblGamma
    << std::endl;
  return dblNet;
}

// TODO: need to redo this using OrderCombo
bool Combo::CloseItmLeg( double price ) {
  bool bClosed( false );
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    Leg& leg( entry.second.m_leg );
    //bClosed |= leg.CloseItm( price );
  }
  return bClosed;
}

// TODO: need to redo this using OrderCombo
void Combo::CalendarRoll( ComboLeg& cleg ) {
  //cleg.m_tracker.CalendarRoll();
}

// TODO: need to redo this using OrderCombo
void Combo::DiagonalRoll( ComboLeg& cleg ) {
  //cleg.m_tracker.DiagonalRoll();
}

// TODO: need to redo this using OrderCombo
void Combo::LegClose( ComboLeg& cleg ) {
  //cleg.m_tracker.Close();
}

// TODO: need to disable Tracker monitoring out of hours
void Combo::AtClose() {
  // maybe remove options?
}

// TODO: need to redo this using OrderCombo
bool Combo::CloseItmLegForProfit( double price ) {
  bool bClosed( false );
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    ou::tf::Leg& leg( entry.second.m_leg );
    //bClosed |= leg.CloseItmForProfit( price );
  }
  return bClosed;
}

// TODO: need to redo this using OrderCombo, and test for delta/gamma
void Combo::GoNeutral( boost::gregorian::date date, boost::posix_time::time_duration time ) {
  // relies on tracker having been quiesced
  // TODO: is the tracker/position active?
  for ( mapComboLeg_t::value_type& cleg: m_mapComboLeg ) {
    // will need improved timing, rather than just end of day
    //cleg.second.m_tracker.TestItmRoll( date, time );
  }
}

void Combo::CloseForProfits( double price ) {
}

void Combo::TakeProfits( double price ) {
}

// prevent exercise or assignment at expiry
// however, the otm leg may need an exist or roll if there is premium remaining (>$0.05)
// so ... the logic needs changing, re-arranging
// TODO: need to redo this using OrderCombo
void Combo::CloseExpiryItm( double price, const boost::gregorian::date date ) {
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    ou::tf::Leg& leg( entry.second.m_leg );
    //leg.CloseExpiryItm( date, price );
  }
}

void Combo::CloseFarItm( double price ) {
//  pPosition_t pPositionCall = m_legCall.GetPosition();
//  pPosition_t pPositionPut  = m_legPut.GetPosition();
//  if ( pPositionCall->IsActive() && pPositionPut->IsActive() ) {
//    double dblProfitCall = pPositionCall->GetUnRealizedPL();
//    double dblProfitPut  = pPositionPut->GetUnRealizedPL();
    // TOOD: finish analysis via TakeProfits - which fixes a quote issue - before continuing here
//  }
}

// TODO: need to redo this using OrderCombo
void Combo::CancelOrders() { // this might be a duplicate function from above
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    ComboLeg& cleg( entry.second );
    //cleg.m_tracker.Quiesce();
    //cleg.m_monitor.CancelOrder(); // or wait for completion?
    Leg& leg( cleg.m_leg );
    //leg.CancelOrder();
  }
}

void Combo::ClosePositions() {
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    ComboLeg& cleg( entry.second );
    //ou::tf::Leg& leg( cleg.m_leg );
    //cleg.m_tracker.Quiesce();
    cleg.m_tracker.ForceClose();
  }
}

// TODO: need to redo this using OrderCombo, and using state
bool Combo::AreOrdersActive() const { // TODO: is an external call still necessary?
  bool bOrdersActive( false );
  for ( const mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    const ou::tf::Leg& leg( entry.second.m_leg );
    //bOrdersActive |= leg.IsOrderActive();
  }
  return bOrdersActive;
}

void Combo::SaveSeries( const std::string& sPrefix ) {
  // TODO: after legs are closed, can they still be saved?
  //   should something else be used?  maybe OptionRepository instead?
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    ou::tf::Leg& leg( entry.second.m_leg );
    leg.SaveSeries( sPrefix );
  }
}

} // namespace option
} // namespace tf
} // namespace ou
