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
  const std::string& sFilePath,
  const std::string& sDataPathPrefix,
  pWatch_t pWatchUnderlying,
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
    const std::string sFullDataPath( sDataPathPrefix + ou::tf::PriceIVs::Directory() + m_pWatchUnderlying->GetInstrumentName() );
    m_pfwATM = std::make_unique<fwATM_t>(
      sFilePath, sFullDataPath,
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

  m_fGatherOptions = std::move( fGatherOptions ); // keep it local & in scope
  assert( m_fGatherOptions );
  // TODO: convert the lambda to bind so can pass a reference and copies are not made in QueryChains
  m_fGatherOptions( // when is it done?
    m_pWatchUnderlying->GetInstrument(),
    [this, dateStop]( std::size_t zero, pInstrument_t pInstrumentOption ){ // see ou::tf::option::PopulateMap for framework
      // find existing expiry, or create new one
      mapChains_t::iterator iterChains = ou::tf::option::GetChain( m_mapChains, pInstrumentOption );
      assert( m_mapChains.end() != iterChains );

      // update put/call@strike with option
      chain_t& chain( iterChains->second );
      Instance* pInstance
        = ou::tf::option::UpdateOption<chain_t,Instance>( chain, pInstrumentOption );
      pInstance->pInstrument = pInstrumentOption; // put / call as appropriate

      if ( 0 == zero ) {
        BOOST_LOG_TRIVIAL(info) << "last option entry found";
        // select, at minimum, one day beyond to ensure non-expiry
        mapChains_t::iterator iter = ou::tf::option::SelectChain( m_mapChains, dateStop, boost::gregorian::days( 1 ) );
        assert( m_mapChains.end() != iter );
        // TODO: clean up strikes to ensure complete call/put pairs

        m_pTrackATM = iter->second.Factory_TrackATM(
          // TODO: track active options & deactiviate on destruction
          [this]( chain_t::strike_t& strike ){ // fWatch_t&& fWatchOn

            assert( strike.call.pInstrument );
            if ( !strike.call.pOption ) {
              strike.call.pOption = m_fBuildOption( strike.call.pInstrument );
            };
            MapAddOption( strike.call.pOption );

            assert( strike.put.pInstrument );
            if ( !strike.put.pOption ) {
              strike.put.pOption = m_fBuildOption( strike.put.pInstrument );
            };
            MapAddOption( strike.put.pOption );
            // TODO: enable watches & recording?
          },
          [this]( chain_t::strike_t& strike  ){ // fWatch_t&& fWatchOff

            assert( strike.call.pOption );
            MapDelOption( strike.call.pOption );

            assert( strike.put.pOption );
            MapDelOption( strike.put.pOption );
            // TODO: disable watches & recording?
          },
          [this]( const ou::tf::PriceIV& iv ){ // fIvATM_t&& fIvATM
            m_pfwATM->Append( iv );
          }
        );
      }
    } );
}

ATM::~ATM() {

  m_pWatchUnderlying->StopWatch();
  m_pWatchUnderlying->OnTrade.Remove( MakeDelegate( this, &ATM::HandleWatchUnderlyingTrade ) );

  m_pfwATM->Write();
  m_pfwATM.reset();

  for ( mapOptionLifeTime_t::value_type& vt: m_mapOptionLifeTime ) {
    m_fEngineOptionStop( vt.second.pOption, m_pWatchUnderlying );
  }
  m_mapOptionLifeTime.clear();

  m_fEngineOptionStart = nullptr;
  m_fEngineOptionStop = nullptr;

  m_fBuildOption = nullptr;

  m_pWatchUnderlying.reset();

  m_mapChains.clear();

}

void ATM::MapAddOption( pOption_t pOption ) {
  pOption->RecordSeries( false );
  m_fEngineOptionStart( pOption, m_pWatchUnderlying );
  const std::string& sName( pOption->GetInstrumentName() );
  mapOptionLifeTime_t::iterator iter = m_mapOptionLifeTime.find( sName );
  if ( m_mapOptionLifeTime.end() == iter ) {
    m_mapOptionLifeTime.emplace( sName, pOption );
  }
  else {
    ++iter->second.count;
  }
}

void ATM::MapDelOption( pOption_t pOption ) {
  m_fEngineOptionStop( pOption, m_pWatchUnderlying );
  const std::string& sName( pOption->GetInstrumentName() );
  mapOptionLifeTime_t::iterator iter = m_mapOptionLifeTime.find( sName );
  if ( m_mapOptionLifeTime.end() == iter ) {
    assert( false );
  }
  else {
    assert( iter->second.count > 0 );
    --iter->second.count;
    if ( 0 == iter->second.count ) {
      m_mapOptionLifeTime.erase( iter );
    }
  }
}

void ATM::HandleWatchUnderlyingTrade( const ou::tf::Trade& trade ) { // TODO: use Quote instead
  if ( m_pTrackATM ) {
    m_pTrackATM->IV( trade.DateTime(), trade.Price() ); // will call back for IvATM timeseries update
  }
}

void ATM::Write() {
  m_pfwATM->Write();
}

} // namespace collect
