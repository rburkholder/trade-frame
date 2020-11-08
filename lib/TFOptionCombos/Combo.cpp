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

#include "Combo.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

const double Combo::m_dblMaxStrikeDelta( 0.51 );       // not 0.50 to prevent rounding problems.
const double Combo::m_dblMaxStrangleDelta( 1.01 );     // not 1.00 to prevent rounding problems

Combo::Combo( )
: m_state( State::Initializing )
{
  m_vLeg.reserve( 16 ); // required for leg.AddChartData
}

Combo::Combo( const Combo& rhs )
: m_state( rhs.m_state ),
  m_vLeg( rhs.m_vLeg ),
  m_pPortfolio( rhs.m_pPortfolio )
{
  m_vLeg.reserve( 16 ); // required for leg.AddChartData -- is this actually needed?
}

Combo::Combo( const Combo&& rhs )
: m_state( rhs.m_state ),
  m_vLeg( std::move( rhs.m_vLeg ) ),
  m_pPortfolio( std::move( rhs.m_pPortfolio ) )
{
  m_vLeg.reserve( 16 ); // required for leg.AddChartData -- is this actually needed?
}

Combo::~Combo( ) {
}

void Combo::Initialize(
  boost::gregorian::date date,
  const mapChains_t* pmapChains,
  fConstructOption_t&& fConstructOption
) {
  //m_pmapChains = pmapChains;
  m_fConstructOption = std::move( fConstructOption );
  Init( date, pmapChains );
}

void Combo::SetPortfolio( pPortfolio_t pPortfolio ) {
  assert( m_vLeg.empty() );
  m_pPortfolio = pPortfolio;
}

// TODO:  need to accept existing positions, need to create own new positions
void Combo::AddPosition( pPosition_t pPosition, pChartDataView_t pChartData, ou::Colour::enumColour colour ) {
  bool bLegFound( false );
  for ( Leg& leg: m_vLeg ) {
    if ( pPosition->GetInstrument()->GetInstrumentName() == leg.GetPosition()->GetInstrument()->GetInstrumentName() ) {
      bLegFound = true;
      break;
    }
  }
  if ( !bLegFound ) {
    assert( m_vLeg.size() < m_vLeg.capacity() );
    assert( m_pPortfolio->Id() == pPosition->GetRow().idPortfolio );
    Leg leg( pPosition );
    m_vLeg.emplace_back( std::move( leg ) );
    m_vLeg.back().SetColour( colour ); // comes after as there is no move on indicators
    m_vLeg.back().AddChartData( pChartData ); // comes after as there is no move on indicators
  }
  if ( State::Initializing == m_state ) {
    m_state = State::Positions;
  }

}

// TODO: make use of doubleUnderlyingSlope to trigger exit latch
void Combo::Tick( double doubleUnderlyingSlope, double dblPriceUnderlying, ptime dt ) {
  for ( Leg& leg: m_vLeg ) {
    leg.Tick( dt );
  }
  switch ( m_state ) {  // TODO: make this a per-leg test?  even need state management?
    case State::Executing:
      if ( !AreOrdersActive() ) {
        m_state = State::Watching;
      }
      break;
    case State::Watching:
      Update( doubleUnderlyingSlope, dblPriceUnderlying );
      break;
  }
}

double Combo::GetNet( double price ) {
  double dblNet {};
  double dblConstructedValue {};
  for ( Leg& leg: m_vLeg ) {
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
  for ( Leg& leg: m_vLeg ) {
    bClosed |= leg.CloseItm( price );
  }
  return bClosed;
}

bool Combo::CloseItmLegForProfit( double price ) {
  bool bClosed( false );
  for ( Leg& leg: m_vLeg ) {
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
  for ( Leg& leg: m_vLeg ) {
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
  for ( Leg& leg: m_vLeg ) {
    leg.CancelOrder();
  }
  m_state = State::Canceled;
}

void Combo::ClosePositions() {
  for ( Leg& leg: m_vLeg ) {
    if ( leg.IsActive() ) {
      leg.ClosePosition();
    }
  }
  m_state = State::Closing;
}

bool Combo::AreOrdersActive() const { // TODO: is an external call still necessary?
  bool bOrdersActive( false );
  for ( const Leg& leg: m_vLeg ) {
    bOrdersActive |= leg.IsOrderActive();
  }
  return bOrdersActive;
}

void Combo::SaveSeries( const std::string& sPrefix ) {
  for ( Leg& leg: m_vLeg ) {
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
