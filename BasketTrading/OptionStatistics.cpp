/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    OptionStatistics.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created: April 24, 2022 16:07
 */

#include <TFVuTrading/TreeItem.hpp>

#include "OUCharting/ChartDataView.h"
#include "OptionStatistics.hpp"

// TODO: re-use the statistics in Leg?
// TODO: remove spkes in charts

OptionStatistics::OptionStatistics( pOption_t pOption )
: m_ptiSelf( nullptr )
{

  m_pOption = pOption;

  m_pdvChart = ou::ChartDataView::Factory();

  m_pdvChart->SetNames( pOption->GetInstrumentName(), "Option" );

  m_pdvChart->Add( ChartSlot::Price, &m_ceTrade );
  m_pdvChart->Add( ChartSlot::Price, &m_ceAsk );
  m_pdvChart->Add( ChartSlot::Price, &m_ceBid );

  m_pdvChart->Add( ChartSlot::Volume, &m_ceAskVolume );
  m_pdvChart->Add( ChartSlot::Volume, &m_ceBidVolume );
  //m_pdvChart->Add( ChartSlot::Volume, &m_ceVolume );

  m_pdvChart->Add( ChartSlot::Spread, &m_ceSpread );

  m_pdvChart->Add( ChartSlot::PL, &m_cePLTotal );

  m_pdvChart->Add( ChartSlot::IV, &m_ceImpliedVolatility );
  m_pdvChart->Add( ChartSlot::Delta, &m_ceDelta );
  m_pdvChart->Add( ChartSlot::Gamma, &m_ceGamma );
  m_pdvChart->Add( ChartSlot::Theta, &m_ceTheta );
  m_pdvChart->Add( ChartSlot::Rho, &m_ceRho );
  m_pdvChart->Add( ChartSlot::Vega, &m_ceVega );

  m_ceAsk.SetColour( ou::Colour::Red );
  m_ceBid.SetColour( ou::Colour::Blue );

  m_ceAskVolume.SetColour( ou::Colour::Red );
  m_ceBidVolume.SetColour( ou::Colour::Blue );

  m_ceTrade.SetColour( ou::Colour::DarkGreen );

  m_ceSpread.SetColour( ou::Colour::Black );

  m_ceAsk.SetName( "Ask" );
  m_ceTrade.SetName( "Tick" );
  m_ceBid.SetName( "Bid" );

  m_ceSpread.SetName( "Spread" );

  m_ceVolume.SetName( "Volume" );

  m_cePLTotal.SetName( "P/L Position" );

  m_ceDelta.SetName( "Delta" );
  m_ceGamma.SetName( "Gamma" );
  m_ceTheta.SetName( "Theta" );
  m_ceImpliedVolatility.SetName( "IV" );
  m_ceRho.SetName( "Rho" );
  m_ceVega.SetName( "Vega" );

  m_pOption->OnQuote.Add( MakeDelegate( this, &OptionStatistics::HandleQuote ) );
  m_pOption->OnTrade.Add( MakeDelegate( this, &OptionStatistics::HandleTrade ) );
  m_pOption->OnGreek.Add( MakeDelegate( this, &OptionStatistics::HandleGreek ) );

}

OptionStatistics::~OptionStatistics() {
  m_pOption->OnQuote.Remove( MakeDelegate( this, &OptionStatistics::HandleQuote ) );
  m_pOption->OnTrade.Remove( MakeDelegate( this, &OptionStatistics::HandleTrade ) );
  m_pOption->OnGreek.Remove( MakeDelegate( this, &OptionStatistics::HandleGreek ) );
  m_ptiSelf = nullptr; // need to be able to dynamically remove tree item?
  m_pdvChart.reset();
  m_pPosition.reset();
  m_pOption.reset();
}

void OptionStatistics::HandleQuote( const ou::tf::Quote& quote ) {
  if ( 0.0 < quote.Bid() ) {
    m_ceAsk.Append( quote.DateTime(), quote.Ask() );
    m_ceBid.Append( quote.DateTime(), quote.Bid() );
    m_ceAskVolume.Append( quote.DateTime(), +quote.AskSize() );
    m_ceBidVolume.Append( quote.DateTime(), -quote.BidSize() );
    m_ceSpread.Append( quote.DateTime(), quote.Ask() - quote.Bid() );
  }

  if ( m_pPosition ) {
    double dblUnRealized;
    double dblRealized;
    double dblCommissionsPaid;
    double dblTotal;
    m_pPosition->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );
    m_cePLTotal.Append( quote.DateTime(), dblTotal );
  }
}

void OptionStatistics::HandleTrade( const ou::tf::Trade& trade ) {
  m_ceTrade.Append( trade.DateTime(), trade.Price() );
  m_ceVolume.Append( trade.DateTime(), trade.Volume() );
}

void OptionStatistics::HandleGreek( const ou::tf::Greek& greek ) {
  m_ceImpliedVolatility.Append( greek.DateTime(), greek.ImpliedVolatility() );
  m_ceDelta.Append( greek.DateTime(), greek.Delta() );
  m_ceGamma.Append( greek.DateTime(), greek.Gamma() );
  m_ceTheta.Append( greek.DateTime(), greek.Theta() );
  m_ceRho.Append( greek.DateTime(), greek.Rho() );
  m_ceVega.Append( greek.DateTime(), greek.Vega() );
}
