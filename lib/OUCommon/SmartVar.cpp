/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "stdafx.h"

#include "SmartVar.h"

namespace ou {

template<> SmartVar<int>::SmartVar( void ) 
  : m_bValueUpdated( false ), m_bEventCleared( true ), m_tBlank( 0 ), m_tItem( 0 ) {
}

template<> SmartVar<unsigned int>::SmartVar( void ) 
  : m_bValueUpdated( false ), m_bEventCleared( true ), m_tBlank( 0 ), m_tItem( 0 ) {
}

template<> SmartVar<double>::SmartVar( void ) 
  : m_bValueUpdated( false ), m_bEventCleared( true ), m_tBlank( 0.0 ), m_tItem( 0.0 ) {
}

} // ou
