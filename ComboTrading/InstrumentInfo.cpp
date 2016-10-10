/************************************************************************
 * Copyright(c) 2016, One Unified. All rights reserved.                 *
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
 * File:   InstrumentInfo.cpp
 * Author: rpb
 * 
 * Created on October 8, 2016, 6:58 PM
 */

#include "InstrumentInfo.h"

InstrumentInfo::InstrumentInfo( pInstrument_t pInstrument, pProvider_t pProvider )
: m_pWatch( 0 ), m_pInstrument( pInstrument ), m_pDataProvider( pProvider )
{
  m_pWatch = new ou::tf::Watch( pInstrument, pProvider );
}

//InstrumentInfo::InstrumentInfo(const InstrumentInfo& orig): m_pWatch( 0 ) {
//}

InstrumentInfo::~InstrumentInfo() {
  UnWatch();
}

void InstrumentInfo::Watch( void ) {
  if ( 0 == m_pWatch ) {
    assert( 0 != m_pDataProvider.get() );  // need to get DataProvider set from somewhere, hopefully during initialization
    m_pWatch = new ou::tf::Watch( m_pInstrument, m_pDataProvider );
    m_pWatch->StartWatch();
  }
}

void InstrumentInfo::UnWatch( void ) {
  if ( 0 != m_pWatch ) {
    if ( m_pWatch->Watching() ) {
      m_pWatch->StopWatch();
    }
    delete m_pWatch;
    m_pWatch = 0;
  }
}

void InstrumentInfo::Emit( void ) {
  if ( 0 != m_pWatch ) {
    m_pWatch->EmitValues();
  }
}
