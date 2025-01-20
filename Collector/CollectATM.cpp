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

#include "CollectATM.hpp"

namespace collect {

ATM::ATM( const std::string& sPathPrefix, pWatch_t pUnderlying )
{

  // TODO: watch built elsewhere, needs to be restartable for a new day?
  //       or, delete and rebuild for a new day?
  //             this, so that can handle when new front month started

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  pInstrument_t pInstrument( pUnderlying->GetInstrument() );

  m_pUnderlying = std::move( pUnderlying );
  m_pUnderlying->RecordSeries( false ); // record manually in Write()

  {
    const std::string sFullPath( sPathPrefix + ou::tf::Quotes::Directory() + pInstrument->GetInstrumentName() );
    m_pfwATM = std::make_unique<fwATM_t>(
      sFullPath,
      [this]( ou::tf::HDF5Attributes& attr ){
        SetAttributes( attr, m_pUnderlying );
      } );
    //m_pOption->OnGreek.Add( MakeDelegate( this, &Greeks::HandleWatchGreeks ) );
  }

  //m_pUnderlying->StartWatch(); // maybe use ticks or quotes to trigger recording atm?
}

ATM::~ATM() {

  assert( m_pUnderlying );
  //m_pUnderlying->StopWatch();

  m_pfwATM->Write();
  //m_pUnderlying->OnGreek.Remove( MakeDelegate( this, &ATM::HandleWatchGreeks ) );
  m_pfwATM.reset();

  m_pUnderlying.reset();

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
