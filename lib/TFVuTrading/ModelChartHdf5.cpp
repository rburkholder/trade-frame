/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "stdafx.h"

#include "ModelChartHdf5.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

ModelChartHdf5::ModelChartHdf5( void ) {
  m_ceQuoteUpper.SetName( "Ask" );
  m_ceQuoteLower.SetName( "Bid" );
  m_ceQuoteSpread.SetName( "Spread" );
  m_ceTrade.SetName( "Ticks" );
  m_ceCallIV.SetName( "Call IV" );
  m_cePutIV.SetName( "Put IV" );
  m_ceImpVol.SetName( "Implied Volatility" );
  m_ceDelta.SetName( "Delta" );
  m_ceGamma.SetName( "Gamma" );
  m_ceTheta.SetName( "Theta" );
  m_ceVega.SetName( "Vega" );
  m_ceRho.SetName( "Rho" );
  m_ceVolume.SetName( "Volume" );
  m_ceVolumeUpper.SetName( "Long" );
  m_ceVolumeLower.SetName( "Short" );
}

ModelChartHdf5::~ModelChartHdf5(void) {
}

void ModelChartHdf5::AddChartEntries( ou::ChartDataView* pChartDataView, const ou::tf::Bars& bars ) {
    m_ceBars.Clear();
    m_ceVolume.Clear();
    for ( ou::tf::Bars::const_iterator iter = bars.begin(); bars.end() != iter; ++iter ) {
      m_ceBars.AppendBar( *iter );
      m_ceVolume.Append( iter->DateTime(), iter->Volume() );
    }
    m_ceVolume.SetColour( ou::Colour::Black );
    pChartDataView->Add( 0, &m_ceBars );
    pChartDataView->Add( 1, &m_ceVolume );
}

void ModelChartHdf5::AddChartEntries( ou::ChartDataView* pChartDataView, const ou::tf::Quotes& quotes ) {
    m_ceQuoteUpper.Clear();
    m_ceVolumeUpper.Clear();
    m_ceQuoteLower.Clear();
    m_ceVolumeLower.Clear();
    m_ceQuoteSpread.Clear();
    for ( ou::tf::Quotes::const_iterator iter = quotes.begin(); quotes.end() != iter; ++iter ) {
      m_ceQuoteUpper.Append( iter->DateTime(), iter->Ask() );
      m_ceVolumeUpper.Append( iter->DateTime(), iter->AskSize() );
      m_ceQuoteLower.Append( iter->DateTime(), iter->Bid() );
      m_ceVolumeLower.Append( iter->DateTime(), - (int) iter->BidSize() );
      m_ceQuoteSpread.Append( iter->DateTime(), iter->Ask() - iter->Bid() );
    }
    m_ceQuoteUpper.SetColour( ou::Colour::Red );
    m_ceVolumeUpper.SetColour( ou::Colour::Red );
    m_ceQuoteLower.SetColour( ou::Colour::Blue );
    m_ceVolumeLower.SetColour( ou::Colour::Blue );
    m_ceQuoteSpread.SetColour( ou::Colour::Black );
    pChartDataView->Add( 0, &m_ceQuoteUpper );
    pChartDataView->Add( 1, &m_ceVolumeUpper );
    pChartDataView->Add( 0, &m_ceQuoteLower );
    pChartDataView->Add( 1, &m_ceVolumeLower );
    pChartDataView->Add( 2, &m_ceQuoteSpread );
}

void ModelChartHdf5::AddChartEntries( ou::ChartDataView* pChartDataView, const ou::tf::Trades& trades ) {
    m_ceTrade.Clear();
    m_ceVolume.Clear();
    for ( ou::tf::Trades::const_iterator iter = trades.begin(); trades.end() != iter; ++iter ) {
      m_ceTrade.Append( iter->DateTime(), iter->Price() );
      m_ceVolume.Append( iter->DateTime(), iter->Volume() );
    }
    m_ceTrade.SetColour( ou::Colour::Green );
    m_ceVolume.SetColour( ou::Colour::Black );
    pChartDataView->Add( 0, &m_ceTrade );
    pChartDataView->Add( 1, &m_ceVolume );
}

void ModelChartHdf5::AddChartEntries( ou::ChartDataView* pChartDataView, const ou::tf::PriceIVs& ivs ) {
    m_ceTrade.Clear();
    m_ceCallIV.Clear();
    m_cePutIV.Clear();
    for ( ou::tf::PriceIVs::const_iterator iter = ivs.begin(); ivs.end() != iter; ++iter ) {
      m_ceTrade.Append( iter->DateTime(), iter->Value() );
      m_ceCallIV.Append( iter->DateTime(), iter->IVCall() );
      m_cePutIV.Append( iter->DateTime(), iter->IVPut() );
    }
    m_ceTrade.SetColour( ou::Colour::Green );
    m_cePutIV.SetColour( ou::Colour::Red );
    m_ceCallIV.SetColour( ou::Colour::Blue );
    pChartDataView->Add( 0, &m_ceTrade );
    pChartDataView->Add( 1, &m_ceCallIV );
    pChartDataView->Add( 1, &m_cePutIV );
}

void ModelChartHdf5::AddChartEntries( ou::ChartDataView* pChartDataView, const ou::tf::Greeks& greeks ) {
    m_ceImpVol.Clear();
    m_ceDelta.Clear();
    m_ceGamma.Clear();
    m_ceTheta.Clear();
    m_ceVega.Clear();
    m_ceRho.Clear();
    for ( ou::tf::Greeks::const_iterator iter = greeks.begin(); greeks.end() != iter; ++iter ) {
      m_ceImpVol.Append( iter->DateTime(), iter->ImpliedVolatility() );
      m_ceDelta.Append( iter->DateTime(), iter->Delta() );
      m_ceGamma.Append( iter->DateTime(), iter->Gamma() );
      m_ceTheta.Append( iter->DateTime(), iter->Theta() );
      m_ceVega.Append( iter->DateTime(), iter->Vega() );
      m_ceRho.Append( iter->DateTime(), iter->Rho() );
    }
    m_ceImpVol.SetColour( ou::Colour::Black );
    m_ceDelta.SetColour( ou::Colour::Black );
    m_ceGamma.SetColour( ou::Colour::Black );
    m_ceTheta.SetColour( ou::Colour::Black );
    m_ceVega.SetColour( ou::Colour::Black );
    m_ceRho.SetColour( ou::Colour::Black );
    pChartDataView->Add( 0, &m_ceImpVol );
    pChartDataView->Add( 1, &m_ceDelta );
    pChartDataView->Add( 2, &m_ceGamma );
    pChartDataView->Add( 3, &m_ceTheta );
    pChartDataView->Add( 4, &m_ceVega );
    pChartDataView->Add( 5, &m_ceRho );
}

} // namespace tf
} // namespace ou
