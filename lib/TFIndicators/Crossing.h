/************************************************************************
 * Copyright(c) 2014, One Unified. All rights reserved.                 *
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

// Started 2014/02/22 

#pragma once

namespace ou {
namespace tf { // TradeFrame

template<typename T>
class Crossing {
public:
  enum enumCrossing { EUnknown, ELT, EGT, ELTX, EGTX }; // Less Than 0, Greater Than 0, X = crossing detected
  Crossing( void );
  Crossing( T val, T hysterisis = 0 ); // set crossing to something other than zero
  Crossing( const Crossing& rhs );
  ~Crossing( void );
  enumCrossing State( void ) const { return m_state; }
  enumCrossing State( T val );
  enumCrossing State( T val, T cross );  // cross is new crossing level
protected:
private:
  enumCrossing m_state;
  T m_hysterisis;  // not used yet
  T m_cross;
};

template<typename T>
Crossing<T>::Crossing(void): m_state( EUnknown ), m_cross( 0 ), m_hysterisis( 0 ) {
}

template<typename T>
Crossing<T>::Crossing( T cross, T hysterisis ): m_state( EUnknown ), m_cross( cross ), m_hysterisis( hysterisis ) {
}

template<typename T>
Crossing<T>::Crossing( const Crossing& rhs ): m_state( rhs.m_state ), m_cross( rhs.m_cross ), m_hysterisis( rhs.m_hysterisis ) {
}

template<typename T>
Crossing<T>::~Crossing(void) {
}

template<typename T>
typename Crossing<T>::enumCrossing Crossing<T>::State( T val, T cross ) {
  m_cross = cross;
  return State( val );
}

template<typename T>
typename Crossing<T>::enumCrossing Crossing<T>::State( T val ) {
  switch ( m_state ) {
  case EUnknown:
    if ( m_cross < val ) m_state = EGT;
    if ( m_cross > val ) m_state = ELT;
    break;
  case ELT:
    if ( m_cross < val ) m_state = EGTX;
    break;
  case EGT:
    if ( m_cross > val ) m_state = ELTX;
    break;
  case ELTX: 
    if ( m_cross < val ) m_state = EGTX;
    else m_state = ELT;
    break;
  case EGTX:
    if ( m_cross > val ) m_state = ELTX;
    else m_state = EGT;
    break;
  }

  return m_state;
}

} // namespace tf
} // ou