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

Combo::Combo()
: m_state( State::Initializing )
{
}

Combo::Combo( Combo&& rhs )
: m_state( rhs.m_state ),
  m_mapComboLeg( std::move( rhs.m_mapComboLeg ) ),
  m_pPortfolio( std::move( rhs.m_pPortfolio ) ),
  m_fConstructOption( std::move( rhs.m_fConstructOption ) ),
  m_fActivateOption( std::move( rhs.m_fActivateOption ) ),
  m_fOpenPosition( std::move( rhs.m_fOpenPosition ) ),
  m_fDeactivateOption( std::move( rhs.m_fDeactivateOption ) )
{
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
  fOpenPosition_t&& fOpenPosition,
  fDeactivateOption_t&& fDeactivateOption
) {
  m_fConstructOption = std::move( fConstructOption );
  m_fActivateOption = std::move( fActivateOption );
  m_fOpenPosition = std::move( fOpenPosition );
  m_fDeactivateOption = std::move( fDeactivateOption );
  Init( date, pmapChains, specs );
}

void Combo::SetPortfolio( pPortfolio_t pPortfolio ) {
  assert( m_mapComboLeg.empty() );
  m_pPortfolio = pPortfolio;
}

Combo::ComboLeg& Combo::LU( LegNote::Type type ) {
  //auto pair = m_mapComboLeg.equal_range( type );
  //assert( m_mapComboLeg.end() != pair.first );
  //assert( pair.first != pair.second ); // at least one, check for only one?
  mapComboLeg_t::iterator iterLeg = m_mapComboLeg.lower_bound( type ); // look for first
  assert( m_mapComboLeg.end() != iterLeg );
  return iterLeg->second;
}

const LegNote::values_t& Combo::SetPosition(  pPosition_t pPositionNew, pChartDataView_t pChartData, ou::Colour::EColour colour ) {

  assert( pPositionNew );
  assert( pChartData );
  assert( m_pPortfolio->Id() == pPositionNew->GetRow().idPortfolio );

  ou::tf::Leg leg;
  const LegNote::values_t& legValues( leg.SetPosition( pPositionNew ) );

  if ( LegNote::State::Open == legValues.m_state ) {

    // this will emplace duplicates, not ready for that yet
    //mapComboLeg_t::iterator iterLeg = m_mapComboLeg.emplace( std::move( mapComboLeg_t::value_type( legValues.m_type, std::move( leg ) ) ) );

    // this prevents duplicates for now until proper multi-leg rolls are implemented
    mapComboLeg_t::iterator iterLeg = m_mapComboLeg.find( legValues.m_type );
    if ( m_mapComboLeg.end() == iterLeg ) {
      using result_t = std::pair<mapComboLeg_t::iterator, bool>;
      result_t result;
      //result = m_mapLeg.emplace( std::move( mapLeg_t::value_type( legValues.m_type, std::move( leg ) ) ) );
      //assert( result.second );
      //iterLeg = result.first;
      iterLeg = m_mapComboLeg.emplace( std::move( mapComboLeg_t::value_type( legValues.m_type, std::move( leg ) ) ) );
    }
    else {
      DeactivatePositionOption( iterLeg->second.m_leg.GetPosition() ); // old position
      iterLeg->second.m_leg = std::move( leg ); // overwrite with new leg
    }

    iterLeg->second.m_leg.SetChartData( pChartData, colour ); // comes after as there is no move on indicators

    Init( legValues.m_type );

    pWatch_t pWatch = pPositionNew->GetWatch();
    pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( pWatch );

    const std::string& sName( pWatch->GetInstrumentName() );

    vMenuActivation_t ma;
    ma.emplace_back( MenuActivation( "Calendar Roll", [this,&sName,type=legValues.m_type](){
      std::cout << "Calendar Roll: " << sName << std::endl;
      CalendarRoll( type );
      } ) );
    ma.emplace_back( MenuActivation( "Diagonal Roll", [this,&sName,type=legValues.m_type](){
      std::cout << "Diagonal Roll: " << sName << std::endl;
      DiagonalRoll( type );
      } ) );
    ma.emplace_back( MenuActivation( "Lock Leg",    [this,&sName,type=legValues.m_type](){
      std::cout << "Lock Leg: " << sName << " (todo)" << std::endl;
      LockLeg( type );
      } ) );
    ma.emplace_back( MenuActivation( "Close Leg",    [this,&sName,type=legValues.m_type](){
      std::cout << "Close: " << sName << " (todo)" << std::endl;
      Close( type );
      } ) );

    m_fActivateOption( pOption, pPositionNew, ou::tf::option::LegNote::LU( legValues.m_type ), std::move( ma ) );

    if ( State::Initializing == m_state ) {
      m_state = State::Positions;
    }

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

Combo::ComboLeg& Combo::InitTracker(
  LegNote::Type type,
  const mapChains_t* pmapChains,
  boost::gregorian::date date,
  boost::gregorian::days days_to_expiry
) {

  // assumes only one of type

  ComboLeg& cleg( LU( type ) );

  pPosition_t pPosition( cleg.m_leg.GetPosition() );
  assert( pPosition );
  citerChain_t citerChain = SelectChain( *pmapChains, date, days_to_expiry );
  const chain_t& chain( citerChain->second );

  cleg.m_tracker.Initialize(
    pPosition, &chain,
    [this]( const std::string& sName, fConstructedOption_t&& f ){ // m_fConstructOption
      m_fConstructOption( sName, std::move( f ) );
      },
    [this]( pOption_t pOption, const std::string& sNotes )->pPosition_t { // m_fOpenLeg

      // TODO: will need to supply previous option => stop calc, may need a clean up lambda
      //   then the note change above can be performed elsewhere

      pPosition_t pPosition = m_fOpenPosition( this, pOption, sNotes );
      // Combo::OverwritePosition( pPosition ); - not needed, performed in fOpenPosition
      return pPosition;
    },
    []( pPosition_t pPositionOld, pOption_t pOption, const std::string& sNotes )->pPosition_t { // fRollLeg_t
      pPosition_t pPosition;  // empty for now as placeholder
      assert( false );
      return pPosition;
    },
    [this,&cleg]( pPosition_t pPositionOld ) { // m_fCloseLeg

      const std::string sNotes( pPositionOld->Notes() );
      LegNote ln( sNotes );

      LegNote::values_t values( ln.Values() );
      values.m_state = LegNote::State::Closed;
      ln.Assign( values );
      pPositionOld->SetNotes( ln.Encode() );
      auto& instance( ou::tf::PortfolioManager::GlobalInstance() ); // NOTE this direct call!!
      instance.PositionUpdateNotes( pPositionOld );

      cleg.m_monitor.SetPosition( pPositionOld );
      cleg.m_monitor.ClosePosition();
    }
  );

  return cleg;

}

// NOTE: may require delayed reaction on this, as a roll will call back into this with new position
void Combo::InitTrackLongOption(
    LegNote::Type type,
    const mapChains_t* pmapChains,
    boost::gregorian::date date,
    boost::gregorian::days days_to_expiry
    ) {

  ComboLeg& cleg( InitTracker( type, pmapChains, date, days_to_expiry ) );

  namespace ph = std::placeholders;

  cleg.m_vfTest.emplace( // invalidates on new size() > capacity()
    cleg.m_vfTest.end(),
    std::bind( &ou::tf::option::Tracker::TestLong, &cleg.m_tracker, ph::_1, ph::_2, ph::_3 ) // Tick
//    [ tracker = &cleg.m_tracker ]( boost::posix_time::ptime dt, double dblUnderlyingSlope, double dblUnderlyingPrice ){
//      tracker->TestLong( dt, dblUnderlyingSlope, dblUnderlyingPrice );
//    }
  );
}

void Combo::InitTrackShortOption(
    LegNote::Type type,
    const mapChains_t* pmapChains,
    boost::gregorian::date date,
    boost::gregorian::days days_to_expiry
) {

  ComboLeg& cleg( InitTracker( type, pmapChains, date, days_to_expiry ) );

  // a) buy out 0.10 (simply closing the position)
  // b) rotate if itm (somewhere else, affects long & short)
  // c) stop monitoring out of hours

  namespace ph = std::placeholders;

  cleg.m_vfTest.emplace(
    cleg.m_vfTest.end(),
    std::bind( &ou::tf::option::Tracker::TestShort, &cleg.m_tracker, ph::_1, ph::_2, ph::_3 ) // Tick
//    [ tracker = &cleg.m_tracker ]( boost::posix_time::ptime dt,double dblUnderlyingSlope, double dblUnderlyingPrice ){
//      tracker->TestShort( dt, dblUnderlyingSlope, dblUnderlyingPrice );
//    }
  );

}

void Combo::DeactivatePositionOption( pPosition_t pPosition ) {
  pWatch_t pWatch = pPosition->GetWatch();
  //assert( pWatch->GetInstrument()->IsOption() ); // TODO may need to change based upon other combo types
  pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( pWatch );
  m_fDeactivateOption( pOption );
}

// TODO: make use of doubleUnderlyingSlope to trigger exit latch
void Combo::Tick( double dblUnderlyingSlope, double dblUnderlyingPrice, ptime dt ) {
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    Leg& leg( entry.second.m_leg );
    leg.Tick( dt, dblUnderlyingPrice );
  }

  switch ( m_state ) {  // TODO: make this a per-leg test?  even need state management?
    case State::Executing:
      if ( !AreOrdersActive() ) {
        m_state = State::Watching;
      }
      break;
    case State::Watching:
      //Update( dblUnderlyingSlope, dblPriceUnderlying );
      break;
  }

  // from original Collar
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    ComboLeg& cleg( entry.second );
    //if ( cleg.m_monitor.IsActive() )
    cleg.m_monitor.Tick( dt ); // needs this in multiple states

    for ( vfTest_t::value_type& fTest: cleg.m_vfTest ) {
      fTest( dt, dblUnderlyingSlope, dblUnderlyingPrice );
    }
  }
}

//void Combo::Tick( double dblUnderlyingSlope, double dblUnderlyingPrice, ptime dt ) {
//  Combo::Tick( dblUnderlyingSlope, dblUnderlyingPrice, dt ); // first or last in sequence?


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

void Combo::PlaceOrder( LegNote::Type type, ou::tf::OrderSide::EOrderSide order_side, uint32_t nOrderQuantity ) {

  ComboLeg& cleg( LU( type ) );

  LegNote::Side ln_side = cleg.m_leg.GetLegNote().Values().m_side; // this is normal entry with order_side as buy

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
      cleg.m_leg.PlaceOrder( order_side, nOrderQuantity );
      m_state = State::Executing;
      break;
  }
}

bool Combo::CloseItmLeg( double price ) {
  bool bClosed( false );
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    Leg& leg( entry.second.m_leg );
    bClosed |= leg.CloseItm( price );
  }
  return bClosed;
}

void Combo::CalendarRoll( LegNote::Type type ) {
  mapComboLeg_t::iterator iter = m_mapComboLeg.find( type ); // assumes only one of type
  assert( m_mapComboLeg.end() != iter );
  ComboLeg& leg( iter->second );
  leg.m_tracker.CalendarRoll();
}

void Combo::DiagonalRoll( LegNote::Type type ) {
  mapComboLeg_t::iterator iter = m_mapComboLeg.find( type ); // assumes only one of type
  assert( m_mapComboLeg.end() != iter );
  ComboLeg& leg( iter->second );
  leg.m_tracker.DiagonalRoll();
}

void Combo::LockLeg( LegNote::Type type ) {
  mapComboLeg_t::iterator iter = m_mapComboLeg.find( type ); // assumes only one of type
  assert( m_mapComboLeg.end() != iter );
  ComboLeg& leg( iter->second );
  leg.m_tracker.Lock( false ); // TODO: need to upate LegNote
}

// TODO: need to disable Tracker monitoring out of hours
void Combo::AtClose() {
  // maybe remove options?
}

void Combo::Close( LegNote::Type type ) {
  mapComboLeg_t::iterator iter = m_mapComboLeg.find( type ); // assumes only one of type
  assert( m_mapComboLeg.end() != iter );
  ComboLeg& leg( iter->second );
  leg.m_tracker.Close();
}

bool Combo::CloseItmLegForProfit( double price ) {
  bool bClosed( false );
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    ou::tf::Leg& leg( entry.second.m_leg );
    bClosed |= leg.CloseItmForProfit( price );
  }
  return bClosed;
}

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
void Combo::CloseExpiryItm( double price, const boost::gregorian::date date ) {
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    ou::tf::Leg& leg( entry.second.m_leg );
    leg.CloseExpiryItm( date, price );
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

void Combo::CancelOrders() {
  m_state = State::Canceled;
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    ComboLeg& cleg( entry.second );
    cleg.m_tracker.Quiesce();
    cleg.m_monitor.CancelOrder(); // or wait for completion?

    Leg& leg( cleg.m_leg );
    leg.CancelOrder();
  }
}

void Combo::ClosePositions() {
  m_state = State::Closing;
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    ou::tf::Leg& leg( entry.second.m_leg );
    if ( leg.IsActive() ) {
      pPosition_t pPosition = leg.ClosePosition();
      auto& instance( ou::tf::PortfolioManager::GlobalInstance() ); // NOTE this direct call!!
      instance.PositionUpdateNotes( pPosition );
    }
  }
}

bool Combo::AreOrdersActive() const { // TODO: is an external call still necessary?
  bool bOrdersActive( false );
  for ( const mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    const ou::tf::Leg& leg( entry.second.m_leg );
    bOrdersActive |= leg.IsOrderActive();
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
