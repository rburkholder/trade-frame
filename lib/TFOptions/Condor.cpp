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
 * File:    Condor.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 10, 2019, 6:24 PM
 */

#include "Condor.h"

// TODO: use VerticalSpread x 2?
//    need to use a vertical spread: bear call, enter for credit
//    need to use a vertical spread: bull put,  enter for credit

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

using pInstrument_t = ou::tf::Instrument::pInstrument_t;
using pOption_t = Option::pOption_t;

Condor::Condor()
: Combo()
{
}

Condor::Condor( Condor&& rhs )
: Combo( std::move( rhs ) )
{
}

Condor::~Condor() {
}

void Condor::Tick( bool bInTrend, double dblPriceUnderlying, ptime dt ) {
  Combo::Tick( bInTrend, dblPriceUnderlying, dt ); // first or last in sequence?
//  if ( m_vLeg.empty() ) {
//    ChooseStrikes( dblPriceUnderlying );
//  }
}

// ==========

ShortCondor::ShortCondor( )
: Condor()
{
}

ShortCondor::ShortCondor( ShortCondor&& rhs )
: Condor( std::move( rhs ) )
{
}

ShortCondor::~ShortCondor( ) { }

void ShortCondor::Tick( bool bInTrend, double dblPriceUnderlying, ptime dt ) {
  Condor::Tick( bInTrend, dblPriceUnderlying, dt ); // first or last in sequence?
//  if ( m_vLeg.empty() ) {
//    ChooseStrikes( dblPriceUnderlying );
//  }
}

// ==========

LongCondor::LongCondor( )
: Condor()
{
}

LongCondor::LongCondor( LongCondor&& rhs )
: Condor( std::move( rhs ) )
{
}

LongCondor::~LongCondor( ) { }

void LongCondor::Tick( bool bInTrend, double dblPriceUnderlying, ptime dt ) {
  Condor::Tick( bInTrend, dblPriceUnderlying, dt ); // first or last in sequence?
//  if ( m_vLeg.empty() ) {
//    ChooseStrikes( dblPriceUnderlying );
//  }
}



} // namespace option
} // namespace tf
} // namespace ou

