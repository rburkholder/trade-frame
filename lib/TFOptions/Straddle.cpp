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
 * File:    Straddle.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on May 25, 2019, 10:56 PM
 */

#include "Straddle.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

Straddle::Straddle( double dblStrikeAtm )
: m_state( State::Initializing ),
  m_bUpperClosed( false ), m_bLowerClosed( false ),
  m_dblStrikeAtm( dblStrikeAtm )
{}

Straddle::Straddle( const Straddle&& rhs )
: m_state( rhs.m_state ),
  m_dblStrikeAtm( rhs.m_dblStrikeAtm ),
  m_legCall( std::move( rhs.m_legCall ) ),
  m_legPut( std::move( rhs.m_legPut ) )
{}

void Straddle::SetOptionCall( pOption_t pCall, ou::Colour::enumColour colour ) {
  m_scCall.SetWatch( pCall );
  m_legCall.SetColour( colour );
  m_state = State::Validating;
}

Straddle::pOption_t Straddle::GetOptionCall() {
  return boost::dynamic_pointer_cast<ou::tf::option::Option>( m_scCall.GetWatch() );
}

void Straddle::SetOptionPut( pOption_t pPut, ou::Colour::enumColour colour ) {
  m_scPut.SetWatch( pPut );
  m_legPut.SetColour( colour );
  m_state = State::Validating;
}

Straddle::pOption_t Straddle::GetOptionPut() { 
  return boost::dynamic_pointer_cast<ou::tf::option::Option>( m_scPut.GetWatch() );
}

bool Straddle::ValidateSpread( size_t nDuration ) {
  bool bResult( false );
  switch ( m_state ) {
    case State::Validating:
      bResult = ( m_scCall.ValidateSpread( nDuration ) && m_scPut.ValidateSpread( nDuration ) );
      break;
  }
  return bResult;
}

void Straddle::SetPositionCall( pPosition_t pCall ) {
  m_scCall.Clear();
  m_legCall.SetPosition( pCall );
  m_state = State::Positions;
}
Straddle::pPosition_t Straddle::GetPositionCall() { 
  return m_legCall.GetPosition();
}

void Straddle::SetPositionPut( pPosition_t pPut ) {
  m_scPut.Clear();
  m_legPut.SetPosition( pPut );
  m_state = State::Positions;
}

Straddle::pPosition_t Straddle::GetPositionPut() {
  return m_legPut.GetPosition();
}

void Straddle::Tick( bool bInTrend, double dblPriceUnderlying, ptime dt ) { // TODO: make use of bInTrend to trigger exit latch
  m_legCall.Tick( dt );
  m_legPut.Tick( dt );
  switch ( m_state ) {  // TODO: make this a per-leg test?  even need state management?
    case State::Executing:
      if ( !AreOrdersActive() ) {
        m_state = State::Watching;
      }
      break;
    case State::Watching:
      Update( bInTrend, dblPriceUnderlying );
      break;
  }
}

void Straddle::OrderLongStraddle() { // if volatility drops, then losses occur on premium
  switch ( m_state ) {
    case State::Positions: // doesn't confirm both put/call are available
    case State::Watching:
      m_legCall.OrderLong( 1 );
      m_legPut.OrderLong( 1 );
      m_state = State::Executing;
      m_bUpperClosed = false;
      m_bLowerClosed = false;
      break;
  }
}
void Straddle::CancelOrders() {
  m_legCall.CancelOrder();
  m_legPut.CancelOrder();
  m_state = State::Canceled;
}

void Straddle::ClosePositions() {
  if ( !m_bUpperClosed ) {
    m_legCall.ClosePosition();
    m_bUpperClosed = true;
  }
  if ( !m_bLowerClosed ) {
    m_legPut.ClosePosition();
    m_bLowerClosed = true;
  }
  m_state = State::Closing;
}

bool Straddle::AreOrdersActive() const { return m_legCall.IsOrderActive() || m_legPut.IsOrderActive(); }

void Straddle::SaveSeries( const std::string& sPrefix ) {
  m_legCall.SaveSeries( sPrefix );
  m_legPut.SaveSeries( sPrefix );
}

void Straddle::AddChartData( pChartDataView_t pChartData ) {
  m_legCall.AddChartData( pChartData );
  m_legPut.AddChartData( pChartData );
}

void Straddle::SetColours( ou::Colour::enumColour colourCall, ou::Colour::enumColour colourPut ) {
  m_legCall.SetColour( colourCall );
  m_legPut.SetColour( colourPut );
}

void Straddle::SetColourCall( ou::Colour::enumColour colour ) {
  m_legCall.SetColour( colour );
}

void Straddle::SetColourPut( ou::Colour::enumColour colour ) {
  m_legPut.SetColour( colour );
}

double Straddle::GetNet() {
  double dblNet {};
  pPosition_t pPositionCall = m_legCall.GetPosition();
  if ( pPositionCall ) {
    double dblCallValue = pPositionCall->GetUnRealizedPL();
    std::cout
      << "leg call: "
      << pPositionCall->GetInstrument()->GetInstrumentName()
      << "=" << dblCallValue;
    dblNet += dblCallValue;
    if ( 0.0 == dblCallValue ) {
      const ou::tf::Quote& quote( pPositionCall->GetWatch()->LastQuote() );
      std::cout
        << ", quote: a" << quote.Ask()
        << ", b" << quote.Bid()
        ;
    }
    std::cout << std::endl;
  }
  pPosition_t pPositionPut = m_legPut.GetPosition();
  if ( pPositionPut ) {
    double dblPutValue = pPositionPut->GetUnRealizedPL();
    std::cout
      << "leg put: "
      << pPositionPut->GetInstrument()->GetInstrumentName()
      << "=" << dblPutValue;
    dblNet += dblPutValue;
    if ( 0.0 == dblPutValue ) {
      const ou::tf::Quote& quote( pPositionPut->GetWatch()->LastQuote() );
      std::cout
        << ", quote: a" << quote.Ask()
        << ", b" << quote.Bid()
        ;
    }
    std::cout << std::endl;
  }
//      double dblNetValue = dblCallValue + dblPutValue;
//      if ( ( 0.0 != dblCallValue ) && ( 0.0 != dblPutValue ) ) {
//        std::cout
//          << "net value=" << dblNetValue
//          << " "
//          << pPositionCall->GetInstrument()->GetInstrumentName() << ":" << dblCallValue
//          << " - "
//          << pPositionPut->GetInstrument()->GetInstrumentName() << ":" << dblPutValue
//          << std::endl;
//      }
  return dblNet;
}

// prevent exercise or assignment at expiry
// however, the otm leg may need an exist or roll if there is premium remaining (>$0.05)
// so ... the logic needs changing, re-arranging
void Straddle::CloseExpiryItm( const boost::gregorian::date date, double price ) {
  m_legCall.CloseExpiryItm( date, price );
  m_legPut.CloseExpiryItm( date, price );
}

void Straddle::Update( bool bTrending, double dblPrice ) { // TODO: incorporate trending underlying
}

} // namespace option
} // namespace tf
} // namespace ou
