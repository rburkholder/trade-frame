/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    Underlying.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on 2021/06/19 19:41
 */

#include "Underlying.h"

Underlying::Underlying(
  pWatch_t pWatchUnderlying_,
  const std::string& sDailyBarPath
)
: pWatchUnderlying( pWatchUnderlying_ ),
  GexCalc( pWatchUnderlying_ ),
  m_sDailyBarPath( sDailyBarPath )
{
  assert( pWatchUnderlying_ );
  m_pChartDataView = std::make_shared<ou::ChartDataView>();
  m_BollingerTransitions.ReadDailyBars( sDailyBarPath, m_cePivots );
}


// TODO: don't really need this anymore, placeholder for real code
void Underlying::BuildUnderlyingChains( gex_t& gex ) {
  //gex.LoadChains( m_fOptionNamesByUnderlying );  // is currently unpopulated
}

void Underlying::SetPivots( double dblR2, double dblR1, double dblPV, double dblS1, double dblS2 ) {
  // TFIndicators/Pivots.h has R3, S3 plus colour assignments
  //m_pivotCrossing.Set( dblR2, dblR1, dblPV, dblS1, dblS2 ); // belongs back in ManageStrategy
  m_cePivots.AddMark( dblR2, ou::Colour::Red, "R2" );
  m_cePivots.AddMark( dblR1, ou::Colour::Red, "R1" );
  m_cePivots.AddMark( dblPV, ou::Colour::Green, "PV" );
  m_cePivots.AddMark( dblS1, ou::Colour::Blue, "S1" );
  m_cePivots.AddMark( dblS2, ou::Colour::Blue, "S2" );
}

