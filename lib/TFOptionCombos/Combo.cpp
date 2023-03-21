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

Combo::ComboLeg& Combo::operator[]( LegNote::Type type ) {
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

  Leg leg;
  const LegNote::values_t& legValues( leg.SetPosition( pPositionNew ) );

  if ( LegNote::State::Open == legValues.m_state ) {

    mapComboLeg_t::iterator iterLeg = m_mapComboLeg.emplace( std::move( mapComboLeg_t::value_type( legValues.m_type, std::move( leg ) ) ) );
    //DeactivatePositionOption( iterLeg->second.GetPosition() ); // old position
    //iterLeg->second = std::move( leg ); // overwrite with new leg

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
}

double Combo::GetNet( double price ) {

  double dblNet {};
  double dblConstructedValue {};
  double dblDelta {};
  double dblGamma {};

  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    Leg& leg( entry.second.m_leg );
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

bool Combo::CloseItmLeg( double price ) {
  bool bClosed( false );
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    Leg& leg( entry.second.m_leg );
    bClosed |= leg.CloseItm( price );
  }
  return bClosed;
}

bool Combo::CloseItmLegForProfit( double price ) {
  bool bClosed( false );
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    Leg& leg( entry.second.m_leg );
    bClosed |= leg.CloseItmForProfit( price );
  }
  return bClosed;
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
    Leg& leg( entry.second.m_leg );
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
    Leg& leg( entry.second.m_leg );
    leg.CancelOrder();
  }
}

void Combo::ClosePositions() {
  m_state = State::Closing;
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    Leg& leg( entry.second.m_leg );
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
    const Leg& leg( entry.second.m_leg );
    bOrdersActive |= leg.IsOrderActive();
  }
  return bOrdersActive;
}

void Combo::SaveSeries( const std::string& sPrefix ) {
  // TODO: after legs are closed, can they still be saved?
  //   should something else be used?  maybe OptionRepository instead?
  for ( mapComboLeg_t::value_type& entry: m_mapComboLeg ) {
    Leg& leg( entry.second.m_leg );
    leg.SaveSeries( sPrefix );
  }
}

} // namespace option
} // namespace tf
} // namespace ou
