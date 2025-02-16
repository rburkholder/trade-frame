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

#include "CollectL2.hpp"

namespace collect {

L2::L2( const std::string& sFilePath, const std::string& sDataPathPrefix, pWatch_t pWatch )
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
    const std::string sFullDataPath( sDataPathPrefix + ou::tf::Quotes::Directory() + pInstrument->GetInstrumentName() );
    m_pfwDepthsByOrder = std::make_unique<fwDepthsByOrder_t>(
      sFilePath, sFullDataPath,
      [this]( ou::tf::HDF5Attributes& attr ){
        SetAttributes( attr, m_pWatch );
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

void L2::Write() {
  m_pfwDepthsByOrder->Write();
}

} // namespace collect
