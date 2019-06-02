/************************************************************************
 * Copyright(c) 2018, One Unified. All rights reserved.                 *
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
 * File:   PositionGreek.cpp
 * Author: raymond@burkholder.net
 * 
 * Created on July 31, 2018, 9:21 AM
 */

#include "PositionGreek.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
  
// TODO: add additional constructors for use with inherited Position

PositionGreek::PositionGreek( pOption_t& pOption, pUnderlying_t& pUnderlying )
: Position( pOption, pProvider_t() ), // supply empty execution provider for now
  m_pOption( pOption ), m_pUnderlying( pUnderlying ), m_nQuantity( 0 )
{
  // NOTE:  may need to construct with a 'row'
  Position::Set( pOption->GetInstrument()->GetInstrumentName() );
  Construction();
}

PositionGreek::~PositionGreek( ) {
  //std::cout << "PositionGreek::Destruction: " << m_row.sName << std::endl;
  m_pOption->OnGreek.Remove( MakeDelegate( this, &PositionGreek::HandleGreek ) );
}

void PositionGreek::Construction() {
  m_pOption->OnGreek.Add( MakeDelegate( this, &PositionGreek::HandleGreek ) );
  //std::cout << "PositionGreek::Construction: " << m_row.sName << std::endl;
}

void PositionGreek::HandleGreek( greek_t greek ) {
  OnGreek( greek );
}

void PositionGreek::PositionPendingDelta( int n ) {
  switch ( m_row.eOrderSidePending ) {
    case OrderSide::Unknown:
      m_row.nPositionPending++;
      if ( 1 == n ) {
        m_row.eOrderSidePending = OrderSide::Buy;
      }
      if ( -1 == n ) {
        m_row.eOrderSidePending = OrderSide::Sell;
      }
      break;
    case OrderSide::Buy:
      if ( 1 == n ) {
        m_row.nPositionPending++;
        //m_row.eOrderSidePending = OrderSide::Buy;
      }
      if ( -1 == n ) {
        m_row.nPositionPending--;
        if ( 0 == m_row.nPositionPending ) m_row.eOrderSidePending = OrderSide::Unknown;
      }
      break;
    case OrderSide::Sell:
      if ( 1 == n ) {
        m_row.nPositionPending--;
        if ( 0 == m_row.nPositionPending ) m_row.eOrderSidePending = OrderSide::Unknown;
      }
      if ( -1 == n ) {
        m_row.nPositionPending++;
        //m_row.eOrderSidePending = OrderSide::Sell;
      }
      break;
  }
  OnPositionChanged( *this );
}

// TODO: multiply by multiplier and position size
std::ostream& operator<<( std::ostream& os, const PositionGreek& position ) {
  const ou::tf::Greek& greek( position.m_pOption->LastGreek() );
  os 
    << (Position) position
    << ", ImpVol " << greek.ImpliedVolatility() 
    << ", Delta " << greek.Delta()
    << ", Gamma " << greek.Gamma()
    << ", Theta " << greek.Theta()
    << ", Vega " << greek.Vega()
    << ", Rho " << greek.Rho()
    ;
  return os;
}

} // namespace tf
} // namespace ou