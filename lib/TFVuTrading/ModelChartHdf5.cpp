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

#include "ModelChartHdf5.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

namespace {
  static const size_t c_nMaxElements { 25000 };
}

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

// ChartBars

void ModelChartHdf5::DefineChartBars( ou::ChartDataView* pChartDataView ) {
  Clear();
  m_ceVolume.SetColour( ou::Colour::Black );
  pChartDataView->Add( 0, &m_ceBars );
  pChartDataView->Add( 1, &m_ceVolume );
}

void ModelChartHdf5::AddChartEntries( ou::ChartDataView* pChartDataView, const ou::tf::Bars& bars ) {

  DefineChartBars( pChartDataView );

  size_t skip = 1 + ( bars.Size() / c_nMaxElements );

  for ( ou::tf::Bars::const_iterator iter = bars.begin(); iter < bars.end(); iter = iter += skip ) {
    m_ceBars.AppendBar( *iter );
    m_ceVolume.Append( iter->DateTime(), iter->Volume() );
  }
}

// ChartQuotes

void ModelChartHdf5::DefineChartQuotes( ou::ChartDataView* pChartDataView ) {
  Clear();
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

void ModelChartHdf5::AddChartEntries( ou::ChartDataView* pChartDataView, const ou::tf::Quotes& quotes ) {

  DefineChartQuotes( pChartDataView );

  size_t skip = 1 + ( quotes.Size() / c_nMaxElements );

  for ( ou::tf::Quotes::const_iterator iter = quotes.begin(); iter < quotes.end(); iter = iter += skip ) {
    m_ceQuoteUpper.Append( iter->DateTime(), iter->Ask() );
    m_ceVolumeUpper.Append( iter->DateTime(), iter->AskSize() );
    m_ceQuoteLower.Append( iter->DateTime(), iter->Bid() );
    m_ceVolumeLower.Append( iter->DateTime(), - (int) iter->BidSize() );
    m_ceQuoteSpread.Append( iter->DateTime(), iter->Ask() - iter->Bid() );
  }
}

// ChartTrades

void ModelChartHdf5::DefineChartTrades( ou::ChartDataView* pChartDataView ) {
  Clear();
  m_ceTrade.SetColour( ou::Colour::Green );
  m_ceVolume.SetColour( ou::Colour::Black );
  pChartDataView->Add( 0, &m_ceTrade );
  pChartDataView->Add( 1, &m_ceVolume );
}

void ModelChartHdf5::AddChartEntries( ou::ChartDataView* pChartDataView, const ou::tf::Trades& trades ) {
  DefineChartTrades( pChartDataView );

  size_t skip = 1 + ( trades.Size() / c_nMaxElements );

  for ( ou::tf::Trades::const_iterator iter = trades.begin(); iter < trades.end(); iter = iter += skip ) {
    m_ceTrade.Append( iter->DateTime(), iter->Price() );
    m_ceVolume.Append( iter->DateTime(), iter->Volume() );
  }
}

// ChartPriceIVs

void ModelChartHdf5::DefineChartPriceIVs( ou::ChartDataView* pChartDataView ) {
  Clear();
  m_ceTrade.SetColour( ou::Colour::Green );
  m_cePutIV.SetColour( ou::Colour::Red );
  m_ceCallIV.SetColour( ou::Colour::Blue );
  pChartDataView->Add( 0, &m_ceTrade );
  pChartDataView->Add( 1, &m_ceCallIV );
  pChartDataView->Add( 1, &m_cePutIV );
}

void ModelChartHdf5::AddChartEntries( ou::ChartDataView* pChartDataView, const ou::tf::PriceIVs& ivs ) {
  DefineChartPriceIVs( pChartDataView );

  size_t skip = 1 + ( ivs.Size() / c_nMaxElements );

  for ( ou::tf::PriceIVs::const_iterator iter = ivs.begin(); iter < ivs.end(); iter = iter += skip ) {
    m_ceTrade.Append( iter->DateTime(), iter->Value() );
    m_ceCallIV.Append( iter->DateTime(), iter->IVCall() );
    m_cePutIV.Append( iter->DateTime(), iter->IVPut() );
  }
}

// ChartPriceIVExpirys

void ModelChartHdf5::DefineChartPriceIVExpirys( ou::ChartDataView* pChartDataView ) {
  Clear();
  m_ceTrade.SetColour( ou::Colour::Green );
  m_cePutIV.SetColour( ou::Colour::Red );
  m_ceCallIV.SetColour( ou::Colour::Blue );
  pChartDataView->Add( 0, &m_ceTrade );
  pChartDataView->Add( 1, &m_ceCallIV );
  pChartDataView->Add( 1, &m_cePutIV );
}

void ModelChartHdf5::AddChartEntries( ou::ChartDataView* pChartDataView, const ou::tf::PriceIVExpirys& ivs ) {
  DefineChartPriceIVs( pChartDataView );

  size_t skip = 1 + ( ivs.Size() / c_nMaxElements );

  for ( ou::tf::PriceIVExpirys::const_iterator iter = ivs.begin(); iter < ivs.end(); iter = iter += skip ) {
    m_ceTrade.Append( iter->DateTime(), iter->Value() );
    m_ceCallIV.Append( iter->DateTime(), iter->IVCall() );
    m_cePutIV.Append( iter->DateTime(), iter->IVPut() );
  }
}

// ChartGreeks

void ModelChartHdf5::DefineChartGreeks( ou::ChartDataView* pChartDataView ) {
  Clear();
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

void ModelChartHdf5::AddChartEntries( ou::ChartDataView* pChartDataView, const ou::tf::Greeks& greeks ) {
  DefineChartGreeks( pChartDataView );

  size_t skip = 1 + ( greeks.Size() / c_nMaxElements );

  for ( ou::tf::Greeks::const_iterator iter = greeks.begin(); iter < greeks.end(); iter = iter += skip ) {
    m_ceImpVol.Append( iter->DateTime(), iter->ImpliedVolatility() );
    m_ceDelta.Append( iter->DateTime(), iter->Delta() );
    m_ceGamma.Append( iter->DateTime(), iter->Gamma() );
    m_ceTheta.Append( iter->DateTime(), iter->Theta() );
    m_ceVega.Append( iter->DateTime(), iter->Vega() );
    m_ceRho.Append( iter->DateTime(), iter->Rho() );
  }
}

// ChartEquities

void ModelChartHdf5::DefineChartEquities( ou::ChartDataView* pChartDataView ) {

  Clear();

  // Quotes

  m_ceQuoteUpper.SetColour( ou::Colour::Red );
  //m_ceVolumeUpper.SetColour( ou::Colour::Red );
  m_ceQuoteLower.SetColour( ou::Colour::Blue );
  //m_ceVolumeLower.SetColour( ou::Colour::Blue );
  m_ceQuoteSpread.SetColour( ou::Colour::Black );
  pChartDataView->Add( 0, &m_ceQuoteUpper );
  //pChartDataView->Add( 1, &m_ceVolumeUpper );
  pChartDataView->Add( 0, &m_ceQuoteLower );
  //pChartDataView->Add( 1, &m_ceVolumeLower );
  pChartDataView->Add( 2, &m_ceQuoteSpread );

  // Trades

  m_ceTrade.SetColour( ou::Colour::Green );
  m_ceVolume.SetColour( ou::Colour::Black );
  pChartDataView->Add( 0, &m_ceTrade );
  pChartDataView->Add( 1, &m_ceVolume );
}

void ModelChartHdf5::AddChartEntries( ou::ChartDataView* pChartDataView, const Equities& equities ) {

  DefineChartEquities( pChartDataView );

  // Quotes

  {
    size_t skipQuotes = 1 + ( equities.quotes.Size() / c_nMaxElements );

    for ( ou::tf::Quotes::const_iterator iter = equities.quotes.begin();
          iter < equities.quotes.end(); iter = iter += skipQuotes ) {
      m_ceQuoteUpper.Append( iter->DateTime(), iter->Ask() );
      //m_ceVolumeUpper.Append( iter->DateTime(), iter->AskSize() );
      m_ceQuoteLower.Append( iter->DateTime(), iter->Bid() );
      //m_ceVolumeLower.Append( iter->DateTime(), - (int) iter->BidSize() );
      m_ceQuoteSpread.Append( iter->DateTime(), iter->Ask() - iter->Bid() );
    }
  }

  // Trades

  {
    size_t skipTrades = 1 + ( equities.trades.Size() / c_nMaxElements );

    for ( ou::tf::Trades::const_iterator iter = equities.trades.begin();
          iter < equities.trades.end(); iter = iter += skipTrades ) {
      m_ceTrade.Append( iter->DateTime(), iter->Price() );
      m_ceVolume.Append( iter->DateTime(), iter->Volume() );
    }
  }
}

// ChartOptions

void ModelChartHdf5::DefineChartOptions( ou::ChartDataView* pChartDataView ) {

  DefineChartEquities( pChartDataView );

  m_ceImpVol.SetColour( ou::Colour::Black );
  m_ceDelta.SetColour( ou::Colour::Black );
  m_ceGamma.SetColour( ou::Colour::Black );
  m_ceTheta.SetColour( ou::Colour::Black );
  m_ceVega.SetColour( ou::Colour::Black );
  m_ceRho.SetColour( ou::Colour::Black );
  pChartDataView->Add( 3, &m_ceImpVol );
  pChartDataView->Add( 4, &m_ceDelta );
  pChartDataView->Add( 5, &m_ceGamma );
  pChartDataView->Add( 6, &m_ceTheta );
  pChartDataView->Add( 7, &m_ceVega );
  pChartDataView->Add( 8, &m_ceRho );
}

void ModelChartHdf5::AddChartEntries( ou::ChartDataView* pChartDataView, const Options& options ) {

  DefineChartOptions( pChartDataView );

  Equities equities( options.quotes, options.trades );
  AddChartEntries( pChartDataView, equities );

  size_t skip = 1 + ( options.greeks.Size() / c_nMaxElements );

  for ( ou::tf::Greeks::const_iterator iter = options.greeks.begin();
        iter < options.greeks.end(); iter = iter += skip ) {
    m_ceImpVol.Append( iter->DateTime(), iter->ImpliedVolatility() );
    m_ceDelta.Append( iter->DateTime(), iter->Delta() );
    m_ceGamma.Append( iter->DateTime(), iter->Gamma() );
    m_ceTheta.Append( iter->DateTime(), iter->Theta() );
    m_ceVega.Append( iter->DateTime(), iter->Vega() );
    m_ceRho.Append( iter->DateTime(), iter->Rho() );
  }
}

// ChartDepthsByMM

void ModelChartHdf5::DefineChartDepthsByMM( ou::ChartDataView* pChartDataView ) {

  DefineChartEquities( pChartDataView );

  // nothing to chart yet
}

void ModelChartHdf5::AddChartEntries( ou::ChartDataView* pChartDataView, const DepthsByMM& depths ) {

  DefineChartOptions( pChartDataView );

}

// ChartDepthsByOrder

void ModelChartHdf5::DefineChartDepthsByOrder( ou::ChartDataView* pChartDataView ) {

  DefineChartEquities( pChartDataView );

  // nothing to chart yet
}

void ModelChartHdf5::AddChartEntries( ou::ChartDataView* pChartDataView, const DepthsByOrder& depths ) {

  DefineChartOptions( pChartDataView );

}

// Supporting methods

void ModelChartHdf5::Clear() {

  m_ceQuoteUpper.Clear();
  m_ceQuoteLower.Clear();
  m_ceQuoteSpread.Clear();
  m_ceTrade.Clear();
  m_ceCallIV.Clear();
  m_cePutIV.Clear();
  m_ceImpVol.Clear();
  m_ceDelta.Clear();
  m_ceGamma.Clear();
  m_ceTheta.Clear();
  m_ceVega.Clear();
  m_ceRho.Clear();
  m_ceVolume.Clear();
  m_ceVolumeUpper.Clear();
  m_ceVolumeLower.Clear();
}

void ModelChartHdf5::HandleQuote( const ou::tf::Quote& quote ) {

  if ( !quote.IsValid() ) {
    std::cout << "InvalidQuote: " << quote.Ask() << "," << quote.Bid() << std::endl;
    return;
  }

  boost::posix_time::ptime dt( quote.DateTime() );

  m_ceQuoteUpper.Append( dt, quote.Ask() );
  m_ceQuoteLower.Append( dt, quote.Bid() );
  m_ceQuoteSpread.Append( dt, quote.Ask() - quote.Bid() );
  m_ceVolumeUpper.Append( dt, quote.AskSize() );
  m_ceVolumeLower.Append( dt, quote.BidSize() );
}

void ModelChartHdf5::HandleTrade( const ou::tf::Trade& trade ) {

  boost::posix_time::ptime dt( trade.DateTime() );

  m_ceTrade.Append( dt, trade.Price() );
  m_ceVolume.Append( dt, trade.Volume() );
}

void ModelChartHdf5::HandleGreek( const ou::tf::Greek& greek ) {

  boost::posix_time::ptime dt( greek.DateTime() );

  m_ceImpVol.Append( dt, greek.ImpliedVolatility() );
  m_ceDelta.Append( dt, greek.Delta() );
  m_ceGamma.Append( dt, greek.Gamma() );
  m_ceTheta.Append( dt, greek.Theta() );
  m_ceVega.Append( dt, greek.Vega() );
  m_ceRho.Append( dt, greek.Rho() );

}


} // namespace tf
} // namespace ou
