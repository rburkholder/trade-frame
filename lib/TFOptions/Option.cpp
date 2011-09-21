/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include "Option.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

Option::Option( pInstrument_t pInstrument ) 
: m_pInstrument( pInstrument ),
  m_dblBid( 0 ), m_dblAsk( 0 ), m_dblTrade( 0 ),
  m_dblStrike( pInstrument->GetStrike() ),
  m_bWatching( false ),
  m_sSide( "-" )
{
}

Option::Option( const Option& rhs ) 
: m_dblBid( rhs.m_dblBid ), m_dblAsk( rhs.m_dblAsk ), m_dblTrade( rhs.m_dblTrade ),
  m_dblStrike( rhs.m_dblStrike ),
  m_greek( rhs.m_greek ),
  m_bWatching( false ),
  m_sSide( rhs.m_sSide ),
  m_pInstrument( rhs.m_pInstrument )
{
  assert( !rhs.m_bWatching );
}

Option& Option::operator=( const Option& rhs ) {
  assert( !rhs.m_bWatching );
  assert( !m_bWatching );
  m_dblStrike = rhs.m_dblStrike;
  m_greek = rhs.m_greek;
  m_sSide = rhs.m_sSide;
  m_pInstrument = rhs.m_pInstrument;
  return *this;
}

void Option::HandleQuote( const CQuote& quote ) {
  m_dblBid = quote.Bid();
  m_dblAsk = quote.Ask();
  m_quotes.Append( quote );
}

void Option::HandleTrade( const CTrade& trade ) {
  m_dblTrade = trade.Trade();
  m_trades.Append( trade );
}

void Option::HandleGreek( const CGreek& greek ) {
  m_greek = greek;
  m_greeks.Append( greek );
}

//
// ==================
//

Call::Call( pInstrument_t pInstrument )
: Option( pInstrument )
{
  // assert instrument is a call
  assert( ou::tf::OptionSide::Call == pInstrument->GetOptionSide() );
  m_sSide = "C";
}

//
// ==================
//

Put::Put( pInstrument_t pInstrument )
: Option( pInstrument )
{
  // assert instrument is a put
  assert( ou::tf::OptionSide::Put == pInstrument->GetOptionSide() );
  m_sSide = "P";
}


//
// ==================
//

} // namespace option
} // namespace tf
} // namespace ou

