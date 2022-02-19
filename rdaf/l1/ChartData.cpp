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
 * File:    ChartData.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l1
 * Created: February 6, 2022 16:15
 * retrofit back to LiveChart
 */

#include <memory>

#include <boost/date_time/posix_time/conversion.hpp>

#include <rdaf/TRint.h>
#include <rdaf/TH3.h>
#include <rdaf/TF1.h>
#include <rdaf/TCanvas.h>
#include <rdaf/TROOT.h>

#include <TFTrading/Instrument.h>

#include "Config.h"
#include "ChartData.h"

ChartData::ChartData( pProvider_t pProvider, const std::string& sIQFeedSymbol, const config::Options& options )
: m_options( options ), ou::ChartDVBasics(), m_bWatching( false )
{
  GetChartDataView()->SetNames( "ChartData", sIQFeedSymbol );

  ou::tf::Instrument::pInstrument_t pInstrument;
  pInstrument = std::make_shared<ou::tf::Instrument>( sIQFeedSymbol ); // simple for an iqfeed watch
  pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, sIQFeedSymbol );
  m_pWatch = std::make_shared<ou::tf::Watch>( pInstrument, pProvider ); // will need to be iqfeed provider, check?

  StartRdaf();
}

ChartData::~ChartData(void) {
  StopWatch();
  m_prdafApp->SetReturnFromRun( true );
  m_threadRdaf.join();

}

void ChartData::ThreadRdaf( ChartData* p ) {

  ChartData* self = reinterpret_cast<ChartData*>( p );

  const config::Options& options( self->m_options );

  self->m_pHistDelta = std::make_shared<TH3D>(
    "h1", ( options.sSymbol + "Delta" ).c_str(),
    options.nTimeBins, options.dblTimeLower, options.dblTimeUpper,
    options.nPriceBins, options.dblPriceLower, options.dblPriceUpper,
    options.nVolumeSideBins, options.dblVolumeSideLower, options.dblVolumeSideUpper
  );
  if ( !self->m_pHistDelta ) {
    std::cout << "problems delta" << std::endl;
  }

  self->m_pHistVolume = std::make_shared<TH3D>(
    "h2", ( options.sSymbol + "Volume" ).c_str(),
    options.nTimeBins, options.dblTimeLower, options.dblTimeUpper,
    options.nPriceBins, options.dblPriceLower, options.dblPriceUpper,
    options.nVolumeTotalBins, options.dblVolumeTotalLower, options.dblVolumeTotalUpper
  );
  if ( !self->m_pHistVolume ) {
    std::cout << "problems history" << std::endl;
  }

  //TCanvas* c = new TCanvas("c", "Something", 0, 0, 800, 600);
  //TF1 *f1 = new TF1("f1","sin(x)", -5, 5);
  //f1->SetLineColor(kBlue+1);
  //f1->SetTitle("My graph;x; sin(x)");
  //f1->Draw();
  //c->Modified(); c->Update();
  self->m_prdafApp->Run();
}

void ChartData::StartRdaf() {

  int argc {};
  char** argv = nullptr;

  m_prdafApp = std::make_unique<TRint>( "rdaf_l1", &argc, argv );
  ROOT::EnableThreadSafety();
  ROOT::EnableImplicitMT();

  m_threadRdaf = std::move( std::thread(
    [this](){
      ThreadRdaf( this );
    }));
}

void ChartData::StartWatch() {
  if ( !m_bWatching ) {
    m_bWatching = true;
    m_pWatch->OnQuote.Add( MakeDelegate( this, &ChartData::HandleQuote ) );
    m_pWatch->OnTrade.Add( MakeDelegate( this, &ChartData::HandleTrade ) );
    m_pWatch->StartWatch();
  }
}

void ChartData::StopWatch() {
  if ( m_bWatching ) {
    m_bWatching = false;
    m_pWatch->StopWatch();
    m_pWatch->OnQuote.Remove( MakeDelegate( this, &ChartData::HandleQuote ) );
    m_pWatch->OnTrade.Remove( MakeDelegate( this, &ChartData::HandleTrade ) );
  }
}

void ChartData::HandleQuote( const ou::tf::Quote& quote ) {
  ou::ChartDVBasics::HandleQuote( quote );
  m_quote = quote;
}

void ChartData::HandleTrade( const ou::tf::Trade& trade ) {

  ou::ChartDVBasics::HandleTrade( trade );

  double mid = m_quote.Midpoint();
  std::time_t nTime = boost::posix_time::to_time_t( trade.DateTime() );
  double dblTime( nTime );
  dblTime = dblTime / 1000.0;

  std::cout << "values: " << dblTime << "," << trade.Price() << "," << trade.Volume() << std::endl;

  m_pHistDelta ->Fill( dblTime, trade.Price(), trade.Price() >= mid ? trade.Volume() : -trade.Volume() );
  m_pHistVolume->Fill( dblTime, trade.Price(), trade.Volume() );
}