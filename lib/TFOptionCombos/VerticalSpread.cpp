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
 * File:    VerticalSpread.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 11, 2019, 8:03 PM
 */

#include "VerticalSpread.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

VerticalSpread::VerticalSpread( )
: Combo()
{
}

VerticalSpread::VerticalSpread( VerticalSpread&& rhs )
: Combo( std::move( rhs ) )
{
}

VerticalSpread::~VerticalSpread( ) { }

void VerticalSpread::Tick( bool bInTrend, double dblPriceUnderlying, ptime dt ) {
  Combo::Tick( bInTrend, dblPriceUnderlying, dt ); // first or last in sequence?
//  if ( m_vLeg.empty() ) {
//    ChooseStrikes( dblPriceUnderlying );
//  }
}

// ==========

ShortVerticalSpread::ShortVerticalSpread( )
: VerticalSpread()
{
}

ShortVerticalSpread::ShortVerticalSpread( ShortVerticalSpread&& rhs )
: VerticalSpread( std::move( rhs ) )
{
}

ShortVerticalSpread::~ShortVerticalSpread( ) { }

void ShortVerticalSpread::Tick( bool bInTrend, double dblPriceUnderlying, ptime dt ) {
  VerticalSpread::Tick( bInTrend, dblPriceUnderlying, dt ); // first or last in sequence?
//  if ( m_vLeg.empty() ) {
//    ChooseStrikes( dblPriceUnderlying );
//  }
}

// ==========

LongVerticalSpread::LongVerticalSpread( )
: VerticalSpread()
{
}

LongVerticalSpread::LongVerticalSpread( LongVerticalSpread&& rhs )
: VerticalSpread( std::move( rhs ) )
{
}

LongVerticalSpread::~LongVerticalSpread( ) { }

void LongVerticalSpread::Tick( bool bInTrend, double dblPriceUnderlying, ptime dt ) {
  VerticalSpread::Tick( bInTrend, dblPriceUnderlying, dt ); // first or last in sequence?
//  if ( m_vLeg.empty() ) {
//    ChooseStrikes( dblPriceUnderlying );
//  }
}

} // namespace option
} // namespace tf
} // namespace ou

