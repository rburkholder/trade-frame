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
, m_bHdf5AttributesSet( false )
, m_cntDepthsByOrder {}
, m_ixDepthsByOrder_Filling( 0 )
, m_ixDepthsByOrder_Writing( 1 )
, m_sPathName( sSaveValuesRoot + "/" + sTimeStamp )
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
  m_piqfeed = ou::tf::iqfeed::Provider::Factory();

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
      // TODO: will need to expand on this, but keep the same for now
      assert( 1 == m_choices.m_vSymbolName.size() );
      const std::string& sName( *m_choices.m_vSymbolName.begin() );
      m_pComposeInstrumentIQFeed->Compose(
        sName,
        [this]( pInstrument_t pInstrument, bool bConstructed ){
          m_pInstrument = std::move( pInstrument );
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

  const std::string& sSymbolName( m_pInstrument->GetInstrumentName() );
  const std::string& sIQFeedSymbolName( m_pInstrument->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );

  std::cout << "future: "
    << sSymbolName
    << ", " << sIQFeedSymbolName
    << std::endl;

  m_pWatch = std::make_shared<ou::tf::Watch>( m_pInstrument, m_piqfeed );

  m_sPathName_Depth
    = m_sPathName + ou::tf::DepthsByOrder::Directory()
    + m_pInstrument->GetInstrumentName();

  assert( !m_pDispatch );  // trigger on re-entry, need to fix
  m_pDispatch = std::make_unique<ou::tf::iqfeed::l2::Symbols>(
    [ this, &sIQFeedSymbolName ](){
      m_pDispatch->Single( true );
      m_pDispatch->WatchAdd(
        sIQFeedSymbolName,
        [this]( const ou::tf::DepthByOrder& depth ){
          m_cntDepthsByOrder++;
          m_rDepthsByOrder[m_ixDepthsByOrder_Filling].Append( depth );
        }
      );
    }
  );

  m_pWatch->StartWatch();
  m_pDispatch->Connect();
}

void Process::StopWatch() {

  m_pWatch->StopWatch();
  m_pWatch->SaveSeries( m_sPathName );

  std::cout << "  ... Done " << std::endl;

}

void Process::Write() {

  assert( 0 == m_rDepthsByOrder[m_ixDepthsByOrder_Writing].Size() );

  rDepthsByOrder_t::size_type ix {};
  ix = m_ixDepthsByOrder_Writing.exchange( 2 ); // take the writing index
  ix = m_ixDepthsByOrder_Filling.exchange( ix ); // and make it the new filling index
  ix = m_ixDepthsByOrder_Writing.exchange( ix ); // and write what was being filled
  assert( 2 == ix );

  //std::cout << "  Saving collected values ... " << std::endl;
  if ( m_pDispatch ) {
    // TODO: need to get the watch in pWatch_t operational
    if ( 0 != m_rDepthsByOrder[m_ixDepthsByOrder_Writing].Size() ) {
      ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );
      ou::tf::HDF5WriteTimeSeries<ou::tf::DepthsByOrder> wtsDepths( dm, true, true, 5, 256 );
      wtsDepths.Write( m_sPathName_Depth, &m_rDepthsByOrder[m_ixDepthsByOrder_Writing] );

      if ( !m_bHdf5AttributesSet ) {
        m_bHdf5AttributesSet= true;
        ou::tf::HDF5Attributes attrDepths( dm, m_sPathName_Depth );
        attrDepths.SetSignature( ou::tf::DepthByOrder::Signature() );
        //attrDepths.SetMultiplier( m_pInstrument->GetMultiplier() );
        //attrDepths.SetSignificantDigits( m_pInstrument->GetSignificantDigits() );
        attrDepths.SetProviderType( m_pWatch->GetProvider()->ID() );
      }

      m_rDepthsByOrder[m_ixDepthsByOrder_Writing].Clear();
    }
  }

}

void Process::Finish() {

  if ( m_pDispatch ) {
    m_pDispatch->WatchDel( m_pInstrument->GetInstrumentName() );
    Write();
  }

  if ( m_pWatch ) {
    StopWatch();
    m_pWatch.reset();
  }

}
