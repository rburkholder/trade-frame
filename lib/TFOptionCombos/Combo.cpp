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
  m_mapLeg( std::move( rhs.m_mapLeg ) ),
  m_pPortfolio( std::move( rhs.m_pPortfolio ) ),
  m_fConstructOption( std::move( rhs.m_fConstructOption ) ),
  m_fActivateOption( std::move( rhs.m_fActivateOption ) ),
  m_fOpenPosition( std::move( rhs.m_fOpenPosition ) ),
  m_fDeactivateOption( std::move( rhs.m_fDeactivateOption ) )
{
}

Combo::~Combo() {
  for ( mapLeg_t::value_type& vt: m_mapLeg ) {
    DeactivatePositionOption( vt.second.GetPosition() );
  }
  m_mapLeg.clear();
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
  assert( m_mapLeg.empty() );
  m_pPortfolio = pPortfolio;
}

// will over-write existing Leg, needs notes field in pPosition
const LegNote::values_t& Combo::SetPosition(  pPosition_t pPositionNew, pChartDataView_t pChartData, ou::Colour::EColour colour ) {

  assert( pPositionNew );
  assert( pChartData );
  assert( m_pPortfolio->Id() == pPositionNew->GetRow().idPortfolio );

  Leg leg;
  const LegNote::values_t& legValues( leg.SetPosition( pPositionNew ) );

  if ( LegNote::State::Open == legValues.m_state ) {

    mapLeg_t::iterator iter = m_mapLeg.find( legValues.m_type );
    if ( m_mapLeg.end() == iter ) {
      using result_t = std::pair<mapLeg_t::iterator, bool>;
      result_t result;
      result = m_mapLeg.emplace( std::move( mapLeg_t::value_type( legValues.m_type, std::move( leg ) ) ) );
      assert( result.second );
      iter = result.first;
    }
    else {
      DeactivatePositionOption( iter->second.GetPosition() ); // old position
      iter->second = std::move( leg ); // overwrite with new leg
    }

    iter->second.SetChartData( pChartData, colour ); // comes after as there is no move on indicators

    Init( legValues.m_type );

    pWatch_t pWatch = pPositionNew->GetWatch();
    pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( pWatch );
    m_fActivateOption( pOption );

    if ( State::Initializing == m_state ) {
      m_state = State::Positions;
    }

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
  for ( mapLeg_t::value_type& entry: m_mapLeg ) {
    Leg& leg( entry.second );
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

  for ( mapLeg_t::value_type& entry: m_mapLeg ) {
    Leg& leg( entry.second );
    dblNet += leg.GetNet( price );
    double dblLegConstructedValue = leg.ConstructedValue();
    std::cout << ",constructed@" << dblLegConstructedValue;
    dblConstructedValue += dblLegConstructedValue;
    std::cout << std::endl;
  }

  std::cout << "  combo constructed: " << dblConstructedValue << std::endl;
  return dblNet;
}

bool Combo::CloseItmLeg( double price ) {
  bool bClosed( false );
  for ( mapLeg_t::value_type& entry: m_mapLeg ) {
    Leg& leg( entry.second );
    bClosed |= leg.CloseItm( price );
  }
  return bClosed;
}

bool Combo::CloseItmLegForProfit( double price ) {
  bool bClosed( false );
  for ( mapLeg_t::value_type& entry: m_mapLeg ) {
    Leg& leg( entry.second );
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
  for ( mapLeg_t::value_type& entry: m_mapLeg ) {
    Leg& leg( entry.second );
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
  for ( mapLeg_t::value_type& entry: m_mapLeg ) {
    Leg& leg( entry.second );
    leg.CancelOrder();
  }
}

void Combo::ClosePositions() {
  m_state = State::Closing;
  for ( mapLeg_t::value_type& entry: m_mapLeg ) {
    Leg& leg( entry.second );
    if ( leg.IsActive() ) {
      pPosition_t pPosition = leg.ClosePosition();
      auto& instance( ou::tf::PortfolioManager::Instance() ); // NOTE this direct call!!
      instance.PositionUpdateNotes( pPosition );
    }
  }
}

bool Combo::AreOrdersActive() const { // TODO: is an external call still necessary?
  bool bOrdersActive( false );
  for ( const mapLeg_t::value_type& entry: m_mapLeg ) {
    const Leg& leg( entry.second );
    bOrdersActive |= leg.IsOrderActive();
  }
  return bOrdersActive;
}

void Combo::SaveSeries( const std::string& sPrefix ) {
  // TODO: after legs are closed, can they still be saved?
  //   should something else be used?  maybe OptionRepository instead?
  for ( mapLeg_t::value_type& entry: m_mapLeg ) {
    Leg& leg( entry.second );
    leg.SaveSeries( sPrefix );
  }
}

Combo::citerChain_t Combo::SelectChain( const mapChains_t& mapChains, boost::gregorian::date date, boost::gregorian::days daysToExpiry ) {
  citerChain_t citerChain = std::find_if( mapChains.begin(), mapChains.end(),
    [date,daysToExpiry](const mapChains_t::value_type& vt)->bool{
      return daysToExpiry <= ( vt.first - date );  // first chain where trading date less than expiry date
  } );
  if ( mapChains.end() == citerChain ) {
    throw ou::tf::option::exception_chain_not_found( "Combo::SelectChain" );
  }
  return citerChain;
}

} // namespace option
} // namespace tf
} // namespace ou
