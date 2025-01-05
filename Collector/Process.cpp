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
 * File:    Process.cpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: October 20, 2022 21:07:40
 */

// start watch on l1
// write out at regular intervals

#include <boost/log/trivial.hpp>

#include <TFTrading/ComposeInstrument.hpp>

#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include "Process.hpp"

namespace {
  static const std::string sSaveValuesRoot( "/app/collector" );
}

Process::Process(
  const config::Choices& choices
, const std::string& sTimeStamp
)
: m_choices( choices )
, m_sPathName( sSaveValuesRoot + "/" + sTimeStamp )
{
  StartIQFeed();
}

Process::~Process() {

  if ( m_pWatch ) {
    StopWatch();
  }

  m_pComposeInstrumentIQFeed.reset();

  m_piqfeed->Disconnect();
  m_piqfeed.reset();
}

// need control c handler to terminate, as this is an ongoing process
void Process::StartIQFeed() {
  m_piqfeed = ou::tf::iqfeed::Provider::Factory();
  m_piqfeed->OnConnected.Add( MakeDelegate( this, &Process::HandleIQFeedConnected ) );
  m_piqfeed->Connect();
}

void Process::HandleIQFeedConnected( int ) {
  ConstructUnderlying();
}

void Process::ConstructUnderlying() {

  m_pComposeInstrumentIQFeed = std::make_unique<ou::tf::ComposeInstrument>(
    m_piqfeed,
    [this](){ // callback once started
      // TODO: will need to expand on this, but keep the same for now
      assert( 1 == m_choices.m_vSymbolName.size() ); // remove with multi-instrument methodology
      const std::string& sName( *m_choices.m_vSymbolName.begin() );
      m_pComposeInstrumentIQFeed->Compose(
        sName,
        [this]( pInstrument_t pInstrument, bool bConstructed ){
          //m_pInstrument = std::move( pInstrument );
          StartWatch( pInstrument );
        }
      );
    }
    );
  assert( m_pComposeInstrumentIQFeed );

}

void Process::SetAttributes( ou::tf::HDF5Attributes& attr ) {
  pInstrument_t pInstrument( m_pWatch->GetInstrument() );
  const ou::tf::InstrumentType::EInstrumentType type( pInstrument->GetInstrumentType() );
  attr.SetInstrumentType( type );
  switch ( type ) {
    case ou::tf::InstrumentType::Future: {
        const ou::tf::HDF5Attributes::structFuture attributes(
          pInstrument->GetExpiryYear(),
          pInstrument->GetExpiryMonth(),
          pInstrument->GetExpiryDay()
        );
        attr.SetFutureAttributes( attributes );
      }
      break;
    case ou::tf::InstrumentType::Option: {
        const ou::tf::HDF5Attributes::structOption attributes(
          pInstrument->GetStrike(),
          pInstrument->GetExpiryYear(),
          pInstrument->GetExpiryMonth(),
          pInstrument->GetExpiryDay(),
          pInstrument->GetOptionSide()
        );
        attr.SetOptionAttributes( attributes );
      }
      break;
  }
  attr.SetProviderType( m_pWatch->GetProvider()->ID() );
  attr.SetMultiplier( pInstrument->GetMultiplier() );
  attr.SetSignificantDigits( pInstrument->GetSignificantDigits() );
}

void Process::StartWatch( pInstrument_t pInstrument ) {

  // TODO: watch built elsewhere, needs to be restartable for a new day?
  //       or, delete and rebuild for a new day?
  //             this, so that can handle when new front month started

  const std::string& sSymbolName( pInstrument->GetInstrumentName() );
  const std::string& sIQFeedSymbolName( pInstrument->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );

  BOOST_LOG_TRIVIAL(info) << "symbol: " // should be able to identify type once composed
    << sSymbolName // from config file?
    << ", " << sIQFeedSymbolName  // resolved name
    ;

  m_pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_piqfeed );
  m_pWatch->RecordSeries( false ); // record manually in Write()

  {
    const std::string sPathName(
      m_sPathName + ou::tf::Quotes::Directory()
      + pInstrument->GetInstrumentName()
      );
    m_pfwQuotes = std::make_unique<fwQuotes_t>(
      sPathName,
      [this]( ou::tf::HDF5Attributes& attr ){
        SetAttributes( attr );
      } );
    m_pWatch->OnQuote.Add( MakeDelegate( this, &Process::HandleWatchQuote ) );
  }

  {
    const std::string sPathName(
      m_sPathName + ou::tf::Trades::Directory()
      + pInstrument->GetInstrumentName()
      );
    m_pfwTrades = std::make_unique<fwTrades_t>(
      sPathName,
      [this]( ou::tf::HDF5Attributes& attr ){
         SetAttributes( attr );
      } );
    m_pWatch->OnTrade.Add( MakeDelegate( this, &Process::HandleWatchTrade ) );
  }

  m_pWatch->StartWatch();
}

void Process::HandleWatchTrade( const ou::tf::Trade& trade ) {
  m_pfwTrades->Append( trade );
}

void Process::HandleWatchQuote( const ou::tf::Quote& quote ) {
  m_pfwQuotes->Append( quote );
}


void Process::StopWatch() {

  assert( m_pWatch );
  m_pWatch->StopWatch();

  m_pfwQuotes->Write();
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &Process::HandleWatchTrade ) );
  m_pfwQuotes.reset();

  m_pfwTrades->Write();
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &Process::HandleWatchQuote ) );
  m_pfwTrades.reset();

  m_pWatch.reset();
  //m_pWatch->SaveSeries( m_sPathName );

  BOOST_LOG_TRIVIAL(info) << "  ... Done ";

}

void Process::Write() {
  m_pfwQuotes->Write();
  m_pfwTrades->Write();
}

void Process::Finish() {

  if ( m_pWatch ) {
    StopWatch();
  }

}
