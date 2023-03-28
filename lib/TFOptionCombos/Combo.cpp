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

#include <TFTrading/PortfolioManager.h>

#include <TFOptions/Chains.h>

#include "Combo.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

//Combo::Combo(){}

Combo::Combo( Combo&& rhs )
: m_mapComboLeg( std::move( rhs.m_mapComboLeg ) ),
  m_pPortfolio( std::move( rhs.m_pPortfolio ) ),
  m_fConstructOption( std::move( rhs.m_fConstructOption ) ),
  m_fActivateOption( std::move( rhs.m_fActivateOption ) ),
  m_fConstructPosition( std::move( rhs.m_fConstructPosition ) ),
  m_fDeactivateOption( std::move( rhs.m_fDeactivateOption ) )
{
  assert( rhs.m_mapInitTrackOption.empty() );
}

Combo::~Combo() {
  for ( mapComboLeg_t::value_type& vt: m_mapComboLeg ) {
    DeactivatePositionOption( vt.second.m_leg.GetPosition() );
  }
  m_mapComboLeg.clear();
}

void Combo::Prepare(
  boost::gregorian::date date,
  const mapChains_t* pmapChains,
  const SpreadSpecs& specs,
  fConstructOption_t&& fConstructOption,
  fActivateOption_t&& fActivateOption,
  fConstructPosition_t&& fConstructPosition,
  fDeactivateOption_t&& fDeactivateOption
) {
  m_fConstructOption = std::move( fConstructOption );
  m_fActivateOption = std::move( fActivateOption );
  m_fConstructPosition = std::move( fConstructPosition );
  m_fDeactivateOption = std::move( fDeactivateOption );
  Init( date, pmapChains, specs );
}

void Combo::SetPortfolio( pPortfolio_t pPortfolio ) {
  assert( m_mapComboLeg.empty() );
  m_pPortfolio = pPortfolio;
}

Combo::mapComboLeg_t::iterator Combo::LU( LegNote::Type type ) {
  //auto pair = m_mapComboLeg.equal_range( type );
  //assert( m_mapComboLeg.end() != pair.first );
  //assert( pair.first != pair.second ); // at least one, check for only one?
  mapComboLeg_t::iterator iterLeg = m_mapComboLeg.lower_bound( type ); // look for first
  assert( m_mapComboLeg.end() != iterLeg );
  //return iterLeg->second;
  return iterLeg;
}

const LegNote::values_t& Combo::SetPosition(  pPosition_t pPositionNew, pChartDataView_t pChartData, ou::Colour::EColour colour ) {

  assert( pPositionNew );
  assert( pChartData );
  assert( m_pPortfolio->Id() == pPositionNew->GetRow().idPortfolio );

  ou::tf::Leg leg;
  const LegNote::values_t& legValues( leg.SetPosition( pPositionNew ) );

  if ( LegNote::State::Open == legValues.m_state ) {

    mapComboLeg_t::iterator iterLeg;

    // this will emplace duplicate LegNote::Type
    iterLeg = m_mapComboLeg.emplace( std::move( mapComboLeg_t::value_type( legValues.m_type, std::move( leg ) ) ) );
    ComboLeg& cleg( iterLeg->second );

    cleg.m_leg.SetChartData( pChartData, colour ); // comes after as there is no move on indicators

    // assign Test to leg
    mapInitTrackOption_t::iterator iter = m_mapInitTrackOption.find( legValues.m_type );
    assert( m_mapInitTrackOption.end() != iter );
    iter->second( cleg );

    pWatch_t pWatch = pPositionNew->GetWatch();
    pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( pWatch );

    const std::string& sName( pWatch->GetInstrumentName() );

    vMenuActivation_t ma;
    ma.emplace_back( MenuActivation(
      "Calendar Roll",
      [this,&cleg,&sName](){
        std::cout << "Calendar Roll: " << sName << "(todo)" << std::endl;
        CalendarRoll( cleg );
      } ) );
    ma.emplace_back( MenuActivation(
      "Diagonal Roll",
      [this,&cleg,&sName](){
        std::cout << "Diagonal Roll: " << sName << "(todo)" << std::endl;
        DiagonalRoll( cleg );
      } ) );
    ma.emplace_back( MenuActivation(
      "Lock Leg",
      [this,&cleg,&sName](){
        std::cout << "Lock Leg: " << sName << "(todo)" << std::endl;
        LegLock( cleg );
      } ) );
    ma.emplace_back( MenuActivation(
      "Close Leg",
      [this,&cleg,&sName](){
        std::cout << "Close Leg: " << sName << "(todo)" << std::endl;
        LegClose( cleg );
      } ) );

    m_fActivateOption( pOption, pPositionNew, ou::tf::option::LegNote::LU( legValues.m_type ), std::move( ma ) );

  }
  else {
    std::cout
      << "Combo::SetPosition "
      << pPositionNew->GetInstrument()->GetInstrumentName()
      << " not open"
      << std::endl;
  }

  return legValues;
}

void Combo::InitTracker(
  ComboLeg& cleg,
  const mapChains_t* pmapChains,
  boost::gregorian::date date,
  boost::gregorian::days days_to_expiry
) {

  pPosition_t pPosition( cleg.m_leg.GetPosition() );
  assert( pPosition );
  citerChain_t citerChain = SelectChain( *pmapChains, date, days_to_expiry );
  const chain_t& chain( citerChain->second );

  cleg.m_tracker.Initialize(
    pPosition, &chain,
    [this]( const std::string& sName, fConstructedOption_t&& f ){ // m_fConstructOption
      m_fConstructOption( sName, std::move( f ) );
    },
    [this]( pPosition_t pPositionOld, pOption_t pOption )->pPosition_t { // fRollLeg_t - response to Tick
      // leg will be removed in Tick

      const std::string sNotes( pPositionOld->Notes() );

      assert( !m_pOrderCombo );

      m_pOrderCombo = ou::tf::OrderCombo::Factory();
      m_pOrderCombo->CloseLeg(
        pPositionOld,
        [this](){} );

      pPosition_t pPosition;
      pPosition = m_fConstructPosition( this, pOption, sNotes );
      m_pOrderCombo->AddLeg(
        pPosition,
        pPositionOld->GetActiveSize(),  pPositionOld->GetActiveSide(),
        [this](){});

      PositionNote( pPositionOld, LegNote::State::Closed );
      DeactivatePositionOption( pPositionOld );

      m_pOrderCombo->Submit(
        [this](){
          m_pOrderCombo_Kill = std::move( m_pOrderCombo );
        } );

      return pPosition;
    },
    [this]( pPosition_t pPositionOld ) { // m_fCloseLeg - response to Tick
      // leg will be removed in Tick

      PositionNote( pPositionOld, LegNote::State::Closed );
      DeactivatePositionOption( pPositionOld );

      assert( !m_pOrderCombo );

      m_pOrderCombo = ou::tf::OrderCombo::Factory();
      m_pOrderCombo->CloseLeg(
        pPositionOld,
        [this](){} );
      m_pOrderCombo->Submit(
        [this](){
          m_pOrderCombo_Kill = std::move( m_pOrderCombo );
        } );
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

// NOTE: may require delayed reaction on this, as a roll will call back into this with new position
void Combo::InitTrackLongOption(
    ComboLeg& cleg,
    const mapChains_t* pmapChains,
    boost::gregorian::date date,
    boost::gregorian::days days_to_expiry
    ) {

  InitTracker( cleg, pmapChains, date, days_to_expiry );

  namespace ph = std::placeholders;
  cleg.AddTest( std::bind( &ou::tf::option::Tracker::TestLong, &cleg.m_tracker, ph::_1, ph::_2, ph::_3 ) );
}

void Combo::InitTrackShortOption(
    ComboLeg& cleg,
    const mapChains_t* pmapChains,
    boost::gregorian::date date,
    boost::gregorian::days days_to_expiry
) {

  InitTracker( cleg, pmapChains, date, days_to_expiry );

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

// TODO: make use of doubleUnderlyingSlope to trigger exit latch
void Combo::Tick( double dblUnderlyingSlope, double dblUnderlyingPrice, ptime dt ) {

  // may need vector?
  if ( m_pOrderCombo ) m_pOrderCombo->Tick( dt );

  using vRemove_t = std::vector<mapComboLeg_t::iterator>;
  vRemove_t vRemove;

  for ( mapComboLeg_t::iterator iter = m_mapComboLeg.begin(); m_mapComboLeg.end() != iter; ++iter ) {

    ComboLeg& cleg( iter->second );
    Leg& leg( cleg.m_leg );

    leg.Tick( dt, dblUnderlyingPrice );

    bool bRemove = cleg.Test( dt, dblUnderlyingSlope, dblUnderlyingPrice );
    if ( bRemove ) {
      vRemove.push_back( iter );
    }
  }

  for ( vRemove_t::value_type iter: vRemove ) { // NOTE: the lambdas above affect this
    m_mapComboLeg.erase( iter );
  };
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

void Combo::LegLock( ComboLeg& cleg ) {
  cleg.m_tracker.Lock( false ); // TODO: need to upate LegNote
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
    cleg.m_tracker.Quiesce();
    //cleg.m_monitor.CancelOrder(); // or wait for completion?

    Leg& leg( cleg.m_leg );
    //leg.CancelOrder();
  }
}

// TODO: need to redo this using OrderCombo
void Combo::ClosePositions() {
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    ou::tf::Leg& leg( entry.second.m_leg );
    if ( leg.IsActive() ) {
      //pPosition_t pPosition = leg.ClosePosition();
      auto& instance( ou::tf::PortfolioManager::GlobalInstance() ); // NOTE this direct call!!
      //instance.PositionUpdateNotes( pPosition );
    }
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
