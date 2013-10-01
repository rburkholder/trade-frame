/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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
#include "TSVolatility.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace hf { // high frequency

TSVolatility::TSVolatility( Prices& series, time_duration dtTau, time_duration dtTauPrime, double p, unsigned int n ) 
  : m_seriesSource( series ), 
    m_dtTau( dtTau ), 
    m_dtTauByTwo( microseconds( dtTau.total_microseconds() / 2 ) ),
//    m_dtTauPrime( microseconds( dtTauPrime.total_microseconds() / 2 ) ), 
    m_dtTauPrime( dtTauPrime ),
    m_p( p ), 
    m_n( n ),
    m_tsDif( series, dtTauPrime ), 
    m_tsNorm( m_tsDif, m_dtTauByTwo, n, p )
{
}

TSVolatility::~TSVolatility(void) {
}

void TSVolatility::HandleUpdate( const Price& price ) {
  Append( price );
}

} // namespace hf
} // namespace tf
} // namespace ou
