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
 * File:    CollectGreeks.cpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: January 18, 2025 22:04/38
 */

//#include <boost/log/trivial.hpp>

#include "CollectGreeks.hpp"

namespace collect {

Greeks::Greeks( const std::string& sPathPrefix, pOption_t pOption )
{

  // TODO: watch built elsewhere, needs to be restartable for a new day?
  //       or, delete and rebuild for a new day?
  //             this, so that can handle when new front month started

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  pInstrument_t pInstrument( pOption->GetInstrument() );

  m_pOption = pOption;
  m_pOption->RecordSeries( false ); // record manually in Write()

  {
    const std::string sFullPath( sPathPrefix + ou::tf::Quotes::Directory() + pInstrument->GetInstrumentName() );
    m_pfwGreeks = std::make_unique<fwGreeks_t>(
      sFullPath,
      [this]( ou::tf::HDF5Attributes& attr ){
        SetAttributes( attr, m_pOption );
      } );
    m_pOption->OnGreek.Add( MakeDelegate( this, &Greeks::HandleWatchGreeks ) );
  }

  m_pOption->StartWatch();
}

Greeks::~Greeks() {

  assert( m_pOption );
  m_pOption->StopWatch();

  m_pfwGreeks->Write();
  m_pOption->OnGreek.Remove( MakeDelegate( this, &Greeks::HandleWatchGreeks ) );
  m_pfwGreeks.reset();

  m_pOption.reset();

}

void Greeks::HandleWatchGreeks( const ou::tf::Greek& greek ) {
  m_pfwGreeks->Append( greek );
}

void Greeks::Write() {
  m_pfwGreeks->Write();
}

} // namespace collect
