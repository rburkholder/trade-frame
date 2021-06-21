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
  pWatch_t pWatch,
  pPortfolio_t pPortfolioAggregate
)
:
  m_pWatch( pWatch ),
  m_GexCalc( pWatch ),
  m_pPortfolioAggregate( pPortfolioAggregate )
{
  assert( pWatch );
  assert( pPortfolioAggregate );
  //m_GexCalc.LoadChains( m_fOptionNamesByUnderlying );  // is currently unpopulated
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

void Underlying::SaveSeries( const std::string& sPrefix ) {
  m_pWatch->SaveSeries( sPrefix );
}

void Underlying::ReadDailyBars( const std::string& sDailyBarPath ) {
  m_BollingerTransitions.ReadDailyBars( sDailyBarPath, m_cePivots );
}

void Underlying::SetChartDataView( pChartDataView_t pChartDataView ) {
  m_pChartDataView = pChartDataView;
}