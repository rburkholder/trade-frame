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
  m_pPortfolioAggregate( pPortfolioAggregate ),
  m_bfTrades06Sec( 6 )
{
  assert( pWatch );
  assert( pPortfolioAggregate );

  m_pChartDataView = std::make_shared<ou::ChartDataView>();
  PopulateChartDataView( m_pChartDataView );

  m_cePrice.SetName( "Price" );
  m_ceVolume.SetName( "Volume" );

  m_bfTrades06Sec.SetOnBarComplete( MakeDelegate( this, &Underlying::HandleBarTrades06Sec ) );

  m_pWatch->OnQuote.Add( MakeDelegate( this, &Underlying::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &Underlying::HandleTrade ) );
  m_pWatch->StartWatch();

}

Underlying::~Underlying() {

  m_pWatch->StopWatch();
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &Underlying::HandleQuote ) );
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &Underlying::HandleTrade ) );

}

void Underlying::SetPivots( double dblR2, double dblR1, double dblPV, double dblS1, double dblS2 ) {
  // TFIndicators/Pivots.h has R3, S3 plus colour assignments
  //m_pivotCrossing.Set( dblR2, dblR1, dblPV, dblS1, dblS2 ); // belongs back in ManageStrategy?
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

void Underlying::PopulateChartDataView( pChartDataView_t pChartDataView ) {
  pChartDataView->Add( EChartSlot::Price, &m_cePrice );
  pChartDataView->Add( EChartSlot::Price, &m_cePivots );
  pChartDataView->Add( EChartSlot::Volume, &m_ceVolume );
}

void Underlying::PopulateChains( fGatherOptionDefinitions_t& f ) {
  m_GexCalc.LoadChains( f );  // is currently unpopulated
}

void Underlying::HandleQuote( const ou::tf::Quote& quote ) {
}

void Underlying::HandleTrade( const ou::tf::Trade& trade ) {
  m_bfTrades06Sec.Add( trade );
}

void Underlying::HandleBarTrades06Sec( const ou::tf::Bar& bar ) {

  m_cePrice.AppendBar( bar );
  m_ceVolume.Append( bar );

}

