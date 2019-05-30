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
 * File:    Strangle.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on May 25, 2019, 10:56 PM
 */

#include "Strangle.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

Strangle::Strangle()
: m_state( State::Initializing ),
  m_bUpperClosed( false ), m_bLowerClosed( false )
{}

Strangle::Strangle( const Strangle&& rhs )
: m_state( rhs.m_state ),
  m_pPortfolio( std::move( rhs.m_pPortfolio ) ),
  m_legCall( std::move( rhs.m_legCall ) ),
  m_legPut( std::move( rhs.m_legPut ) )
{}

void Strangle::SetPortfolio( pPortfolio_t pPortfolio ) {
  //m_legCall.Clear()? or assert is empty?
  //m_legPut.Clear()? or assert is empty?
  m_pPortfolio = pPortfolio;
}

void Strangle::SetPositionCall( pPosition_t pCall ) {
  assert( m_pPortfolio->Id() == pCall->GetRow().idPortfolio );
  m_legCall.SetPosition( pCall );
  m_state = State::Positions;
}
Strangle::pPosition_t Strangle::GetPositionCall() {
  return m_legCall.GetPosition();
}

void Strangle::SetPositionPut( pPosition_t pPut ) {
  assert( m_pPortfolio->Id() == pPut->GetRow().idPortfolio );
  m_legPut.SetPosition( pPut );
  m_state = State::Positions;
}

Strangle::pPosition_t Strangle::GetPositionPut() {
  return m_legPut.GetPosition();
}

void Strangle::Tick( bool bInTrend, double dblPriceUnderlying, ptime dt ) { // TODO: make use of bInTrend to trigger exit latch
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

void Strangle::OrderLongStrangle() { // if volatility drops, then losses occur on premium
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
void Strangle::CancelOrders() {
  m_legCall.CancelOrder();
  m_legPut.CancelOrder();
  m_state = State::Canceled;
}

void Strangle::ClosePositions() {
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

bool Strangle::AreOrdersActive() const { return m_legCall.IsOrderActive() || m_legPut.IsOrderActive(); }

void Strangle::SaveSeries( const std::string& sPrefix ) {
  m_legCall.SaveSeries( sPrefix );
  m_legPut.SaveSeries( sPrefix );
}

void Strangle::AddChartDataCall( pChartDataView_t pChartData, ou::Colour::enumColour colour ) {
  m_legCall.SetColour( colour );
  m_legCall.AddChartData( pChartData );
}

void Strangle::AddChartDataPut( pChartDataView_t pChartData, ou::Colour::enumColour colour ) {
  m_legPut.SetColour( colour );
  m_legPut.AddChartData( pChartData );
}

void Strangle::Update( bool bTrending, double dblPrice ) {
  // TODO: incorporate trending underlying
}

double Strangle::GetNet() {
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
void Strangle::CloseExpiryItm( double price, const boost::gregorian::date date ) {
  m_legCall.CloseExpiryItm( date, price );
  m_legPut.CloseExpiryItm( date, price );
}

void Strangle::CloseFarItm( double price ) {
  pPosition_t pPositionCall = m_legCall.GetPosition();
  pPosition_t pPositionPut  = m_legPut.GetPosition();
  if ( pPositionCall->IsActive() && pPositionPut->IsActive() ) {
    double dblProfitCall = pPositionCall->GetUnRealizedPL();
    double dblProfitPut  = pPositionPut->GetUnRealizedPL();
    // TOOD: finish analysis via TakeProfits - which fixes a quote issue - before continuing here
  }
}

bool Strangle::CloseItmLeg( double price ) {
  bool bClosed( false );
  {
    pPosition_t pPositionCall = m_legCall.GetPosition();
    if ( pPositionCall->IsActive() ) {
      pInstrument_t pInstrument = pPositionCall->GetInstrument();
      if ( price > pInstrument->GetStrike() ) {
        if ( 0.05 < pPositionCall->GetUnRealizedPL() ) {
          m_legCall.ClosePosition();
          bClosed = true;
        }
      }
    }
  }
  {
    pPosition_t pPositionPut  = m_legPut.GetPosition();
    if ( pPositionPut->IsActive() ) {
      pInstrument_t pInstrument = pPositionPut->GetInstrument();
      if ( price < pInstrument->GetStrike() ) {
        if ( 0.05 < pPositionPut->GetUnRealizedPL() ) {
          m_legPut.ClosePosition();
          bClosed = true;
        }
      }
    }
  }
  return bClosed;
}

void Strangle::CloseForProfits( double price ) {
}

void Strangle::TakeProfits( double price ) {
}

} // namespace option
} // namespace tf
} // namespace ou
