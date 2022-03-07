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

#include <rdaf/TRint.h>
#include <rdaf/TROOT.h>
#include <rdaf/TFile.h>
#include <rdaf/TTree.h>
//#include <rdaf/TCanvas.h>

#include <TFTrading/Instrument.h>

#include "Config.h"
#include "ChartData.h"

ChartData::ChartData(
    const std::string& sFilePrefix,
    const std::string& sIQFeedSymbol,
    const config::Options& options,
    pProvider_t pProvider
)
: m_options( options ), ou::ChartDVBasics(), m_bWatching( false )
{
  GetChartDataView()->SetNames( "ChartData", sIQFeedSymbol );

  ou::tf::Instrument::pInstrument_t pInstrument;
  pInstrument = std::make_shared<ou::tf::Instrument>( sIQFeedSymbol ); // simple for an iqfeed watch
  pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, sIQFeedSymbol );
  m_pWatch = std::make_shared<ou::tf::Watch>( pInstrument, pProvider ); // will need to be iqfeed provider, check?

  StartRdaf( sFilePrefix );
}

ChartData::~ChartData(void) {
  StopWatch();
  m_pFile->Flush();
  m_pFile->Close();
  m_prdafApp->SetReturnFromRun( true );
  m_threadRdaf.join();

}

void ChartData::ThreadRdaf( ChartData* p, const std::string& sFilePrefix ) {

  ChartData* self = reinterpret_cast<ChartData*>( p );

  const config::Options& options( self->m_options );

  if ( false ) { // diagnostics
    //double dblDateTimeUpper;
    //double dblDateTimeLower;

    //std::time_t nTime;
    //nTime = boost::posix_time::to_time_t( options.dtTimeUpper );
    //dblDateTimeUpper = (double) nTime / 1000.0;
    //nTime = boost::posix_time::to_time_t( options.dtTimeLower );
    //dblDateTimeLower = (double) nTime / 1000.0;

    //std::cout << "date range: " << dblDateTimeLower << " ... " << dblDateTimeUpper << std::endl;
  }

  self->m_pFile = std::make_unique<TFile>(
    ( sFilePrefix + ".root" ).c_str(), "RECREATE", "tradeframe rdaf/l1 based data, quotes & trades"
  );

  self->m_pTreeQuote = std::make_shared<TTree>(
    "quotes", ( self->m_pWatch->GetInstrumentName() + " quotes" ).c_str()
  );
  self->m_pTreeQuote->Branch( "quote", &self->m_pTreeQuote, "time/D:ask/D:askvol/l:bid/D:bidvol/l" );
  if ( !self->m_pTreeQuote ) {
    std::cout << "problems m_pTreeQuote" << std::endl;
  }

  self->m_pTreeTrade = std::make_shared<TTree>(
    "trades", ( self->m_pWatch->GetInstrumentName() + " trades" ).c_str()
  );
  self->m_pTreeTrade->Branch( "trade", &self->m_pTreeTrade, "time/D:price/D:vol/l:direction/L" );
  if ( !self->m_pTreeTrade ) {
    std::cout << "problems m_pTreeTrade" << std::endl;
  }

  //TCanvas* c = new TCanvas("c", "Something", 0, 0, 800, 600);
  //TF1 *f1 = new TF1("f1","sin(x)", -5, 5);
  //f1->SetLineColor(kBlue+1);
  //f1->SetTitle("My graph;x; sin(x)");
  //f1->Draw();
  //c->Modified(); c->Update();
  self->m_prdafApp->Run();
}

void ChartData::StartRdaf( const std::string& sFileName ) {

  int argc {};
  char** argv = nullptr;

  m_prdafApp = std::make_unique<TRint>( "rdaf_l1", &argc, argv );
  ROOT::EnableThreadSafety();
  ROOT::EnableImplicitMT();

  m_threadRdaf = std::move( std::thread( ThreadRdaf, this, sFileName ) );
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

  m_quote = quote;
  ou::ChartDVBasics::HandleQuote( quote );

  std::time_t nTime = boost::posix_time::to_time_t( quote.DateTime() );
  m_treeQuote.time = (double)nTime / 1000.0;
  m_treeQuote.ask = quote.Ask();
  m_treeQuote.askvol = quote.AskSize();
  m_treeQuote.bid = quote.Bid();
  m_treeQuote.bidvol = quote.BidSize();

  m_pTreeQuote->Fill();

}

void ChartData::HandleTrade( const ou::tf::Trade& trade ) {

  ou::ChartDVBasics::HandleTrade( trade );

  const double mid = m_quote.Midpoint();
  const double price = trade.Price();
  const uint64_t volume = trade.Volume();

  std::time_t nTime = boost::posix_time::to_time_t( trade.DateTime() );
  m_treeTrade.time = (double)nTime / 1000.0;
  m_treeTrade.price = price;
  m_treeTrade.vol = volume;
  if ( mid == price ) {
    m_treeTrade.direction = 0;
  }
  else {
    m_treeTrade.direction = ( mid < price ) ? volume : -volume;
  }

  m_pTreeTrade->Fill();

  // std::cout << "values: " << dblTime << "," << trade.Price() << "," << trade.Volume() << std::endl;

}

void ChartData::SaveValues( const std::string& sPrefix ) {
  m_pWatch->SaveSeries( sPrefix );
  m_pTreeQuote->Print();
  m_pTreeTrade->Print();
  m_pFile->Write();
}