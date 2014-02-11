/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include "PositionState.h"

PositionState::PositionState( void ) 
  : m_ix( 0 ), m_dblStop( 0.0 ), m_dblTarget( 0.0 )
{
}

PositionState::PositionState( size_t ix, pPosition_t pPosition ) 
  : m_ix( ix ), m_pPosition( pPosition ), m_dblStop( 0.0 ), m_dblTarget( 0.0 )
{
  m_pPosition->OnPositionChanged.Add( fastdelegate::MakeDelegate( this, &PositionState::HandlePositionChanged ) );
}

PositionState::PositionState( const PositionState& rhs ) 
  : m_ix( rhs.m_ix ), m_pPosition( rhs.m_pPosition ), m_dblStop( rhs.m_dblStop ), m_dblTarget( rhs.m_dblTarget )
{
}

PositionState::~PositionState(void) { 
  if ( 0 != m_pPosition.get() )
    m_pPosition->OnPositionChanged.Remove( fastdelegate::MakeDelegate( this, &PositionState::HandlePositionChanged ) );
}

void PositionState::Buy( uint32_t quan ) {
  m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, quan );
}

void PositionState::Sell( uint32_t quan ) {
  m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, quan );
}

void PositionState::HandlePositionChanged( const ou::tf::Position& position ) {
  const ou::tf::Position::TableRowDef& row( position.GetRow() );
  if ( ( 0 == row.nPositionActive ) && ( 0 == row.nPositionPending ) ) {
    if ( 0 != OnPositionClosed ) OnPositionClosed( *this );
  }
}

void PositionState::ExitLong( void ) {
  const ou::tf::Position::TableRowDef& row( m_pPosition->GetRow() );
  if ( 0.0 < ( row.dblUnRealizedPL ) ) {
    m_pPosition->ClosePosition();
  }
}

void PositionState::ExitShort( void ) {
  const ou::tf::Position::TableRowDef& row( m_pPosition->GetRow() );
  if ( 0.0 < ( row.dblUnRealizedPL ) ) {
    m_pPosition->ClosePosition();
  }
}
