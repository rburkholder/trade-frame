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

//#include <boost/log/trivial.hpp>

#include <TFOptions/Chains.h>

#include "CollectATM.hpp"

namespace collect {

ATM::ATM( const std::string& sPathPrefix, pWatch_t pWatchUnderlying,
          fBuildOption_t&& fBuildOption, fGatherOptions_t&& fGatherOptions )
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
    //m_pOption->OnGreek.Add( MakeDelegate( this, &Greeks::HandleWatchGreeks ) );
  }

  m_pWatchUnderlying->OnTrade.Add( MakeDelegate( this, &ATM::HandleWatchUnderlyingTrade ) );
  m_pWatchUnderlying->StartWatch(); // maybe use ticks or quotes to trigger recording atm?

  assert( fGatherOptions );
  fGatherOptions(
    m_pWatchUnderlying->GetInstrument(),
    [this]( pInstrument_t pInstrumentOption ){
      // find existing expiry, or create new one
      mapChains_t::iterator iterChains = ou::tf::option::GetChain( m_mapChains, pInstrumentOption );
      assert( m_mapChains.end() != iterChains );

      // update put/call@strike with option
      chain_t& chain( iterChains->second );
      Instance* pEntry
        = ou::tf::option::UpdateOption<chain_t,Instance>( chain, pInstrumentOption );
      pEntry->pInstrument = pInstrumentOption; // put / call as appropriate
    } );
}

ATM::~ATM() {

  m_pWatchUnderlying->StopWatch();
  m_pWatchUnderlying->OnTrade.Remove( MakeDelegate( this, &ATM::HandleWatchUnderlyingTrade ) );

  m_pfwATM->Write();
  //m_pUnderlying->OnGreek.Remove( MakeDelegate( this, &ATM::HandleWatchGreeks ) );
  m_pfwATM.reset();

  m_pWatchUnderlying.reset();

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
