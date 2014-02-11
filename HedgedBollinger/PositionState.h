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

// started 2014/02/6

#pragma once

#include <OUCommon/FastDelegate.h>

#include <TFTrading/Position.h>

class PositionState {
public:

  typedef ou::tf::Position::pPosition_t pPosition_t;

  PositionState( void );
  PositionState( size_t ix, pPosition_t pPosition );
  PositionState( const PositionState& rhs );
  virtual ~PositionState(void);

  typedef fastdelegate::FastDelegate1<const PositionState&> OnPositionClosed_t;
  OnPositionClosed_t OnPositionClosed;

  void Buy( uint32_t quan );
  void Sell( uint32_t quan );

  void ExitLong( void );
  void ExitShort( void );

  pPosition_t Position( void ) { return m_pPosition; }
  size_t Index( void ) const { return m_ix; }

protected:
  pPosition_t m_pPosition;
private:

  size_t m_ix;  // index to use during callbacks

  double m_dblStop;
  double m_dblTarget;

  void HandlePositionChanged( const ou::tf::Position& position );
  
};

