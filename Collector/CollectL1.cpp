/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    Collect.cpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: January 5, 2025 17:11:37
 */

//#include <boost/log/trivial.hpp>

#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include "CollectL1.hpp"

Collect::Collect( const std::string& sPathPrefix, pWatch_t pWatch )
{

  // TODO: watch built elsewhere, needs to be restartable for a new day?
  //       or, delete and rebuild for a new day?
  //             this, so that can handle when new front month started

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  pInstrument_t pInstrument( pWatch->GetInstrument() );

  m_pWatch = pWatch;
  m_pWatch->RecordSeries( false ); // record manually in Write()

  {
    const std::string sFullPath( sPathPrefix + ou::tf::Quotes::Directory() + pInstrument->GetInstrumentName() );
    m_pfwQuotes = std::make_unique<fwQuotes_t>(
      sFullPath,
      [this]( ou::tf::HDF5Attributes& attr ){
        SetAttributes( attr );
      } );
    m_pWatch->OnQuote.Add( MakeDelegate( this, &Collect::HandleWatchQuote ) );
  }

  {
    const std::string sFullPath( sPathPrefix + ou::tf::Trades::Directory() + pInstrument->GetInstrumentName() );
    m_pfwTrades = std::make_unique<fwTrades_t>(
      sFullPath,
      [this]( ou::tf::HDF5Attributes& attr ){
         SetAttributes( attr );
      } );
    m_pWatch->OnTrade.Add( MakeDelegate( this, &Collect::HandleWatchTrade ) );
  }

  m_pWatch->StartWatch();
}

Collect::~Collect() {

  assert( m_pWatch );
  m_pWatch->StopWatch();

  m_pfwQuotes->Write();
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &Collect::HandleWatchTrade ) );
  m_pfwQuotes.reset();

  m_pfwTrades->Write();
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &Collect::HandleWatchQuote ) );
  m_pfwTrades.reset();

  m_pWatch.reset();
  //m_pWatch->SaveSeries( m_sPathName );

  //BOOST_LOG_TRIVIAL(info) << "  ... Done ";

}

void Collect::HandleWatchTrade( const ou::tf::Trade& trade ) {
  m_pfwTrades->Append( trade );
}

void Collect::HandleWatchQuote( const ou::tf::Quote& quote ) {
  m_pfwQuotes->Append( quote );
}

void Collect::SetAttributes( ou::tf::HDF5Attributes& attr ) {
  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
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

void Collect::Write() {
  m_pfwQuotes->Write();
  m_pfwTrades->Write();
}
