/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    CollectATM.cpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: January 19, 2025 14:06:45
 */

#include <boost/log/trivial.hpp>

#include <TFOptions/Chains.h>

#include "CollectATM.hpp"

namespace collect {

ATM::ATM(
  const std::string& sPathPrefix, pWatch_t pWatchUnderlying,
  fBuildOption_t&& fBuildOption, fGatherOptions_t&& fGatherOptions,
  fEngine_t&& fEngineOptionStart, fEngine_t&& fEngineOptionStop,
  boost::gregorian::date dateStop // use for expiry calculation
)
{

  // TODO: watch built elsewhere, needs to be restartable for a new day?
  //       or, delete and rebuild for a new day?
  //             this, so that can handle when new front month started

  assert( fBuildOption );
  m_fBuildOption = std::move( fBuildOption );

  assert( pWatchUnderlying );
  m_pWatchUnderlying = std::move( pWatchUnderlying );
  m_pWatchUnderlying->RecordSeries( false ); // record manually in Write()

  {
    const std::string sFullPath( sPathPrefix + ou::tf::Quotes::Directory() + m_pWatchUnderlying->GetInstrumentName() );
    m_pfwATM = std::make_unique<fwATM_t>(
      sFullPath,
      [this]( ou::tf::HDF5Attributes& attr ){
        SetAttributes( attr, m_pWatchUnderlying );
      } );
  }

  assert( fEngineOptionStart );
  m_fEngineOptionStart = std::move( fEngineOptionStart );

  assert( fEngineOptionStop );
  m_fEngineOptionStop = std::move( fEngineOptionStop );

  m_pWatchUnderlying->OnTrade.Add( MakeDelegate( this, &ATM::HandleWatchUnderlyingTrade ) );
  m_pWatchUnderlying->StartWatch(); // maybe use ticks or quotes to trigger recording atm?

  assert( fGatherOptions );
  // TODO: convert the lambda to bind so can pass a reference and copies are not made in QueryChains
  fGatherOptions( // when is it done?
    m_pWatchUnderlying->GetInstrument(),
    [this, dateStop]( std::size_t zero, pInstrument_t pInstrumentOption ){ // see ou::tf::option::PopulateMap for framework
      // find existing expiry, or create new one
      mapChains_t::iterator iterChains = ou::tf::option::GetChain( m_mapChains, pInstrumentOption );
      assert( m_mapChains.end() != iterChains );

      // update put/call@strike with option
      chain_t& chain( iterChains->second );
      Instance* pEntry
        = ou::tf::option::UpdateOption<chain_t,Instance>( chain, pInstrumentOption );
      pEntry->pInstrument = pInstrumentOption; // put / call as appropriate

      if ( 0 == zero ) {
        BOOST_LOG_TRIVIAL(info) << "last option entry found";
        // select, at minimum, one day beyond to ensure non-expiry
        mapChains_t::const_iterator iter = ou::tf::option::SelectChain( m_mapChains, dateStop, boost::gregorian::days( 1 ) );
        assert( m_mapChains.end() != iter );
        m_pTrackATM = iter->second.Factory_TrackATM(
          []( chain_t::strike_t& ){ // fWatch_t&& fWatchOn

          },
          []( chain_t::strike_t& ){ // fWatch_t&& fWatchOff

          },
          []( const ou::tf::PriceIV& ){ // fIvATM_t&& fIvATM

          }
        );
        // TODO: clean up strikes to ensure complete call/put pairs
        // TODO: start processing options
        // TODO: start up ATM watch
        // TODO: connect up with option construction
      }
    } );
}

ATM::~ATM() {

  m_pWatchUnderlying->StopWatch();
  m_pWatchUnderlying->OnTrade.Remove( MakeDelegate( this, &ATM::HandleWatchUnderlyingTrade ) );

  m_pfwATM->Write();
  m_pfwATM.reset();

  m_fEngineOptionStart = nullptr;
  m_fEngineOptionStop = nullptr;

  m_fBuildOption = nullptr;

  m_pWatchUnderlying.reset();

  m_mapChains.clear();

}

void ATM::HandleWatchUnderlyingTrade( const ou::tf::Trade& ) { // TODO: use Quote instead
  // TODO: update the Chain ATM with new price
}

void ATM::HandleWatchGreeksPut( const ou::tf::Greek& greek ) {
  //m_pfwATM->Append( greek );
}

void ATM::HandleWatchGreeksCall( const ou::tf::Greek& greek ) {
  //m_pfwATM->Append( greek );
}

void ATM::Write() {
  m_pfwATM->Write();
}

} // namespace collect
