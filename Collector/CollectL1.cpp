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
 * File:    CollectL1.cpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: January 5, 2025 17:11:37
 */

//#include <boost/log/trivial.hpp>

#include "CollectL1.hpp"

namespace collect {

L1::L1( const std::string& sFilePath, const std::string& sDataPathPrefix, pWatch_t pWatch )
{

  // TODO: watch built elsewhere, needs to be restartable for a new day?
  //       or, delete and rebuild for a new day?
  //             this, so that can handle when new front month started

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  pInstrument_t pInstrument( pWatch->GetInstrument() );

  m_pWatch = pWatch;
  m_pWatch->RecordSeries( false ); // record manually in Write()

  {
    const std::string sFullDataPath( sDataPathPrefix + ou::tf::Quotes::Directory() + pInstrument->GetInstrumentName() );
    m_pfwQuotes = std::make_unique<fwQuotes_t>(
      sFilePath, sFullDataPath,
      [this]( ou::tf::HDF5Attributes& attr ){
        SetAttributes( attr, m_pWatch );
      } );
    m_pWatch->OnQuote.Add( MakeDelegate( this, &L1::HandleWatchQuote ) );
  }

  {
    const std::string sFullDataPath( sDataPathPrefix + ou::tf::Trades::Directory() + pInstrument->GetInstrumentName() );
    m_pfwTrades = std::make_unique<fwTrades_t>(
      sFilePath, sFullDataPath,
      [this]( ou::tf::HDF5Attributes& attr ){
         SetAttributes( attr, m_pWatch );
      } );
    m_pWatch->OnTrade.Add( MakeDelegate( this, &L1::HandleWatchTrade ) );
  }

  m_pWatch->StartWatch();
}

L1::~L1() {

  assert( m_pWatch );
  m_pWatch->StopWatch();

  m_pfwQuotes->Write();
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &L1::HandleWatchTrade ) );
  m_pfwQuotes.reset();

  m_pfwTrades->Write();
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &L1::HandleWatchQuote ) );
  m_pfwTrades.reset();

  m_pWatch.reset();
  //m_pWatch->SaveSeries( m_sPathName );

  //BOOST_LOG_TRIVIAL(info) << "  ... Done ";

}

void L1::HandleWatchTrade( const ou::tf::Trade& trade ) {
  m_pfwTrades->Append( trade );
}

void L1::HandleWatchQuote( const ou::tf::Quote& quote ) {
  m_pfwQuotes->Append( quote );
}

void L1::Write() {
  m_pfwQuotes->Write();
  m_pfwTrades->Write();
}

} // namespace collect
