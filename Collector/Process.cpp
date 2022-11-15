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

// start watch on l1, l2
// write on regular intervals

#include <TFTrading/ComposeInstrument.hpp>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
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
, m_sTimeStamp( sTimeStamp )
{
  StartIQFeed();
}

Process::~Process() {

  if ( m_pDispatch ) {
    m_pDispatch->Disconnect();
  }

  if ( m_pWatch ) {
    m_pWatch->StopWatch();
    m_pWatch.reset();
  }

  m_pInstrument.reset();

  m_pComposeInstrumentIQFeed.reset();

  m_piqfeed->Disconnect();
  m_piqfeed.reset();
}

// need control c handler to terminate, as this is an ongoing process

void Process::StartIQFeed() {
  m_piqfeed = ou::tf::iqfeed::IQFeedProvider::Factory();

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
      const std::string& sName( m_choices.m_sSymbolName );
      m_pComposeInstrumentIQFeed->Compose(
        sName,
        [this]( pInstrument_t pInstrument ){
          m_pInstrument = pInstrument;
          StartWatch();
        }
      );
    }
    );
  assert( m_pComposeInstrumentIQFeed );

}

void Process::StartWatch() {

  // TODO: watch built elsewhere, needs to be restartable for a new day?
  //       or, delete and rebuild for a new day?
  //             this, so that can handle when new front month started

  assert( m_pInstrument );
  const std::string& sIQFeedSymbolName( m_pInstrument->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );

  std::cout << "future: "
    << m_pInstrument->GetInstrumentName()
    << ", " << sIQFeedSymbolName
    << std::endl;

  m_pWatch = std::make_shared<ou::tf::Watch>( m_pInstrument, m_piqfeed );

  assert( !m_pDispatch );  // trigger on re-entry, need to fix
  m_pDispatch = std::make_unique<ou::tf::iqfeed::l2::Symbols>(
    [ this, sIQFeedSymbolName ](){
      m_pDispatch->Single( true );
      m_pDispatch->WatchAdd(
        sIQFeedSymbolName,
        [this]( const ou::tf::DepthByOrder& depth ){
          m_depths_byorder.Append( depth );
        }
      );
    }
  );

  m_pWatch->StartWatch();
  m_pDispatch->Connect();
}

void Process::StopWatch() {

  const std::string sPathName = sSaveValuesRoot + "/" + m_sTimeStamp;
  const std::string& sIQFeedSymbolName( m_pInstrument->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );

  std::cout << "  Saving collected values ... " << std::endl;

  if ( m_pDispatch ) {
    m_pDispatch->WatchDel( sIQFeedSymbolName );
      // TODO: need to get the watch in pWatch_t operational
      if ( 0 != m_depths_byorder.Size() ) {
        ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );
        const std::string sPathNameDepth
          = sPathName + ou::tf::DepthsByOrder::Directory()
          + m_pInstrument->GetInstrumentName();
        ou::tf::HDF5WriteTimeSeries<ou::tf::DepthsByOrder> wtsDepths( dm, true, true, 5, 256 );
        wtsDepths.Write( sPathNameDepth, &m_depths_byorder );
        ou::tf::HDF5Attributes attrDepths( dm, sPathNameDepth );
        attrDepths.SetSignature( ou::tf::DepthByOrder::Signature() );
        //attrDepths.SetMultiplier( m_pInstrument->GetMultiplier() );
        //attrDepths.SetSignificantDigits( m_pInstrument->GetSignificantDigits() );
        attrDepths.SetProviderType( m_pWatch->GetProvider()->ID() );
      }
  }

  m_pWatch->StopWatch();
  m_pWatch->SaveSeries( sPathName );

  std::cout << "  ... Done " << std::endl;

}

void Process::Finish() {
  if ( m_pWatch ) {
    StopWatch();
    m_pWatch.reset();
  }
}
