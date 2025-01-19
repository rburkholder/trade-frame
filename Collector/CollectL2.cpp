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
 * File:    CollectL2.cpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: January 6, 2025 06:18:07
 */

//#include <boost/log/trivial.hpp>

#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include "CollectL2.hpp"

namespace collect {

L2::L2( const std::string& sPathPrefix, pWatch_t pWatch )
{

  // TODO: watch built elsewhere, needs to be restartable for a new day?
  //       or, delete and rebuild for a new day?
  //             this, so that can handle when new front month started

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  pInstrument_t pInstrument( pWatch->GetInstrument() );
  const std::string& sIQFeedSymbolName( pInstrument->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );

  m_pWatch = pWatch;
  m_pWatch->RecordSeries( false ); // record manually in Write()

  {
    const std::string sFullPath( sPathPrefix + ou::tf::Quotes::Directory() + pInstrument->GetInstrumentName() );
    m_pfwDepthsByOrder = std::make_unique<fwDepthsByOrder_t>(
      sFullPath,
      [this]( ou::tf::HDF5Attributes& attr ){
        SetAttributes( attr );
      } );
    m_pWatch->OnDepthByOrder.Add( MakeDelegate( this, &L2::HandleWatchDepthByOrder ) );
  }

  assert( !m_pDispatch );  // trigger on re-entry, need to fix
  m_pDispatch = std::make_unique<ou::tf::iqfeed::l2::Symbols>(
    [ this, &sIQFeedSymbolName ](){
      m_pDispatch->Single( true );
      m_pDispatch->WatchAdd(
        sIQFeedSymbolName,
        [this]( const ou::tf::DepthByOrder& depth ){
          m_cntDepthsByOrder++;
          m_pfwDepthsByOrder->Append( depth );
        }
      );
    }
  );

  m_pWatch->StartWatch();
  m_pDispatch->Connect();
}

L2::~L2() {

  assert( m_pWatch );
  m_pWatch->StopWatch();

  if ( m_pDispatch ) {
    m_pDispatch->WatchDel( m_pWatch->GetInstrumentName() );
    Write();
  }

  if ( m_pDispatch ) {
    m_pDispatch->Disconnect();
  }

  m_pfwDepthsByOrder->Write();
  m_pWatch->OnDepthByOrder.Remove( MakeDelegate( this, &L2::HandleWatchDepthByOrder ) );
  m_pfwDepthsByOrder.reset();

  m_pWatch.reset();

}

void L2::HandleWatchDepthByOrder( const ou::tf::DepthByOrder& dbo ) {
  m_pfwDepthsByOrder->Append( dbo );
}

void L2::SetAttributes( ou::tf::HDF5Attributes& attr ) {
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
  }
  attr.SetProviderType( m_pWatch->GetProvider()->ID() );
  attr.SetMultiplier( pInstrument->GetMultiplier() );
  attr.SetSignificantDigits( pInstrument->GetSignificantDigits() );
}

void L2::Write() {
  m_pfwDepthsByOrder->Write();
}

} // namespace collect
