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

PositionGreek::PositionGreek( pOption_t pOption )
: Position( pOption->GetInstrument(), pProvider_t(), pOption->GetProvider() ), // supply empty execution provider for now
  m_pOption( pOption )
{
  // NOTE:  may need to construct with a 'row'
  Position::Set( pOption->GetInstrument()->GetInstrumentName() );
  Construction();
}

PositionGreek::~PositionGreek( ) {
  m_pOption->OnGreek.Remove( MakeDelegate( this, &PositionGreek::HandleGreek ) );
}

void PositionGreek::Construction() {
  m_pOption->OnGreek.Add( MakeDelegate( this, &PositionGreek::HandleGreek ) );
}

void PositionGreek::HandleGreek( greek_t greek ) {
}

std::ostream& operator<<( std::ostream& os, const PositionGreek& position ) {
  const ou::tf::Greek& greek( position.m_pOption->LastGreek() );
  os 
    << (Position) position
    << ", ImpVol " << greek.ImpliedVolatility() 
    << ", Delta " << greek.Delta()
    << ", Gamma " << greek.Gamma()
    ;
  return os;
}

} // namespace tf
} // namespace ou